from telethon import TelegramClient
import asyncio
from typing import Callable

async def sendTelegramCode(apiId: int, phoneNumber: str, apiHash: str) -> bool:
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
        phoneCode = await telegramClient.send_code_request(phoneNumber)
        # telegramClient.sign_in(phoneNumber, phoneCode)
    except Exception as e:
        print(e)
        return False
    
    return True
       

async def isTelegramCredentialsValid(apiId: int, phoneNumber: str, apiHash: str) -> bool:      
    try:
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)
        await telegramClient.connect()
        await telegramClient.disconnect()
    except Exception as e:
        print(e)
        return False
    
    return True

async def isTelegramPhoneNumberCodeValid(apiId: int, phoneNumber: str, apiHash: str, code: int) -> bool:
    try:
        asyncio.set_event_loop(asyncio.new_event_loop())
        telegramClient = TelegramClient("AuthorizationCheck", apiId, apiHash, timeout=10)   
        await telegramClient.connect()
        await telegramClient.sign_in(phoneNumber, code)
        await telegramClient.disconnect()
    except Exception as e:
        print(e)
        return False
    
    return True

async def func():
    print("FFF")

class coro:
    def __init__(self, functionName: str, *args):
        functions: dict[str, Callable] = {"sendTelegramCode": sendTelegramCode, "isTelegramCredentialsValid": isTelegramCredentialsValid, "isTelegramPhoneNumberCodeValid": isTelegramPhoneNumberCodeValid}
        self.__state = 0
        self.__callable = functions[functionName]
        self.__args = args
        print(self.__callable.__name__)

    def __iter__(self):
        return self

    def __await__(self):
        return self

    def __next__(self):
        if self.__state == 0:
            self._callable_iter = self.__callable(*self.__args).__await__()
            self.__state = 1
        if self.__state == 1:
            try:
                suspend_val = next(self._callable_iter)
                return suspend_val
            except StopIteration as stop:
                y = stop.value
        raise RuntimeError("cannot reuse already awaited coroutine")

if __name__ == "__main__":
    def main():
        coro_obj = coro("isTelegramCredentialsValid", "1214", "4535q 4", 54354)
    asyncio.run(func.__await__())

# const char* apiHash = "019edf3f20c8460b741fb94114e6fec0";
# const char* phoneNumber = "+375292384917";
# long long apiId = 13711370;