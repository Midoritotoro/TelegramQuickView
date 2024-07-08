from datetime import datetime, timezone
from dateutil.tz import tzlocal
from telethon import TelegramClient, events
from telethon.tl.types import InputPeerChannel, PeerChannel
from telethon.errors import PeerFloodError
import asyncio
from pathlib import Path
from shutil import rmtree
import json

channels = ["test329483", "testssss352"]


class Sleuth:
    def __init__(
            self,
            pathToSettingsJsonFile: str,
            pathToAppRootDirectory: str
    ):
        self.postIndex = 0
        self.pathToSettingsJsonFile = pathToSettingsJsonFile
        telegramCredentials = self.__getUserSettings()
        
        self.api_id = telegramCredentials.get("apiId")
        self.api_hash = telegramCredentials.get("apiHash")
        self.phone_number = telegramCredentials.get("phoneNumber") 
        self.lastPostsCount = telegramCredentials.get("lastPostsCount")
        self.code = telegramCredentials.get("code")
        self.codeHash = telegramCredentials.get("codeHash")
        
        self.__pathToAppRootDirectory = pathToAppRootDirectory
        
        self.__client = TelegramClient('TelegramQuickView', self.api_id, self.api_hash, timeout=10)
        
    async def __updatePaths(self, set: bool, username: str = None) -> None:
        self.postIndex += 1
        
        if set:
            self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectory + f"/{username}/" + f"{self.postIndex}"
        else:
            self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectoryContent[:-1] + f"{self.postIndex}"
            
        self.__download_paths = [
            f"{self.__pathToAppRootDirectory}/Изображения",
            f"{self.__pathToAppRootDirectory}/Видео",
            f"{self.__pathToAppRootDirectory}/Аудио",
            f"{self.__pathToAppRootDirectory}/Документы",
            f"{self.__pathToAppRootDirectory}/Остальное",
            f"{self.__pathToAppRootDirectory}/Текст"
        ]
        
    def __getUserSettings(self) -> dict[str, str]:
        with open(self.pathToSettingsJsonFile, "r", encoding="utf-8") as jsonFile:
            data = json.load(jsonFile)
        return data

    async def __get_messages(self, username: str):
        self.postIndex = 0
        tasks = []
        export = []
        async for message in self.__client.iter_messages(
                username,
                limit = self.lastPostsCount
        ):
         
            if not message:
                break
            
            if self.postIndex == 0:
                await self.__updatePaths(True, username)
            else:
                await self.__updatePaths(False)

            if message.sender is not None and message.text is not None:
                clean_message = message.text

                if message.file is not None:
                    file_type = message.file.mime_type.split('/')[0]
                    await self.__check_download_path()
                    download_path = await self.__get_download_path(file_type)
                    tasks.append(asyncio.create_task(message.download_media(file=download_path)))
                    export.append(asyncio.create_task(self.__export_to_txt(clean_message, f"{self.__download_paths[5]}/{self.postIndex}.txt")))

                if len(tasks) == self.lastPostsCount:
                    await asyncio.gather(*tasks, *export)
                    tasks.clear()
                    export.clear()

    async def __export_to_txt(self, message: str, output_path: str) -> None:
        with open(output_path, "w", encoding="utf-8") as file:
            file.write(message)

    async def __get_download_path(self, file_type) -> str:
        return {
            'image': self.__download_paths[0],
            'video': self.__download_paths[1],
            'audio': self.__download_paths[2],
            'documents': self.__download_paths[3],
        }.get(file_type, self.__download_paths[4])

    async def __check_download_path(self) -> None:
        if Path(self.__pathToAppRootDirectoryContent).exists():
            rmtree(self.__pathToAppRootDirectoryContent)
        if not Path(self.__pathToAppRootDirectoryContent).exists():
            Path(self.__pathToAppRootDirectoryContent).mkdir(parents=True)
        for download_path in self.__download_paths:
            Path(download_path).mkdir(parents=True, exist_ok=True)

    async def checkAndParseTelegramChannels(self) -> None:
        await self.__client.connect()
        if not await self.__client.is_user_authorized():
            await self.__client.sign_in(self.phone_number, self.code, phone_code_hash=self.codeHash)
                
        # while True:
            
            # await self.get_messages(self.group_username)
            # await asyncio.sleep(60)
        for channel in channels:
            await self.createChannelHandler(channel)
        # await self.__client.disconnect()
        await self.__client.run_until_disconnected()
     
        
    async def createChannelHandler(self, username: str):
        channel_entity = await self.__client.get_input_entity(username)
        if isinstance(channel_entity, InputPeerChannel):
            @self.__client.on(events.NewMessage(chats=channel_entity))
            async def handler(event):
                await self.__get_messages(username)
                print(f"Новый пост в канале: {username}")
                print(f"Текст: {event.message.text}")
            print(f"Создан обработчик для канала: {username}")
        else:
            print(f"Ошибка: {username} - это не канал")
        
    def start(self) -> None:
        self.__client.loop.run_until_complete(self.checkAndParseTelegramChannels())
  
if __name__ == "__main__":
   telegramParser = Sleuth("C:/Users/danya/AppData/Roaming/TelegramQuickView/userData.json", "D:/Media")
   telegramParser.start()
            