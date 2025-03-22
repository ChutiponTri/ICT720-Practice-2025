from flask import Flask, request, abort, send_from_directory, render_template
import requests
import json
import os
import sys

from linebot.v3 import (
    WebhookHandler
)
from linebot.v3.models import (
    UnknownEvent
)
from linebot.v3.exceptions import (
    InvalidSignatureError
)
from linebot.v3.webhooks import (
    MessageEvent,
    TextMessageContent,
    LocationMessageContent,
    StickerMessageContent,
    ImageMessageContent,
    VideoMessageContent,
    AudioMessageContent,
    FileMessageContent,
    UserSource,
    RoomSource,
    GroupSource,
    FollowEvent,
    UnfollowEvent,
    JoinEvent,
    LeaveEvent,
    PostbackEvent,
    BeaconEvent,
    MemberJoinedEvent,
    MemberLeftEvent,
)
from linebot.v3.messaging import (
    Configuration,
    ApiClient,
    MessagingApi,
    MessagingApiBlob,
    ReplyMessageRequest,
    PushMessageRequest,
    MulticastRequest,
    BroadcastRequest,
    TextMessage,
    ApiException,
    LocationMessage,
    StickerMessage,
    ImageMessage,
    TemplateMessage,
    FlexMessage,
    Emoji,
    QuickReply,
    QuickReplyItem,
    ConfirmTemplate,
    ButtonsTemplate,
    CarouselTemplate,
    CarouselColumn,
    ImageCarouselTemplate,
    ImageCarouselColumn,
    FlexBubble,
    FlexImage,
    FlexBox,
    FlexText,
    FlexIcon,
    FlexButton,
    FlexSeparator,
    FlexContainer,
    MessageAction,
    URIAction,
    PostbackAction,
    DatetimePickerAction,
    CameraAction,
    CameraRollAction,
    LocationAction,
    ErrorResponse
)
from gemini import Assistance

# Get environment variables
devices = ['Asset-T0', 'Asset-T1', 'Asset-T2', 'Ton-M5StickC-0', 'Ton-M5StickC-1', 'Ton-M5StickC-2', 'Ton-M5StickC-3']
stations = ["station1", "station2", "station3"]
channel_secret = os.getenv("LINE_CHANNEL_SECRET", None)
channel_access_token = os.getenv("LINE_CHANNEL_ACCESS_TOKEN", None)
liff_id = os.getenv("LIFF_ID", None)
rest_station_api = os.getenv("REST_STATION_API", None)
rest_asset_api = os.getenv("REST_ASSET_API", None)
ai_assistance = Assistance()
if channel_secret is None or channel_access_token is None or rest_station_api is None or rest_asset_api is None:
    print("Specify LINE_CHANNEL_SECRET and LINE_CHANNEL_ACCESS_TOKEN as environment variables.")
    sys.exit(1)

# init app
app = Flask(__name__)
handler = WebhookHandler(channel_secret)
configuration = Configuration(
    access_token=channel_access_token
)

# standard LINE webhook
@app.route("/webhook", methods=["POST"])
def line_webhook():
    signature = request.headers["X-Line-Signature"]
    body = request.get_data(as_text=True)
    try:
        handler.handle(body, signature)
    except InvalidSignatureError:
        abort(400)
    return "OK"

# text message handler
@handler.add(MessageEvent, message=TextMessageContent)
def handle_text_message(event):
    text = event.message.text     
    with ApiClient(configuration) as api_client:
        line_bot_api = MessagingApi(api_client)
        # check station
        station = [i for i in stations if i.lower() in text.lower()]
        device = [i for i in devices if i.lower() in text.lower()]
        if station:
            for i in station:
                req = requests.get(rest_station_api + i)
                data = req.json()
                # resp_text = str(data["data"])
                resp_text = ai_assistance.prompt(text, data).content
                line_bot_api.reply_message(
                    ReplyMessageRequest(
                        reply_token=event.reply_token,
                        messages=[TextMessage(text=resp_text)]
                    )
                )
        # check asset
        elif device:
            for i in device:
                req = requests.get(rest_asset_api + i)
                data = req.json()
                # resp_text = str(data["data"])
                resp_text = ai_assistance.prompt(text, data).content
                line_bot_api.reply_message(
                    ReplyMessageRequest(
                        reply_token=event.reply_token,
                        messages=[TextMessage(text=resp_text)]
                    )
                )
        # not commands
        else:
            line_bot_api.reply_message(
                ReplyMessageRequest(
                    reply_token=event.reply_token,
                    messages=[TextMessage(text=text)]
                )
            )

# LIFF
@app.route("/liff", methods=["GET"])
def liff():
    return render_template("liff.html", liff_id=liff_id)
