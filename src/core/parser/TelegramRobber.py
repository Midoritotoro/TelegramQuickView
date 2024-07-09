from datetime import datetime, timezone
from dateutil.tz import tzlocal
from telethon import TelegramClient, events
from typing import Callable
from telethon.tl.types import InputPeerChannel, PeerChannel, Message, TypeMessageReplyHeader
from telethon.errors import PeerFloodError
import asyncio
from pathlib import Path
from shutil import rmtree
import json
from os import listdir, rename
from datetime import datetime

channels = ["test329483", "testssss352"]

class HandlersManager:
    def __init__(self, client: TelegramClient, downloadFunction: Callable, username: str) -> None:
        self.__client = client
        self.__downloadFunction = downloadFunction
        self.__username = username
        self.__groupedMessageId = 0
        self.__groupedMessageDate: datetime 
        
    
    async def createChannelHandler(self):
        channel_entity = await self.__client.get_input_entity(self.__username)
        if isinstance(channel_entity, InputPeerChannel):
            @self.__client.on(events.NewMessage(chats=channel_entity))
            async def handler(event: events.NewMessage.Event):
                event.message: Message = event.message
                if event.message.grouped_id != None:
                    if event.message.grouped_id != self.__groupedMessageId:
                        print("event.message.grouped_id != self.__groupedMessageId")
                        self.__groupedMessageId = event.message.grouped_id
                        self.__groupedMessageDate = event.message.date
                        await self.__downloadFunction(self.__username, 1, False, event.message)
                    elif event.message.grouped_id == self.__groupedMessageId:
                        print("event.message.grouped_id == self.__groupedMessageId")
                        if event.message.date.minute == self.__groupedMessageDate.minute or (event.message.date.minute - self.__groupedMessageDate.minute == 1 and event.message.date.second <= 20):
                            print("event.message.date.minute == self.__groupedMessageDate.minute or (event.message.date.minute - self.__groupedMessageDate.minute == 1 and event.message.date.second <= 20)")
                            await self.__downloadFunction(self.__username, 1, False, event.message)
                elif event.message.grouped_id == None:
                    print("event.message.grouped_id == None")
                    await self.__downloadFunction(self.__username, 1, True)

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
        
    def __getUserSettings(self) -> dict[str, str]:
        with open(self.pathToSettingsJsonFile, "r", encoding="utf-8") as jsonFile:
            data = json.load(jsonFile)
        return data

    async def __get_messages(self, username: str, limit: int, checkDownloadPath: bool = True, message: Message = None):
        if message != None:
            postIndex = await self.getPostIndex(username)
            if postIndex > 1:
                await self.__check_download_path(username, postIndex - 1)
            else:
                await self.__check_download_path(username, postIndex)
            if len(message.text) > 1:
                print(message.text)
                await self.__export_to_txt(message.text, f"{self.__download_paths[5]}/text.txt")
                return
            # await self.organizeDirectory(username)
            file_type = message.file.mime_type.split('/')[0]
            download_path = await self.__get_download_path(file_type)
            await message.download_media(file=download_path)
            
        tasks = []
        export = []
        async for message in self.__client.iter_messages(
                username,
                limit = limit
        ):
            print(message.text)
            if not message:
                break
            
            if message.sender is not None and message.text is not None:
                
                if checkDownloadPath:
                    postIndex = await self.getPostIndex(username)
                    await self.__check_download_path(username, postIndex)
                await self.organizeDirectory(username)
                clean_message = message.text
                await self.__export_to_txt(clean_message, f"{self.__download_paths[5]}/text.txt")
                # export.append(asyncio.create_task(self.__export_to_txt(clean_message, f"{self.__download_paths[5]}/text.txt")))
                
                if message.file is not None:
                    file_type = message.file.mime_type.split('/')[0]
                    download_path = await self.__get_download_path(file_type)
                    await message.download_media(file=download_path)
                    # tasks.append(asyncio.create_task(message.download_media(file=download_path)))
            
            # if len(tasks) == limit:
               #  await asyncio.gather(*tasks, *export)
                #tasks.clear()
                # export.clear()

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

    async def __check_download_path(self, username: str, postIndex: int) -> None:
        self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectory + f"/{username}/" + str(postIndex)
        self.__download_paths = [
            f"{self.__pathToAppRootDirectoryContent}/Изображения",
            f"{self.__pathToAppRootDirectoryContent}/Видео",
            f"{self.__pathToAppRootDirectoryContent}/Аудио",
            f"{self.__pathToAppRootDirectoryContent}/Документы",
            f"{self.__pathToAppRootDirectoryContent}/Остальное",
            f"{self.__pathToAppRootDirectoryContent}/Текст"
        ]
        
        if not Path(self.__pathToAppRootDirectoryContent).exists():
            Path(self.__pathToAppRootDirectoryContent).mkdir(parents=True)
        for download_path in self.__download_paths:
            Path(download_path).mkdir(parents=True, exist_ok=True)
    
    async def organizeDirectory(self, username: str) -> None:
        dirObjects = listdir(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0])    

        if (int(dirObjects[-1]) > self.lastPostsCount):  
            rmtree(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + dirObjects[0])
   
            dirObjects = listdir(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0])
            for index, obj in enumerate(dirObjects, start=1):
                if obj != str(index):
                    rename(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + obj, self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + f"{index}")
                    
        postIndex = await self.getPostIndex(username)
        self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectory + f"/{username}/" + str(postIndex - 1)
        self.__download_paths = [
            f"{self.__pathToAppRootDirectoryContent}/Изображения",
            f"{self.__pathToAppRootDirectoryContent}/Видео",
            f"{self.__pathToAppRootDirectoryContent}/Аудио",
            f"{self.__pathToAppRootDirectoryContent}/Документы",
            f"{self.__pathToAppRootDirectoryContent}/Остальное",
            f"{self.__pathToAppRootDirectoryContent}/Текст"
        ]
                
    async def getPostIndex(self, username: str) -> int:
        if Path(self.__pathToAppRootDirectory + f"/{username}/").exists():
            dirObjects = listdir(self.__pathToAppRootDirectory + f"/{username}/")
            if len(dirObjects) > 0:
                return int(dirObjects[-1]) + 1
        return 1

    async def checkAndParseTelegramChannels(self) -> None:
        await self.__client.connect()
        if not await self.__client.is_user_authorized():
            await self.__client.sign_in(self.phone_number, self.code, phone_code_hash=self.codeHash)

        for channel in channels:
            handlersManager = HandlersManager(self.__client, self.__get_messages, channel)
            await handlersManager.createChannelHandler()

        await self.__client.run_until_disconnected()
        
    def start(self) -> None:
        self.__client.loop.run_until_complete(self.checkAndParseTelegramChannels())
  
if __name__ == "__main__":
    telegramParser = Sleuth("C:/Users/danya/AppData/Roaming/TelegramQuickView/userData.json", "D:/Media")
    telegramParser.start()
     