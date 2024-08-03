import sqlite3 
import os


def checkDatabaseAvailability(dataBasePath: str) -> tuple[sqlite3.Connection, sqlite3.Cursor]:
    connection = sqlite3.connect(dataBasePath)
    cursor = connection.cursor()
    sqlQuery = """
    CREATE TABLE IF NOT EXISTS downloadedPosts(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        channelName TEXT,
        postAttachmentPath TEXT,
        postTime TEXT,
        postIndex INTEGER
        );
    """
    
    cursor.execute(sqlQuery)
    connection.commit()

    return connection, cursor
    
def insertPostInfo(channelName: str, postAttachmentPath: str, postTime: str, postIndex: int) -> None:
    appDataPath = os.getenv('APPDATA')
    dataBasePath = appDataPath + "\\TelegramQuickView\\downloadedPosts.db"

    connection, cursor = checkDatabaseAvailability(dataBasePath)

    sqlInsertQuery = """
    INSERT INTO downloadedPosts (channelName, postAttachmentPath, postTime, postIndex) VALUES (?, ?, ?, ?)
    """
    
    cursor.execute(sqlInsertQuery, (channelName, postAttachmentPath, postTime, postIndex))
    connection.commit()
    
    connection.close()
