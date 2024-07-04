from telethon import TelegramClient

def isUserAuthorized(apiHash: str, apiId: int, phoneNumber: str) -> bool:
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
        telegramClient.connect()
        telegramClient.disconnect()
    except:
        return False
    return True