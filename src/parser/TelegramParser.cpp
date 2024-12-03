#include "TelegramParser.h"

#include <QMutex>
#include "../media/ffmpeg/Guard.h"

#include "../core/Time.h"

#include <future>


TelegramParser::TelegramParser() :
    AbstractTelegramParser()
    , _downloadSensitiveContent(_userDataManager->getDownloadSensitiveContentAbility())
    , _countOfLatestDownloadingMessages(_userDataManager->getCountOfLatestDownloadingMessages())
{
    foreach(const auto & element, _userDataManager->getUsernamesOfTargetChannels())
        _targetChannelsList.append(element.toString());

    foreach(const auto & channel, _targetChannelsList) {
        sendQuery(td::td_api::make_object<td::td_api::searchPublicChat>(channel.toStdString()),
            [this](Object object) {
                if (object == nullptr)
                    return;

                if (object->get_id() == td::td_api::error::ID)
                    handleError(td::move_tl_object_as<td::td_api::error>(object));

                if (const auto chat = td::move_tl_object_as<td::td_api::chat>(object))
                    _userDataManager->setTargetChannelsChatIds(QList<qint64>({ chat->id_ }));
            }
        );
    }

    connect(this, &AbstractTelegramParser::userAuthorized, [this]() {
        moveToThread(_thread);
        if (!_thread->isRunning())
            _thread->start();
        });

    foreach(const auto & id, _userDataManager->getIdsOfTargetChannels())
        _chats.emplace(id.toLongLong());

    moveToThread(_thread);
    if (!_thread->isRunning())
        _thread->start();
}

TelegramParser::~TelegramParser() {
    _thread->quit();
}

auto TelegramParser::createFileDownloadQueryHandler(const int index) {
    return [this, index](Object object) {
        if (object == nullptr || object->get_id() != td::td_api::file::ID)
            return checkFileDownloadError(std::move(object));

        auto file = td::move_tl_object_as<td::td_api::file>(object);
        if (file == nullptr || file->size_ == 0)
            return;

        auto it = _downloadingMessages.find(file->id_);

        if (it != _downloadingMessages.end()) {
            if (file->local_->is_downloading_completed_) {
                it->second.attachments.append(file->local_->path_.c_str());

                if (!_downloadedMessages.empty() && index < _downloadedMessages.size()) {
                    _downloadedMessages[index].
                        attachments.append(file->local_->path_.c_str());
                }
                else {
                    _downloadedMessages.push_back(Telegram::Message());
                    _downloadedMessages[0].
                        attachments.append(file->local_->path_.c_str());
                }
            }
            _downloadingMessages.erase(it);
        }
        };
}

auto TelegramParser::createHistoryRequestHandler() {
    return [this](Object object) {
        if (object == nullptr || object->get_id() == td::td_api::error::ID)
            return checkFileDownloadError(std::move(object));

        auto messages = td::move_tl_object_as<td::td_api::messages>(object);
        if (messages == nullptr || messages->total_count_ == 0)
            return;

        qDebug() << "messages->total_count_: " << messages->total_count_;
        _downloadedMessages.clear();

        for (auto index = 0, countOfDownloadedMessages = 0; index < messages->total_count_; ++index) {
            qDebug() << "countOfDownloadedMessages: " << countOfDownloadedMessages;

            if (countOfDownloadedMessages > _countOfLatestDownloadingMessages)
                break;

            auto message = std::move(messages->messages_[index]);
            const auto messageId = message->id_;

            if (_parsedMessages.find(messageId) != _parsedMessages.end())
                continue;

            int64_t mediaId = 0;

            const auto currentMessage = parseMessageContent(*message, mediaId);
            const auto mediaGrouped = isMediaGrouped(std::exchange(message, {}));

            _downloadedMessages.push_back(currentMessage);

            qDebug() << "messageId: " << messageId << "messageMediaAlbumId: " << currentMessage.mediaAlbumId << " from: " << currentMessage.sender
                << "isMediaGrouped: " << mediaGrouped;


            if (mediaId != 0) {
                if (mediaGrouped)
                    --countOfDownloadedMessages;
                _downloadingMessages[mediaId] = currentMessage;

                const auto attachmentsSize = currentMessage.attachments.size();
                sendQuery(
                    td::td_api::make_object<td::td_api::downloadFile>(mediaId, 32, 0, 0, true),
                    createFileDownloadQueryHandler(_downloadedMessages.size() - 1)
                );

                while (_downloadedMessages[_downloadedMessages.size() - 1].attachments.size() == attachmentsSize) {
                    processResponse(_clientManager->receive(10));
                }
            }

            _parsedMessages.emplace(messageId);
            ++countOfDownloadedMessages;
        }

        auto iterator = _chats.find(_nextRawChat);

        _nextRawChat != 0
            ? iterator != _chats.end()
                ? _nextRawChat = (++iterator != _chats.end() ? *iterator : 0)
                : _nextRawChat = 0
            :
              _nextRawChat = _chats.empty() ? 0 : *_chats.begin();

        emit messagesLoaded();
        };
}

