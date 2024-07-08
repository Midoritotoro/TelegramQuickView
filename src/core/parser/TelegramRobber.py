from datetime import datetime, timezone
from dateutil.tz import tzlocal
from telethon.sync import TelegramClient
import asyncio
from pathlib import Path
from shutil import rmtree
import json
import asyncio


class Sleuth:
    def __init__(
            self,
            pathToSettingsJsonFile: str,
            pathToAppRootDirectory: str
    ):
        self.pathToSettingsJsonFile = pathToSettingsJsonFile
        telegramCredentials = self.getTelegramCredentials()
        self.api_id = telegramCredentials.get("apiId")
        self.api_hash = telegramCredentials.get("apiHash")
        self.phone_number = telegramCredentials.get("phoneNumber") 
        self.group_username = "antifishechki"
        self.__pathToAppRootDirectory = pathToAppRootDirectory

        self.__download_paths = [
            f"{self.__pathToAppRootDirectory}/Изображения",
            f"{self.__pathToAppRootDirectory}/Видео",
            f"{self.__pathToAppRootDirectory}/Аудио",
            f"{self.__pathToAppRootDirectory}/Документы",
            f"{self.__pathToAppRootDirectory}/Остальное",
            f"{self.__pathToAppRootDirectory}/Текст"
        ]
        
        self.__client = TelegramClient('TelegramQuickView', self.api_id, self.api_hash, timeout=10)
        
    def getTelegramCredentials(self) -> dict[str, str]:
        with open(self.pathToSettingsJsonFile, "r", encoding="utf-8") as jsonFile:
            data = json.load(jsonFile)
        return data

    async def get_messages(self):
        tasks = []
        export = []
        async for message in self.__client.iter_messages(
                self.group_username
                ):
            
            if not message:
                break 

            if message.sender is not None and message.text is not None:
                clean_message = message.text

                if message.file is not None:
                    file_type = message.file.mime_type.split('/')[0]
                    download_path = await self.__get_download_path(file_type)
                    tasks.append(asyncio.create_task(self.__client.download_media(message, file=download_path)))
                    export.append(asyncio.create_task(self.export_to_txt(clean_message, f"{self.__download_paths[5]}/{f"{message.date.astimezone(tzlocal())}".replace(" ", "_").rsplit("+", 1)[0].replace(":", "_")}.txt")))

                if len(tasks) == 50:
                    await asyncio.gather(*tasks, *export)
                    tasks.clear()
                    export.clear()

        await asyncio.gather(*tasks)
        await self.export_to_txt(f"{self.__download_paths[5]}/{f"{message.date.astimezone(tzlocal())}".replace(" ", "_").rsplit("+", 1)[0].replace(":", "_")}.txt")

    async def dig(self) -> str:
        try:
            await self.__check_download_path()
            await self.__client.connect()
            code, codeHash = await self.getTelegramCode()
        
            if not await self.__client.is_user_authorized():
                await self.__client.sign_in(self.phone_number, code, phone_code_hash=codeHash)

            await self.get_messages()
            await self.__client.disconnect()

        except Exception as e:
            print(e)

    async def export_to_txt(self, message: str, output_path: str) -> None:
        with open(output_path, "w", newline="", encoding="utf-8") as file:
            file.write(message)

    async def __get_download_path(self, file_type) -> str:
        return {
            'image': self.__download_paths[0],
            'video': self.__download_paths[1],
            'audio': self.__download_paths[2],
            'documents': self.__download_paths[3],
        }.get(file_type, self.__download_paths[4])

    async def __check_download_path(self) -> None:
        if Path(self.__pathToAppRootDirectory).exists():
            rmtree(self.__pathToAppRootDirectory)
        if not Path(self.__pathToAppRootDirectory).exists():
            Path(self.__pathToAppRootDirectory).mkdir(parents=True)
        for download_path in self.__download_paths:
            Path(download_path).mkdir(parents=True, exist_ok=True)
     
    async def getTelegramCode(self) -> tuple[str | None, str | None]:
        with open(self.pathToSettingsJsonFile, "r", encoding="utf-8") as jsonFile:
            data = json.load(jsonFile)
        return data.get("code"), data.get("codeHash")
            
  
if __name__ == "__main__":
    telegramParser = Sleuth("C:/Users/danya/AppData/Roaming/TelegramQuickView/userData.json", "D:/Media")
    asyncio.run(telegramParser.dig())
            