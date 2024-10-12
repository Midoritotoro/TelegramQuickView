#pragma once

#include "AbstractTelegramParser.h"
#include "PostSqlManager.h"


class TelegramParser: public AbstractTelegramParser {
private:
	std::map<std::int64_t, td::td_api::object_ptr<td::td_api::user>> users_;
	std::map<std::int64_t, std::string> chat_title_;

	std::unique_ptr<PostSqlManager> _sqlManager = nullptr;

	QStringList _targetChannelsList;
public:
	TelegramParser();
protected:
	void processResponse(td::ClientManager::Response response) override;
private:
	void startChatsChecking();

	auto createFileDownloadQueryHandler();
	void checkFileDownloadError(Object object);

	std::string getUserName(std::int64_t user_id) const;
    std::string getChatTitle(std::int64_t chat_id) const;

	void on_NewMessageUpdate(td::td_api::object_ptr <td::td_api::Object> update);

	std::uint64_t nextDownloadQueryId();
};