from telethon import TelegramClient
import asyncio

def sendTelegramCode(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        asyncio.get_event_loop()
    except:
        asyncio.set_event_loop(asyncio.new_event_loop())
        
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
        phoneCode = telegramClient.send_code_request(phoneNumber)
        # telegramClient.sign_in(phoneNumber, phoneCode)
    except:
        return False
    
    return True
       

def isTelegramCredentialsValid(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        asyncio.get_event_loop()
    except:
        asyncio.set_event_loop(asyncio.new_event_loop())
        
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
        telegramClient.connect()
        telegramClient.disconnect()
    except:
        return False
    
    return True

def isTelegramPhoneNumberCodeValid(apiId: int, phoneNumber: str, apiHash: str, code: int) -> bool:
    try:
        asyncio.get_event_loop()
    except:
        asyncio.set_event_loop(asyncio.new_event_loop())
        
    try:
        asyncio.set_event_loop(asyncio.new_event_loop())
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)   
        telegramClient.connect()
        # telegramClient.sign_in(phoneNumber, code=code, phone_code_hash=phone_code_hash)
        telegramClient.disconnect()
    except:
        return False
    
    return True


# const char* apiHash = "019edf3f20c8460b741fb94114e6fec0";
# const char* phoneNumber = "+375292384917";
# long long apiId = 13711370;