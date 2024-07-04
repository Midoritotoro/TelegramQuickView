from telethon import TelegramClient

def isUserAuthorized(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
    except:
        return False
    return True

# const char* apiHash = "019edf3f20c8460b741fb94114e6fec0";
# const char* phoneNumber = "+375292384917";
# long long apiId = 13711370;