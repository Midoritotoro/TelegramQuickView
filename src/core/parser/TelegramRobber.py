from os import replace
import re
from datetime import datetime, timezone
from dateutil.tz import tzlocal
from telethon.errors import ApiIdInvalidError, UsernameInvalidError
from telethon import TelegramClient
import re
import asyncio
from pathlib import Path
from shutil import rmtree

class Sleuth:
    def __init__(
            self,
            api_id: int,
            api_hash: str,
            phone_number: int,
            username: str,
            start_date: str=None,
            end_date: str=None,
            download_path:str=None,
    ):
        self.api_id = api_id
        self.api_hash = api_hash
        self.group_username = username
        self.start_date = start_date
        self.end_date = end_date
        self.phone_number = phone_number 
        
        self.__download_path = download_path

        self.__base_path = f"{self.__download_path}/{username}"
        self.__download_paths = [
            f"{self.__base_path}/Изображения",
            f"{self.__base_path}/Видео",
            f"{self.__base_path}/Аудио",
            f"{self.__base_path}/Документы",
            f"{self.__base_path}/Остальное",
            f"{self.__base_path}/Текст"
        ]
        self.__client = TelegramClient('TelegramQuickView', self.api_id, self.api_hash, timeout=10)


    async def get_messages(self):
        tasks = []
        export = []
        async for message in self.__client.iter_messages(
                self.group_username,
                offset_date=self.start_date,
                reverse=True):
            
            if not message:
                break

            if self.start_date and self.end_date:
                if message.date.astimezone(timezone.utc) >= self.end_date:
                    break

            if message.sender is not None and message.text is not None:
                clean_message = message.text

                if message.file is not None:
                    file_type = message.file.mime_type.split('/')[0]
                    download_path = await self.__get_download_path(file_type)
                    tasks.append(asyncio.create_task(message.download_media(file=download_path)))
                    export.append(asyncio.create_task(self.export_to_txt(clean_message, f"{self.__download_paths[5]}/{f"{message.date.astimezone(tzlocal())}".replace(" ", "_").rsplit("+", 1)[0].replace(":", "_")}.txt")))

                if len(tasks) == 50:
                    await asyncio.gather(*tasks, *export)
                    tasks.clear()
                    export.clear()

        await asyncio.gather(*tasks)
        await self.export_to_txt(f"{self.__download_paths[5]}/{f"{message.date.astimezone(tzlocal())}".replace(" ", "_").rsplit("+", 1)[0].replace(":", "_")}.txt")

    async def dig(self) -> str:
        try:
            await self.__validate_date()
            await self.__check_download_path()
            await self.__client.connect()
           # phone_code = await self.__client.send_code_request(self.phone_number) // without console
           # phone_code_hash = phone_code.phone_code_hash
           # await self.__client.sign_in(self.phone, code=code, phone_code_hash=phone_code_hash)
            await self.__client.start(self.phone_number)

            await self.get_messages()
            await self.__client.disconnect()
        except:
            pass

    async def export_to_txt(self, message: str, output_path: str) -> None:
        with open(output_path, "w", newline="", encoding="utf-8") as file:
            file.write(message)

    async def __validate_date(self) -> None:
        date_pattern = r'^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$'
        if not re.match(date_pattern, self.start_date) and not re.match(date_pattern, self.end_date):
            return
        
        self.start_date = (datetime.strptime(self.start_date, '%Y-%m-%d').replace(hour=0, minute=0, second=0, microsecond=0)).astimezone(timezone.utc)
        self.end_date = (datetime.strptime(self.end_date, '%Y-%m-%d').replace(hour=23, minute=59, second=59, microsecond=999999)).astimezone(timezone.utc)

    async def __get_download_path(self, file_type) -> str:
        return {
            'image': self.__download_paths[0],
            'video': self.__download_paths[1],
            'audio': self.__download_paths[2],
            'documents': self.__download_paths[3],
        }.get(file_type, self.__download_paths[4])

    async def __check_download_path(self):
        if Path(self.__base_path).exists():
            rmtree(self.__base_path)
        if not Path(self.__base_path).exists():
            Path(self.__base_path).mkdir(parents=True)
        for download_path in self.__download_paths:
            Path(download_path).mkdir(parents=True, exist_ok=True)
            
    def start(self) -> None:
        asyncio.run(self.dig())