#pragma once

#include "AbstractTelegramParser.h"
#include "data/PostSqlManager.h"

#include <QMutex>
#include <QWaitCondition>

class TelegramParser: public AbstractTelegramParser {
	Q_OBJECT
private:
	std::map<int64_t, td::td_api::object_ptr<td::td_api::user>> users_;
	std::map<int64_t, std::string> chat_title_;

	std::map<int32_t, Telegram::Message> _downloadingMessages;
	std::vector<Telegram::Message> _downloadedMessages;

	QStringList _targetChannelsList;
	std::vector<long long> _chatIdsVector;
	
	QMutex _mutex;
	QWaitCondition _waitCondition;

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
	void parseMessageContent(td::td_api::object_ptr<td::td_api::message>&& message, std::string& text, int64_t& mediaId);

	void startChatsChecking();

	auto createHistoryRequestHandler();
	auto createFileDownloadQueryHandler();
	void checkFileDownloadError(Object object);

	std::string getUserName(std::int64_t user_id) const;
    std::string getChatTitle(std::int64_t chat_id) const;

	void on_NewMessageUpdate(td::td_api::object_ptr <td::td_api::Object> update);
	std::string convertTdMessageTimestamp(int64_t time);
};