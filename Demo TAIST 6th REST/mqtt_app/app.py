import paho.mqtt.client as mqtt
from pymongo import MongoClient
from datetime import datetime
from dotenv import load_dotenv
import sqlite3
import time
import json
import os

load_dotenv()

class MQTT_Conn():
    def __init__(self):
        self.client = mqtt.Client(client_id="ton-mqtt-client", callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_publish = self.on_publish
        self.client.on_message = self.on_message
        mqtt_broker = "mosquitto_rssi"
        mqtt_port = 1883
        self.client.username_pw_set("ton-t-sim", "Chayawut16")
        self.client.connect(mqtt_broker, mqtt_port)
        self.sqlite = SQLite()
        self.mongo = Mongo()
        
    def on_connect(self, client, userdata, connect_flags, reason_code, properties):
        print(f"Connected {reason_code}")
        self.client.subscribe("ict720/ton/#")

    def on_disconnect(self, client, userdata, disconnect_flags, reason_code, properties):
        print(f"Disconnected reason {reason_code}")
        self.client.reconnect()

    def on_publish(self, client, userdata, mid, reason_code, properties):
        print(mid, reason_code)

    def on_message(self, client, userdata, message:mqtt.MQTTMessage):
        print(message.topic, ": ", message.payload)
        if message.topic.split('/')[-1] == "beat":
            data = json.loads(message.payload)
            print(f"Device: {data['mac']}")
            return
        if message.topic.split('/')[-1] == "data":
            data = json.loads(message.payload)
            print(f"Device: {data['name']}")
            # insert to SQLite
            station = message.topic.split('/')[2]
            device = data['name']
            rssi = data['rssi']
            self.sqlite.insert_one(station, device, rssi)
            # insert to MongoDB
            data = {
                "timestamp": datetime.now(), 
                "station": station, 
                "device": device, 
                "rssi": rssi
            }
            self.mongo.insert_one("ton", "data", data)
            print("Inserted to MongoDB")

    def publish(self, topic, message:str):
        self.client.publish(topic, message.encode())
        
    def run(self):
        try:
            flag = True
            self.client.loop_start()
            while flag:
                # client.publish("ict720/ton/server/test", json.dumps({"name": "Ton", "age": 23}))
                time.sleep(1)
        except KeyboardInterrupt:
            flag = False

class Mongo():
    def __init__(self):
        self.client = MongoClient(os.getenv("DB_ADDRESS"))
    
    def insert_one(self, database, collection, data):
        database = self.client[database]
        collection = database[collection]
        collection.insert_one(data)

class SQLite():
    def __init__(self):
        self.create_table()

    def get_db_connection(self):
        conn = sqlite3.connect("/mqtt_app/database/ton.db")
        return conn
    
    def create_table(self):
        conn = self.get_db_connection()
        cursor = conn.cursor()
        query = """CREATE TABLE IF NOT EXISTS taist (
            _id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            station TEXT,
            device TEXT,
            rssi INTEGER
        )"""
        cursor.execute(query)
        conn.commit()
        conn.close()
    
    def insert_one(self, station, device, rssi):
        conn = self.get_db_connection()
        cursor = conn.cursor()
        query = "INSERT INTO taist (station, device, rssi) VALUES (?, ?, ?)"
        cursor.execute(query,(station, device, rssi))
        conn.commit()
        conn.close()

if __name__ == '__main__':
    client = MQTT_Conn()
    client.run()