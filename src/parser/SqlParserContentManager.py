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
    dataBasePath = appDataPath + "\\TelegramQuickView\\downloadedPosts.sqlite3"

    connection, cursor = checkDatabaseAvailability(dataBasePath)

    sqlInsertQuery = """
    INSERT INTO downloadedPosts (channelName, postAttachmentPath, postTime, postIndex) VALUES (?, ?, ?, ?)
    """

    sqlPostAvailabilityCheckQuery = "SELECT * FROM downloadedPosts WHERE channelName = ? AND postIndex = ?"
    
    cursor.execute(sqlPostAvailabilityCheckQuery, (channelName, postIndex))
    result = cursor.fetchall()
    
    if len(result) == 0:
        cursor.execute(sqlInsertQuery, (channelName, postAttachmentPath, postTime, postIndex))
        
        connection.commit()
        connection.close()
        return

    if type(result[0]) == tuple:
        result = result[0]
        if len(result) > 2:
            cursor.execute("UPDATE downloadedPosts SET postAttachmentPath = ? WHERE channelName = ? AND postIndex = ?", (result[2] + ", " + postAttachmentPath, channelName, postIndex))
            connection.commit()
            connection.close()

# if __name__ == "__main__":
#     def main():
#         insertPostInfo("erfwfr", "C:\\Users\\danya\\Downloads\\tickets.jpg", "2024",  1)
#         insertPostInfo("erfwfr", "C:\\Users\\danya\\Downloads\\2024-07-29_02-28-57.png", "2024", 1)
#         insertPostInfo("erfwfr", "C:\\Users\\danya\\Downloads\\videoRes.mp4", "2024", 1)
#     main()