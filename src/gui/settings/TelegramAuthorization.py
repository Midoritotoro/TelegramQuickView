from telethon import TelegramClient
import asyncio

def isUserAuthorized(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        asyncio.set_event_loop(asyncio.new_event_loop())
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
    except:
        return False
    return True

# const char* apiHash = "019edf3f20c8460b741fb94114e6fec0";
# const char* phoneNumber = "+375292384917";
# long long apiId = 13711370;