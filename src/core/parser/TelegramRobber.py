from tkinter.messagebox import askyesno
from dateutil.tz import tzlocal
from telethon import TelegramClient, events
from typing import Callable
from telethon.tl.types import InputPeerChannel, Message
import asyncio
from pathlib import Path
from shutil import rmtree
import json
from os import listdir, rename
from datetime import datetime


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
                if event.message.grouped_id != None:
                    if event.message.grouped_id != self.__groupedMessageId:
                        self.__groupedMessageId = event.message.grouped_id
                        self.__groupedMessageDate = event.message.date
                        await self.__downloadFunction(self.__username, event.message)
                    elif event.message.grouped_id == self.__groupedMessageId:
                        if event.message.date.minute == self.__groupedMessageDate.minute or (event.message.date.minute - self.__groupedMessageDate.minute == 1 and event.message.date.second <= 20):
                            await self.__downloadFunction(self.__username, event.message)
                elif event.message.grouped_id == None:
                    await self.__downloadFunction(self.__username, event.message)

class Sleuth:
    def __init__(
            self,
            pathToSettingsJsonFile: str,
            pathToAppRootDirectory: str
    ):
        self.pathToSettingsJsonFile = pathToSettingsJsonFile
        userSettings = self.__getUserSettings()
        
        self.api_id = userSettings.get("apiId")
        self.api_hash = userSettings.get("apiHash")
        self.phone_number = userSettings.get("phoneNumber") 
        self.lastPostsCount = userSettings.get("lastPostsCount")
        self.code = userSettings.get("code")
        self.codeHash = userSettings.get("codeHash")
        self.channels = userSettings.get("channels")
        self.lastMessageGroupId = 0
        self.__pathToAppRootDirectory = pathToAppRootDirectory
        self.__pathToAppRootDirectoryContent = ""
        
        self.__client = TelegramClient('TelegramQuickView', self.api_id, self.api_hash, timeout=10)
        
    def __getUserSettings(self) -> dict[str, str]:
        with open(self.pathToSettingsJsonFile, "r", encoding="utf-8") as jsonFile:
            data = json.load(jsonFile)
        return data

    async def __get_singleMessage(self, username: str, message: Message):
        postIndex = await self.getPostIndex(username)
        if message.grouped_id != None: 
            if self.lastMessageGroupId != message.grouped_id:
                await self.__check_download_path(username, postIndex)
                await self.__organizeDirectory(username)
                self.lastMessageGroupId = message.grouped_id
        elif message.grouped_id == None:
            await self.__check_download_path(username, postIndex)
            await self.__organizeDirectory(username)
        if len(message.text) > 1:
            await self.__export_to_txt(message.text, f"{self.__download_paths[5]}/text.txt")
        if message.media != None:
            file_type = message.file.mime_type.split('/')[0]
            download_path = await self.__get_download_path(file_type)
            await message.download_media(file=download_path)

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
    
    async def __organizeDirectory(self, username: str) -> None:
        dirObjects = listdir(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0])    

        if (int(dirObjects[-1]) > self.lastPostsCount):  
            rmtree(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + dirObjects[0])
   
            dirObjects = listdir(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0])
            for index, obj in enumerate(dirObjects, start=1):
                if obj != str(index):
                    rename(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + obj, self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + f"{index}")
                    
        postIndex = await self.__getPostIndex(username)
        self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectory + f"/{username}/" + str(postIndex - 1)
        self.__download_paths = [
            f"{self.__pathToAppRootDirectoryContent}/Изображения",
            f"{self.__pathToAppRootDirectoryContent}/Видео",
            f"{self.__pathToAppRootDirectoryContent}/Аудио",
            f"{self.__pathToAppRootDirectoryContent}/Документы",
            f"{self.__pathToAppRootDirectoryContent}/Остальное",
            f"{self.__pathToAppRootDirectoryContent}/Текст"
        ]
                
    async def __getPostIndex(self, username: str) -> int:
        if Path(self.__pathToAppRootDirectory + f"/{username}/").exists():
            dirObjects = listdir(self.__pathToAppRootDirectory + f"/{username}/")
            if len(dirObjects) > 0:
                return int(dirObjects[-1]) + 1
        return 1

    async def __checkAndParseTelegramChannels(self) -> None:
        if self.__client.disconnected:
            await self.__client.connect()
        if not await self.__client.is_user_authorized():
            await self.__client.sign_in(self.phone_number, self.code, phone_code_hash=self.codeHash)

        for channel in self.channels:
            handlersManager = HandlersManager(self.__client, self.__get_singleMessage, channel)
            await handlersManager.createChannelHandler()

        await self.__client.run_until_disconnected()
     
    async def __fetchRecentChannelsUpdates(self):
        if self.__client.disconnected:
            await self.__client.connect()
        if not await self.__client.is_user_authorized():
            await self.__client.sign_in(self.phone_number, self.code, phone_code_hash=self.codeHash)
        for channel in self.channels:
            async for message in self.__client.iter_messages(
                channel,
                limit=self.lastPostsCount
            ):
                if not message:
                    break
                    
                if message.file is not None:
                    file_type = message.file.mime_type.split('/')[0]
                    postIndex = await self.__getPostIndex(channel)
                    await self.__check_download_path(channel, postIndex)
                    await self.__organizeDirectory(channel)
                    download_path = await self.__get_download_path(file_type)
                    await message.download_media(file=download_path)
                    await self.__export_to_txt(message.text, f"{self.__download_paths[5]}/text.txt")
        await self.__client.run_until_disconnected()
         
def start(telegramParser: Sleuth) -> None:
    telegramParser.__client.loop.run_until_complete(telegramParser.__fetchRecentChannelsUpdates())
        
    # self.__client.loop.run_until_complete(self.__fetchRecentChannelsUpdates())

if __name__ == "__main__":
    telegramParser = Sleuth("C:/Users/danya/AppData/Roaming/TelegramQuickView/userData.json", "D:/Media")
    telegramParser.start()