from telethon import TelegramClient
import asyncio
import json


async def sendTelegramCode(apiId: int, phoneNumber: str, apiHash: str, pathToUserSettingsJson: str) -> bool:   
    try:
        telegramClient = TelegramClient("TelegramQuickView", apiId, apiHash, timeout=10)
        if telegramClient.disconnected:
            await telegramClient.connect()
            
        code = await telegramClient.send_code_request(phoneNumber)
        
        with open(pathToUserSettingsJson, "r", encoding="utf-8") as jsonFile:
           data = json.load(jsonFile)
        data["codeHash"] = code.phone_code_hash
        
        with open(pathToUserSettingsJson, "w", encoding="utf-8") as jsonFile:
           json.dump(data, jsonFile)
           
        await telegramClient.disconnect()
    except:
        return False
    return True

async def isTelegramCredentialsValid(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        telegramClient = TelegramClient("TelegramQuickView", apiId, apiHash, timeout=10)
        if telegramClient.disconnected:
            await telegramClient.connect()
        await telegramClient.disconnect()
    except:
        return False
    return True

async def isTelegramPhoneNumberCodeValid(apiId: int, phoneNumber: str, apiHash: str, code: int, codeHash: str) -> bool:
    try:
        telegramClient = TelegramClient("TelegramQuickView", apiId, apiHash, timeout=10)
        if telegramClient.disconnected:
            await telegramClient.connect()
            
        if not await telegramClient.is_user_authorized():
            await telegramClient.sign_in(phoneNumber, code, phone_code_hash=codeHash)
            
        await telegramClient.disconnect()
    except:
        return False
    return True

def asyncCall(functionName: str, *args) -> bool:
    functions = {"sendTelegramCode": sendTelegramCode, "isTelegramCredentialsValid": isTelegramCredentialsValid, "isTelegramPhoneNumberCodeValid": isTelegramPhoneNumberCodeValid}
    return asyncio.run((functions[functionName])(*args))
