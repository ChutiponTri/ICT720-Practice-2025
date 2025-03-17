from flask import Flask, request, jsonify
from pymongo import MongoClient
from datetime import datetime, timedelta
from dotenv import load_dotenv
import json
import os

load_dotenv()
app = Flask(__name__)
mongo_client = MongoClient(os.getenv("DB_ADDRESS"))

@app.route("/api/station/<station_id>", methods=["GET"])
def query_station(station_id):
    database = mongo_client[os.getenv("DB_NAME")]
    collection = database[os.getenv("DB_COLLECTION")]
    resp = {}
    if station_id is None:
        resp["status"] = "error"
        resp["message"] = "station_id is required"
        return jsonify(resp)
    query = {"station": station_id,}
    data = collection.find(query).sort("timestamp", -1).limit(10)
    resp["status"] = "ok"
    resp["station"] = station_id
    resp["data"] = []
    for record in data:
        record_data = {}
        record_data["timestamp"] = record["timestamp"].isoformat()
        record_data["device"] = record["device"]
        record_data["rssi"] = record["rssi"]
        resp["data"].append(record_data)
    return jsonify(resp)

@app.route("/api/asset/<asset_id>", methods=["GET"])
def query_asset(asset_id):
    database = mongo_client[os.getenv("DB_NAME")]
    collection = database[os.getenv("DB_COLLECTION")]
    resp = {}
    if asset_id is None:
        resp["status"] = "error"
        resp["message"] = "asset_id is required"
        return jsonify(resp)
    query = {"device": asset_id}
    data = collection.find(query).sort("timestamp", -1).limit(10)
    resp["status"] = "ok"
    resp["asset"] = asset_id
    resp["data"] = []
    for record in data:
        record_data = {}
        record_data["timestamp"] = record["timestamp"].isoformat()
        record_data["station"] = record["station"]
        record_data["rssi"] = record["rssi"]
        resp["data"].append(record_data)
    return jsonify(resp)

@app.route("/api/station/num/<station_id>", methods=["GET"])
def query_num_dev(station_id):
    database = mongo_client[os.getenv("DB_NAME")]
    collection = database[os.getenv("DB_COLLECTION")]
    resp = {}
    if station_id is None:
        resp["status"] = "error"
        resp["message"] = "station_id is required"
        return jsonify(resp)
    rssi_cond = int(request.args.get('rssi', -60))
    mins_cond = int(request.args.get('mins', 10))
    query = {
        "station": station_id,
        "timestamp": {"$gt": datetime.now() - timedelta(minutes=mins_cond)},
        "rssi": {"$gt": rssi_cond}
    }
    data = collection.find(query)
    resp["status"] = "ok"
    resp["station"] = station_id
    resp["data"] = []
    devices = []
    for record in data:
        if record["device"] in devices:
            continue
        devices.append(record['device'])
        record_data = {}
        record_data["timestamp"] = record["timestamp"].isoformat()
        record_data["device"] = record["device"]
        record_data["rssi"] = record["rssi"]
        resp["data"].append(record_data)
    resp["num"] = len(devices)  # Corrected to count devices outside the loop
    return jsonify(resp)

@app.route("/api/asset/num/<asset_id>", methods=["GET"])
def query_num_stat(asset_id):
    database = mongo_client[os.getenv("DB_NAME")]
    collection = database[os.getenv("DB_COLLECTION")]
    resp = {}
    if asset_id is None:
        resp["status"] = "error"
        resp["message"] = "asset_id is required"
        return jsonify(resp)
    rssi_cond = int(request.args.get('rssi', -60))
    mins_cond = int(request.args.get('mins', 10))
    query = {
        "device": asset_id,
        "timestamp": {"$gt": datetime.now() - timedelta(minutes=mins_cond)},
        "rssi": {"$gt": rssi_cond}
    }
    data = collection.find(query)
    resp["status"] = "ok"
    resp["station"] = asset_id
    resp["data"] = []
    stations = []
    for record in data:
        if record["station"] in stations:
            continue
        stations.append(record["station"])
        record_data = {}
        record_data["timestamp"] = record["timestamp"].isoformat()
        record_data["station"] = record["station"]
        record_data["rssi"] = record["rssi"]
        resp["data"].append(record_data)
    resp["num"] = len(stations)  # Corrected to count stations outside the loop
    return jsonify(resp)

@app.route("/api/station/10mins", methods=["GET"])
def query_mins():
    database = mongo_client[os.getenv("DB_NAME")]
    collection = database[os.getenv("DB_COLLECTION")]
    resp = {}
    time_threshold = datetime.now() - timedelta(minutes=10)
    data = collection.find({"timestamp": {"$gte": time_threshold}})
    resp["status"] = "ok"
    resp["data"] = []
    for record in data:
        record_data = {}
        record_data["timestamp"] = record["timestamp"].isoformat()
        record_data["device"] = record["device"]
        record_data["rssi"] = record["rssi"]
        resp["data"].append(record_data)
    return jsonify(resp)

@app.route("/api/station/rssi", methods=["GET"])
def query_rssi():
    database = mongo_client[os.getenv("DB_NAME")]
    collection = database[os.getenv("DB_COLLECTION")]
    resp = {}
    data = collection.find({"rssi": {"$gt": -60}})
    resp["status"] = "ok"
    resp["data"] = []
    for record in data:
        record_data = {}
        record_data["timestamp"] = record["timestamp"].isoformat()
        record_data["device"] = record["device"]
        record_data["rssi"] = record["rssi"]
        resp["data"].append(record_data)
    return jsonify(resp)

@app.route("/", methods=["GET"])
def root():
    resp = {"Hello": "World"}
    return jsonify(resp)

if __name__ == "__main__":
    app.run(debug=True)
