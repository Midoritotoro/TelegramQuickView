from datetime import datetime, timezone
from dateutil.tz import tzlocal
from telethon import TelegramClient, events
from telethon.tl.types import InputPeerChannel, PeerChannel
from telethon.errors import PeerFloodError
import asyncio
from pathlib import Path
from shutil import rmtree
import json
from os import listdir, rename

channels = ["test329483", "testssss352"]


class Sleuth:
    def __init__(
            self,
            pathToSettingsJsonFile: str,
            pathToAppRootDirectory: str
    ):
        self.pathToSettingsJsonFile = pathToSettingsJsonFile
        telegramCredentials = self.__getUserSettings()
        
        self.api_id = telegramCredentials.get("apiId")
        self.api_hash = telegramCredentials.get("apiHash")
        self.phone_number = telegramCredentials.get("phoneNumber") 
        self.lastPostsCount = telegramCredentials.get("lastPostsCount")
        self.code = telegramCredentials.get("code")
        self.codeHash = telegramCredentials.get("codeHash")
        
        self.__pathToAppRootDirectory = pathToAppRootDirectory
        self.__pathToAppRootDirectoryContent = ""
        
        self.__client = TelegramClient('TelegramQuickView', self.api_id, self.api_hash, timeout=10)
        
    async def __updatePaths(self, index: int, set: bool, username: str = None) -> None:  
        if set:
            self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectory + f"/{username}/" + f"{index}"
        else:
            self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectoryContent[:-1] + f"{index}"

        self.__download_paths = [
            f"{self.__pathToAppRootDirectoryContent}/Изображения",
            f"{self.__pathToAppRootDirectoryContent}/Видео",
            f"{self.__pathToAppRootDirectoryContent}/Аудио",
            f"{self.__pathToAppRootDirectoryContent}/Документы",
            f"{self.__pathToAppRootDirectoryContent}/Остальное",
            f"{self.__pathToAppRootDirectoryContent}/Текст"
        ]
        
    def __getUserSettings(self) -> dict[str, str]:
        with open(self.pathToSettingsJsonFile, "r", encoding="utf-8") as jsonFile:
            data = json.load(jsonFile)
        return data

    async def __get_messages(self, username: str, limit: int):
        postIndex = await self.getPostIndex(username)
        if Path(self.__pathToAppRootDirectory + f"/{username}/").exists():
            await self.__updatePaths(postIndex, False)
        else:
            await self.__updatePaths(postIndex, True, username)
        tasks = []
        export = []
        async for message in self.__client.iter_messages(
                username,
                limit = limit
        ):
            
            if not message:
                break
            
            await self.__updatePaths(postIndex, False)
            
            if message.sender is not None and message.text is not None:
                await self.__check_download_path()
                clean_message = message.text
                export.append(asyncio.create_task(self.__export_to_txt(clean_message, f"{self.__download_paths[5]}/{postIndex}.txt")))
                
                if message.file is not None:
                    file_type = message.file.mime_type.split('/')[0]
                    download_path = await self.__get_download_path(file_type)
                    tasks.append(asyncio.create_task(message.download_media(file=download_path)))

            if len(tasks) == limit:
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
        while True:
            dirObjects = listdir(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0])
            
            if not (int(dirObjects[-1]) > self.lastPostsCount):
                break
            
            rmtree(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + dirObjects[0])
                
        dirObjects = listdir(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0])
        for index, obj in enumerate(dirObjects, start=1):
            if obj != str(index):
                rename(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + obj)
                
    async def getPostIndex(self, username: str) -> int:
        dirObjects = listdir(self.__pathToAppRootDirectory + f"/{username}/")
        return int(dirObjects[-1])
        

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
                await self.__check_download_path()
                await self.__get_messages(username, 1)
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
            