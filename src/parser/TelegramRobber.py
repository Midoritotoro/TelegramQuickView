from SqlParserContentManager import insertPostInfo
from dateutil.tz import tzlocal
from telethon import TelegramClient, events
from typing import Callable, Any
from telethon.tl.types import InputPeerChannel, Message
import asyncio
from pathlib import Path
from shutil import rmtree
import json
from os import listdir, rename
import os.path
from datetime import datetime


class HandlersManager:
    def __init__(self: 'HandlersManager',
                 client: TelegramClient,
                 downloadFunction: Callable,
                 username: str
    ) -> None:
        self.__client = client
        self.__downloadFunction = downloadFunction
        self.__username = username
    
    async def createChannelHandler(self: 'HandlersManager') -> None:
        channelEntity = await self.__client.get_input_entity(self.__username)
        
        if isinstance(channelEntity, InputPeerChannel):
            @self.__client.on(events.NewMessage(chats=channelEntity))
            async def handler(event: events.NewMessage.Event) -> None:
                await self.__downloadFunction(self.__username, event.message)

class Sleuth:
    def __init__(
            self: 'Sleuth',
            pathToSettingsJsonFile: str,
            pathToAppRootDirectory: str
    ) -> None:
        self.pathToSettingsJsonFile = pathToSettingsJsonFile
        
        userSettings = self.__getUserSettings()
        self.__api_id = userSettings.get("apiId")
        self.__api_hash = userSettings.get("apiHash")
        self.__phone_number = userSettings.get("phoneNumber") 
        self.__lastPostsCount = userSettings.get("lastPostsCount")
        self.__code = userSettings.get("code")
        self.__codeHash = userSettings.get("codeHash")
        self.__channels = userSettings.get("channels")
        self.__lastMessageGroupId = 0
        self.__groupedMessageId = 0
        self.__currentLastPostCount = self.__lastPostsCount
        self.__groupedMessageDate: datetime 
        self.__pathToAppRootDirectory = pathToAppRootDirectory
        self.__pathToAppRootDirectoryContent = ""
        
        sessionFile = os.path.join(os.path.dirname(os.path.abspath(__file__)).rsplit("\\", 2)[0], "out/build/TelegramQuickView.session") # ----------------------- ?
        # print(sessionFile) 
        # print(os.path.dirname(os.path.abspath(__file__)))
        self.__client = TelegramClient(sessionFile, self.__api_id, self.__api_hash, timeout=10) # ----------------------- ?
        
    def __getUserSettings(self: 'Sleuth') -> dict[str, Any]:
        with open(self.pathToSettingsJsonFile, "r", encoding="utf-8") as jsonFile:
            data = json.load(jsonFile)
        return data

    async def __downloadSingleMessage(self: 'Sleuth', username: str, message: Message) -> None:
        postIndex = await self.__getPostIndex(username)
        print(message.text)
        if message.grouped_id == None: 
            await self.__checkDownloadPath(username, postIndex)
            await self.__organizeDirectory(username)
        else:
            self.__currentLastPostCount += 1
            if self.__lastMessageGroupId != message.grouped_id:
                await self.__checkDownloadPath(username, postIndex)
                await self.__organizeDirectory(username)
                self.__lastMessageGroupId = message.grouped_id
           
        postIndex = await self.__getPostIndex(username)
        
        if len(message.text) > 1:
            await self.__exportToTxt(message.text, f"{self.__downloadPaths[1]}/text.txt")
            insertPostInfo(username, f"{self.__downloadPaths[1]}/text.txt", message.date, postIndex - 1)
            
        if message.file != None:
            fileType = message.file.mime_type.split('/')[0]
            downloadPath = self.__downloadPaths[0]
            insertPostInfo(username, downloadPath, message.date, postIndex - 1)
            await self.__client.download_media(message=message, file=downloadPath)

    async def __exportToTxt(self: 'Sleuth', message: str, outputPath: str) -> None:
        with open(outputPath, "w", encoding="utf-8") as file:
            file.write(message)
    
    async def __updateDownloadPaths(self: 'Sleuth'):
        self.__downloadPaths = [
            f"{self.__pathToAppRootDirectoryContent}/Attachments",
            f"{self.__pathToAppRootDirectoryContent}/Text"
        ]

    async def __checkDownloadPath(self: 'Sleuth', username: str, postIndex: int) -> None:
        self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectory + f"/{username}/" + str(postIndex)
        await self.__updateDownloadPaths()

        if not Path(self.__pathToAppRootDirectoryContent).exists():
            Path(self.__pathToAppRootDirectoryContent).mkdir(parents=True)
        for download_path in self.__downloadPaths:
            Path(download_path).mkdir(parents=True, exist_ok=True)
    
    async def __organizeDirectory(self: 'Sleuth', username: str) -> None:
        dirObjects = listdir(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0])    

        if (int(dirObjects[-1]) > self.__lastPostsCount):  
            rmtree(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + dirObjects[0])
   
            dirObjects = listdir(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0])
            for index, obj in enumerate(dirObjects, start=1):
                if obj != str(index):
                    rename(self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + obj, self.__pathToAppRootDirectoryContent.rsplit('/', 1)[0] + "/" + f"{index}")
                    
        postIndex = await self.__getPostIndex(username)
        
        self.__pathToAppRootDirectoryContent = self.__pathToAppRootDirectory + f"/{username}/" + str(postIndex - 1)
        await self.__updateDownloadPaths()
                
    async def __getPostIndex(self: 'Sleuth', username: str) -> int:
        if Path(self.__pathToAppRootDirectory + f"/{username}/").exists():
            dirObjects = listdir(self.__pathToAppRootDirectory + f"/{username}/")
            if len(dirObjects) > 0:
                return int(dirObjects[-1]) + 1
        return 1

    async def __checkAndParseTelegramChannels(self: 'Sleuth') -> None:
        if self.__client.disconnected:
            await self.__client.connect()
            
        if not await self.__client.is_user_authorized():
            await self.__client.sign_in(self.__phone_number, self.__code, phone_code_hash=self.__codeHash)

        for channel in self.__channels:
            handlersManager = HandlersManager(self.__client, self.__downloadSingleMessage, channel)
            await handlersManager.createChannelHandler()

        await self.__client.run_until_disconnected()
                    
    async def __fetchRecentChannelsUpdates(self: 'Sleuth'):
        if self.__client.disconnected:
            await self.__client.connect()
            
        if not await self.__client.is_user_authorized():
            await self.__client.sign_in(self.__phone_number, self.__code, phone_code_hash=self.__codeHash)
            
        for channel in self.__channels:
            index = 0
            async for message in self.__client.iter_messages(
                channel
            ):
                if not message:
                    break
                
                index += 1
                if index > self.__currentLastPostCount:
                    break       
                
                await self.__downloadSingleMessage(channel, message)
                self.__currentLastPostCount = self.__lastPostsCount
         
    def start(self: 'Sleuth') -> None:
        # self.__client.loop.run_until_complete(self.__fetchRecentChannelsUpdates())
        self.__client.loop.run_until_complete(self.__checkAndParseTelegramChannels())