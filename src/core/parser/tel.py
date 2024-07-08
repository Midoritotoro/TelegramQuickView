from telethon import TelegramClient, events
from telethon.tl.types import InputPeerChannel, PeerChannel
from telethon.errors import PeerFloodError

api_id = 13711370  # Замените на свой API ID
api_hash = '019edf3f20c8460b741fb94114e6fec0'  # Замените на свой API Hash
phone = '+375292384917'  # Замените на свой номер телефона

client = TelegramClient('TelegramQuickView', api_id, api_hash)

# Словарь для хранения каналов и их ID
channels = {}

async def main():
    await client.connect()
    if not await client.is_user_authorized():
        await client.send_code_request(phone)
        await client.sign_in(phone, input('Введите код: '))

    # Добавляем обработчик для новых сообщений
    @client.on(events.NewMessage(chats='antifishechki'))
    async def handler(event):
        # Получаем имя канала из словаря
        print(f"Новый пост в канале: 'antifishechki'")
        print(f"Текст: {event.message.text}")

    # Функция для добавления канала в список
    async def add_channel(channel_name):
        try:
            channel = await client.get_input_entity(channel_name)
            if isinstance(channel, InputPeerChannel):
                channels[channel_name] = channel.channel_id
                print(f"Канал {channel_name} добавлен!")
            else:
                print("Ошибка: Это не канал.")
        except PeerFloodError:
            print("Ошибка: Слишком много запросов. Попробуйте позже.")
        except Exception as e:
            print(f"Ошибка: {e}")

    # Функция для удаления канала из списка
    async def remove_channel(channel_name):
        if channel_name in channels:
            del channels[channel_name]
            print(f"Канал {channel_name} удален.")
        else:
            print(f"Канал {channel_name} не найден.")

    # Пример использования функций добавления и удаления каналов


    # Запускаем слушателя событий
    await client.run_until_disconnected()

if __name__ == '__main__':
    client.loop.run_until_complete(main())