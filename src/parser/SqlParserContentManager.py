import sqlite3 
import os


def checkDatabaseAvailability(dataBasePath: str) -> tuple[sqlite3.Connection, sqlite3.Cursor]:
    connection = sqlite3.connect(dataBasePath)
    cursor = connection.cursor()
    sqlQuery = """
    CREATE TABLE IF NOT EXISTS downloadedPosts(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        channelId INTEGER,
        postPath TEXT,
        postTime TEXT
        );
    """
    
    cursor.execute(sqlQuery)
    connection.commit()

    return connection, cursor
    
def insertPostInfo(channelId: int, postPath: str, postTime: str) -> None:
    appDataPath = os.getenv('APPDATA')
    dataBasePath = appDataPath + "\\TelegramQuickView\\downloadedPosts.db"

    connection, cursor = checkDatabaseAvailability(dataBasePath)

    sqlInsertQuery = """
    INSERT INTO downloadedPosts (channelId, postPath, postTime) VALUES (?, ?, ?)
    """
    
    cursor.execute(sqlInsertQuery, (channelId, postPath, postTime))
    connection.commit()
    
    connection.close()
