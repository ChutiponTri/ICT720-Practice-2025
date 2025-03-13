import requests
TOKEN = "YOUR BOT TOKEN"
BASE_URL = f"https://api.telegram.org/bot{TOKEN}"
chat = 7086001303

def get_updates():
    url = f"{BASE_URL}/getUpdates"
    response = requests.get(url)
    return response.json()

def send_message(chat_id, text):
    url = f"{BASE_URL}/sendMessage"
    data = {"chat_id": chat_id, "text": text}
    response = requests.post(url, data=data)
    return response.json()

if __name__ == "__main__":
    updates = get_updates()
    print(updates)

    if updates["result"]:
        chat_id = updates["result"][-1]["message"]["chat"]["id"]
        send_message(chat_id, "Hello Chigga Lu")   
