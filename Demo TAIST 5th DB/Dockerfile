FROM python:3.9

WORKDIR /python-docker

COPY requirements.txt ./

RUN pip install -r requirements.txt

COPY . .

EXPOSE 8080

# CMD ["uvicorn", "app.main:app"]