Telegram::Message TelegramParser::loadMessage() {
    if (!_downloadedMessages.empty()) {
        auto message = _downloadedMessages.back();
        _downloadedMessages.pop_back();

        return message;
    }

    if (_nextRawChat == 0)
        _nextRawChat = *_chats.begin();

    sendQuery(
        td::td_api::make_object<td::td_api::getChatHistory>(*_chats.find(_nextRawChat), 0,
            -Telegram::maximumMessageAttachmentsCount * _countOfLatestDownloadingMessages, 100, false),
        createHistoryRequestHandler()
    );

    bool needBreak = false;
    connect(this, &TelegramParser::messagesLoaded, [=, &needBreak]() {
        needBreak = true;
        });

    while (!needBreak)
        processResponse(_clientManager->receive(10));

    if (_downloadedMessages.empty())
        return Telegram::Message();

    auto message = _downloadedMessages.back();
    _downloadedMessages.pop_back();

    return message;
}

bool TelegramParser::isMediaGrouped(td::td_api::object_ptr<td::td_api::message>&& message) {
    if (message == nullptr || message->media_album_id_ == 0)
        return false;

    qDebug() << "isMediaGrouped is called";
    QMutexLocker locker(&_mutex);
    bool mediaGrouped = false;

    const auto targetAlbumId = message->media_album_id_;

    const auto handler = [=, &mediaGrouped](Object object) {
        qDebug() << "handler called";
        if (object == nullptr || object->get_id() == td::td_api::error::ID) {
            qDebug() << "handler: messages == nullptr || messages->total_count_ == 0";
            return checkFileDownloadError(std::move(object));
        }

        auto messages = td::move_tl_object_as<td::td_api::messages>(object);
        if (messages == nullptr || messages->total_count_ == 0) {
            qDebug() << "handler: messages == nullptr || messages->total_count_ == 0";
            return;
        }

        qDebug() << "TelegramParser::isMediaGrouped: messages->count: " << messages->total_count_;

        for (auto index = 0; index < messages->total_count_; ++index) {
            qDebug() << "messages->messages_[index]->media_album_id_: " << messages->messages_[index]->media_album_id_
                << "targetAlbumId: " << targetAlbumId;

            if (messages->messages_[index]->media_album_id_ == targetAlbumId) {
                mediaGrouped = true;
                qDebug() << "TelegramParser::isMediaGrouped: " << mediaGrouped;
                return;
            }
        }
        };

    sendQuery(
        td::td_api::make_object<td::td_api::getChatHistory>(
            message->chat_id_, 0, -Telegram::maximumMessageAttachmentsCount,
            100, false),
        handler
    );

    auto index = 0;

    while (index < 50 && mediaGrouped == false) {
        processResponse(_clientManager->receive(10));
        ++index;
    }

    return mediaGrouped;
}

Telegram::Message TelegramParser::parseMessageContent(
    td::td_api::message& sourceMessage,
    int64_t& destinationMediaId)
{
    Telegram::Message message;

    message.mediaAlbumId = sourceMessage.media_album_id_;
    message.date = Time::formattedUnixTime(sourceMessage.date_);

    td::td_api::downcast_call(*sourceMessage.sender_id_,
        Telegram::overloaded(
            [this, &message](td::td_api::messageSenderUser& user) {
                message.sender = getUserName(user.user_id_).c_str();
            },
            [this, &message](td::td_api::messageSenderChat& chat) {
                message.sender = getChatTitle(chat.chat_id_).c_str();
            }
        )
    );

    switch (sourceMessage.content_->get_id()) {
    case td::td_api::messageText::ID:
        message.text += static_cast<td::td_api::messageText&>(*sourceMessage.content_).text_->text_.c_str();
        break;

    case td::td_api::messagePhoto::ID:
        destinationMediaId = static_cast<td::td_api::messagePhoto&>(*sourceMessage.content_).photo_->sizes_.back()->photo_->id_;
        message.text += static_cast<td::td_api::messagePhoto&>(*sourceMessage.content_).caption_->text_.c_str();
        break;

    case td::td_api::messageVideo::ID:
        destinationMediaId = static_cast<td::td_api::messageVideo&>(*sourceMessage.content_).video_->video_->id_;
        message.text += static_cast<td::td_api::messageVideo&>(*sourceMessage.content_).caption_->text_.c_str();
        break;

    case td::td_api::messageDocument::ID:
        destinationMediaId = static_cast<td::td_api::messageDocument&>(*sourceMessage.content_).document_->document_->id_;
        message.text += static_cast<td::td_api::messageDocument&>(*sourceMessage.content_).caption_->text_.c_str();
        break;
    }

    return message;
}

