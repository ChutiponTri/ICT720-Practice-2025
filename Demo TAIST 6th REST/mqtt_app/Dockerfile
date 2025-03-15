FROM python:3.12

WORKDIR /mqtt_app

COPY requirements.txt ./

RUN pip install -r requirements.txt

COPY . .

# CMD ["python", "-u", "app.py"]
