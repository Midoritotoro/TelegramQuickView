from telethon import TelegramClient

def isUserAuthorized(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
        telegramClient.connect()
        telegramClient.disconnect()
    except:
        return False
    return True