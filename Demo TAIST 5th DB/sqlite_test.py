import sqlite3 as sqlite

def get_db_connection():
    conn = sqlite.connect("sql_database/ton.db")
    return conn

def create_table():
    conn = get_db_connection()
    cursor = conn.cursor()
    query = """CREATE TABLE IF NOT EXISTS customers (
        number INTEGER PRIMARY KEY AUTOINCREMENT,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
        device VARCHAR(25) NOT NULL,
        data REAL NOT NULL    
    )"""
    cursor.execute(query)
    conn.commit()
    conn.close()
    return {"message": "Create Successfully"}

def insert_one(device, data):
    conn = get_db_connection()
    cursor = conn.cursor()
    query = "INSERT INTO customers (device, data) VALUES (?, ?)"
    cursor.execute(query, (device, data))
    conn.commit()
    conn.close()
    return {"message": "Create Successfully"}

if __name__ == '__main__':
    create_table()
    insert_one("t-sim", 3.14)