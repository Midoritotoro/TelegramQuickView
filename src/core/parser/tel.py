from telethon import TelegramClient, events
from telethon.tl.types import InputPeerChannel, PeerChannel
from telethon.errors import PeerFloodError
import asyncio


api_id = 13711370
api_hash = '019edf3f20c8460b741fb94114e6fec0'
phone = '+375292384917'

channels = ["test329483", "testssss352"]
client = TelegramClient('TelegramQuickView', api_id, api_hash)

async def createChannelHandler(channel_name):
    channel_entity = await client.get_input_entity(channel_name)
    if isinstance(channel_entity, InputPeerChannel):
        @client.on(events.NewMessage(chats=channel_entity))
        async def handler(event):
            print(f"Новый пост в канале: {channel_name}")
            print(f"Текст: {event.message.text}")
        print(f"Создан обработчик для канала: {channel_name}")
    else:
        print(f"Ошибка: {channel_name} - это не канал")
