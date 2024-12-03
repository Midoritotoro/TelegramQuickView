#pragma once

#include "AbstractTelegramParser.h"

#include <QMutex>
#include <QWaitCondition>

#include <set>

class TelegramParser: public AbstractTelegramParser {
	Q_OBJECT
private:
	std::map<int64_t, td::td_api::object_ptr<td::td_api::user>> users_;
	std::map<int64_t, std::string> chat_title_;

	std::set<int64_t> _parsedMessages;

	std::map<int32_t, Telegram::Message> _downloadingMessages;
	std::vector<Telegram::Message> _downloadedMessages;

	QWaitCondition _waitCondition;
	QMutex _mutex;

	int64_t _nextRawChat = 0;

	QStringList _targetChannelsList;
	std::set<int64_t> _chats;

	int _downloadedFiles = 0;
	int _totalFilesToDownload = 0;

	int _countOfLatestDownloadingMessages = 1;
	bool _downloadSensitiveContent;
public:
	TelegramParser();
	~TelegramParser();

	[[nodiscard]] Telegram::Message loadMessage();
Q_SIGNALS:
	void messagesLoaded();
protected:
	void processResponse(td::ClientManager::Response response) override;
private:
	[[nodiscard]] bool isMediaGrouped(td::td_api::object_ptr<td::td_api::message>&& message);
	[[nodiscard]] Telegram::Message parseMessageContent(
		td::td_api::message& sourceMessage, 
		int64_t& destinationMediaId);

	void startChatsChecking();

	auto createHistoryRequestHandler();
	auto createFileDownloadQueryHandler(const int index);
	void checkFileDownloadError(Object object);

	std::string getUserName(std::int64_t user_id) const;
    std::string getChatTitle(std::int64_t chat_id) const;

	void on_NewMessageUpdate(Object update);
};