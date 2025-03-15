import requests

# station_id = "esp32"          # Station ID
# url = f"http://192.168.137.1:5000/api/station/{station_id}"

asset_id = "Ton-M5StickC-0"     # Asset ID is a device name
url = f"http://192.168.137.1:5000/api/asset/{asset_id}"

resp = requests.get(url=url)
if resp.status_code == 200:
    print(resp.json())
else:
    print(resp.status_code, resp.text)