void TelegramParser::startChatsChecking() {
    while (true)
        processResponse(_clientManager->receive(10));
}

void TelegramParser::checkFileDownloadError(Object object) {
    if (object == nullptr || object->get_id() != td::td_api::error::ID)
        return;

    const auto error = td::move_tl_object_as<td::td_api::error>(object);
    qDebug() << "Error: " << to_string(error);

    handleError(error);
    on_NewMessageUpdate(std::move(object));
}

void TelegramParser::processResponse(td::ClientManager::Response response) {
    if (!response.object)
        return;

    if (response.request_id == 0)
        return on_NewMessageUpdate(std::move(response.object));

    auto iterator = _handlers.find(response.request_id);

    if (iterator != _handlers.end()) {
        iterator->second(std::move(response.object));
        _handlers.erase(iterator);
    }
}

std::string TelegramParser::getUserName(std::int64_t user_id) const {
    auto it = users_.find(user_id);

    return it == users_.end() ?
        "unknown user"
        : it->second->first_name_ +
        " " + it->second->last_name_;
}

std::string TelegramParser::getChatTitle(std::int64_t chat_id) const {
    auto it = chat_title_.find(chat_id);

    return it == chat_title_.end()
        ? "unknown chat"
        : it->second;
}

void TelegramParser::on_NewMessageUpdate(td::td_api::object_ptr<td::td_api::Object> update) {
    td::td_api::downcast_call(
        *update, Telegram::overloaded(
            [this](td::td_api::updateNewChat& update_new_chat) {
                chat_title_[update_new_chat.chat_->id_] = update_new_chat.chat_->title_;
            },
            [this](td::td_api::updateChatTitle& update_chat_title) {
                chat_title_[update_chat_title.chat_id_] = update_chat_title.title_;
            },
            [this](td::td_api::updateUser& update_user) {
                const auto user_id = update_user.user_->id_;
                users_[user_id] = std::move(update_user.user_);
            },
            [this](td::td_api::updateNewMessage& update_new_message) {
                if (update_new_message.message_->has_sensitive_content_ && _downloadSensitiveContent == false) {
                    qDebug() << "Message has a 18+ content, so it`s has been skipped";
                    return;
                }

                if (_chats.find(update_new_message.message_->chat_id_) == _chats.end())
                    return;

                if (_parsedMessages.find(update_new_message.message_->id_) != _parsedMessages.end())
                    return;

                _parsedMessages.emplace(update_new_message.message_->id_);

                int64_t mediaId = 0;
                auto message = parseMessageContent(*update_new_message.message_, mediaId);

                if (mediaId == 0) {
                    _downloadedMessages.push_back(message);
                    return emit messagesLoaded();
                }

                if (!_downloadingMessages.empty()) {
                    const auto previousDownloadedMessage = _downloadingMessages.end()->second;
                    if (!message.isNull()) {
                        if (message.mediaAlbumId == 0 || (message.mediaAlbumId != 0 &&
                            previousDownloadedMessage.mediaAlbumId != message.mediaAlbumId)) {
                            _downloadedMessages.push_back(previousDownloadedMessage);
                            emit messagesLoaded();
                        }
                    }
                }

                _downloadingMessages[mediaId] = message;

                sendQuery(
                    td::td_api::make_object<td::td_api::downloadFile>(mediaId, 32, 0, 0, true),
                    createFileDownloadQueryHandler(_downloadedMessages.size() - 1)
                );
            },
            [](auto& update) {

            }
        )
    );

    processUpdate(std::move(update));
}