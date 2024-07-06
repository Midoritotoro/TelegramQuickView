from telethon import TelegramClient
import asyncio

async def sendTelegramCode(apiId: int, phoneNumber: str, apiHash: str) -> bool:   
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
        await telegramClient.connect()
        await telegramClient.send_code_request(phoneNumber)
        await telegramClient.disconnect()
    except:
        return False
    return True
       

async def isTelegramCredentialsValid(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
        await telegramClient.connect()
        await telegramClient.disconnect()
    except:
        return False
    return True

async def isTelegramPhoneNumberCodeValid(apiId: int, phoneNumber: str, apiHash: str, code: int) -> bool:
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)   
        await telegramClient.connect()
        await telegramClient.sign_in(phoneNumber, code)
        await telegramClient.disconnect()
    except Exception as e:
        print(e)
        return False
    return True

def asyncCall(functionName: str, *args) -> bool:
    functions = {"sendTelegramCode": sendTelegramCode, "isTelegramCredentialsValid": isTelegramCredentialsValid, "isTelegramPhoneNumberCodeValid": isTelegramPhoneNumberCodeValid}
    return asyncio.run((functions[functionName])(*args))
