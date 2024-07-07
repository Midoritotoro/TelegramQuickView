from telethon import TelegramClient
import asyncio
import json


async def sendTelegramCode(apiId: int, phoneNumber: str, apiHash: str, pathToUserSettingsJson: str) -> bool:   
    try:
        telegramClient = TelegramClient("TelegramQuickView", apiId, apiHash, timeout=10)
        await telegramClient.connect()
        code = await telegramClient.send_code_request(phoneNumber)
        with open(pathToUserSettingsJson, "r", encoding="utf-8") as jsonFile:
           data = json.load(jsonFile)
        data["codeHash"] = code.phone_code_hash
        with open(pathToUserSettingsJson, "w", encoding="utf-8") as jsonFile:
           json.dump(data, jsonFile)
        await telegramClient.disconnect()
    except Exception as e:
        print(e)
        return False
    return True
       

async def isTelegramCredentialsValid(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        telegramClient = TelegramClient("TelegramQuickView", apiId, apiHash, timeout=10)
        await telegramClient.connect()
        await telegramClient.disconnect()
    except Exception as e:
        print(e)
        return False
    return True

async def isTelegramPhoneNumberCodeValid(apiId: int, phoneNumber: str, apiHash: str, code: int, codeHash: str) -> bool:
    try:
        telegramClient = TelegramClient("TelegramQuickView", apiId, apiHash, timeout=10)   
        await telegramClient.connect()
        await telegramClient.sign_in(phoneNumber, code, phone_code_hash=codeHash)
        await telegramClient.disconnect()
    except Exception as e:
        print(e)
        return False
    return True

def asyncCall(functionName: str, *args) -> bool:
    functions = {"sendTelegramCode": sendTelegramCode, "isTelegramCredentialsValid": isTelegramCredentialsValid, "isTelegramPhoneNumberCodeValid": isTelegramPhoneNumberCodeValid}
    return asyncio.run((functions[functionName])(*args))


if __name__ == "__main__":
    asyncCall("sendTelegramCode", 13711370, "+375292384917", "019edf3f20c8460b741fb94114e6fec0", "C:/Users/danya/AppData/Roaming/TelegramQuickView/userData.json")
#  apiHash = "019edf3f20c8460b741fb94114e6fec0"
#  phoneNumber = "+375292384917"
#  apiId = 13711370

