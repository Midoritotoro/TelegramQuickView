from os import replace
import re
from datetime import datetime, timezone
from dateutil.tz import tzlocal
from telethon.errors import ApiIdInvalidError, UsernameInvalidError
from telethon.sync import TelegramClient
import re
from pathlib import Path
from shutil import rmtree

class Sleuth:
    def __init__(
            self,
            api_id: int,
            api_hash: str,
            phone_number: int,
            pathToSettingsJsonFile: str,
            pathToAppRootDirectory: str
    ):
        self.api_id = api_id
        self.api_hash = api_hash
        self.pathToSettingsJsonFile = pathToSettingsJsonFile
        self.phone_number = phone_number 
        
        self.__pathToAppRootDirectory = pathToAppRootDirectory

        self.__download_paths = [
            f"{self.__pathToAppRootDirectory}/Изображения",
            f"{self.__pathToAppRootDirectory}/Видео",
            f"{self.__pathToAppRootDirectory}/Аудио",
            f"{self.__pathToAppRootDirectory}/Документы",
            f"{self.__pathToAppRootDirectory}/Остальное",
            f"{self.__pathToAppRootDirectory}/Текст"
        ]
        # self.__client = TelegramClient('TelegramQuickView', self.api_id, self.api_hash, timeout=10)


    def get_messages(self):

            print("parsed")

    def dig(self) -> str:
        try:
            # self.__validate_date()
            # self.__check_download_path()
             #self.__client.connect()
           # phone_code = await self.__client.send_code_request(self.phone_number) // without console
           # phone_code_hash = phone_code.phone_code_hash
           # await self.__client.sign_in(self.phone, code=code, phone_code_hash=phone_code_hash)
            # self.__client.start(self.phone_number)
             self.get_messages()
             #self.__client.disconnect()
        except:
            pass

    def export_to_txt(self, message: str, output_path: str) -> None:
        with open(output_path, "w", newline="", encoding="utf-8") as file:
            file.write(message)

    def __validate_date(self) -> None:
        date_pattern = r'^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$'
        if not re.match(date_pattern, self.start_date) and not re.match(date_pattern, self.end_date):
            return
        
        self.start_date = (datetime.strptime(self.start_date, '%Y-%m-%d').replace(hour=0, minute=0, second=0, microsecond=0)).astimezone(timezone.utc)
        self.end_date = (datetime.strptime(self.end_date, '%Y-%m-%d').replace(hour=23, minute=59, second=59, microsecond=999999)).astimezone(timezone.utc)

    def __get_download_path(self, file_type) -> str:
        return {
            'image': self.__download_paths[0],
            'video': self.__download_paths[1],
            'audio': self.__download_paths[2],
            'documents': self.__download_paths[3],
        }.get(file_type, self.__download_paths[4])

    def __check_download_path(self):
        if Path(self.__base_path).exists():
            rmtree(self.__base_path)
        if not Path(self.__base_path).exists():
            Path(self.__base_path).mkdir(parents=True)
        for download_path in self.__download_paths:
            Path(download_path).mkdir(parents=True, exist_ok=True)
            