import streamlit as st
import pandas as pd
import numpy as np
import requests
import json
import time
from langchain_google_genai import ChatGoogleGenerativeAI
from langchain.schema import HumanMessage, AIMessage
from dotenv import load_dotenv
import os

load_dotenv()
class Assistance():
    def __init__(self):
        # Set up the LLM model
        self.llm = ChatGoogleGenerativeAI(model="gemini-2.0-flash", api_key=os.getenv("API_KEY"))

        # chat history
        if "messages" not in st.session_state:
            st.session_state.messages = []

        self.header()
        self.chat_tab()
        self.data_tab()

    # Function to Display Header
    def header(self):
        # UI for API key
        st.title("GenAI App")
        st.sidebar.title("Settings")
        asset_id = st.sidebar.selectbox("Asset ID",["Ton-M5StickC-0", "Ton-M5StickC-1", "Ton-M5StickC-2", "Asset-T0", "Asset-T1", "Asset-T2"])
        self.asset_data = self.fetch_asset_data(asset_id)
        self.context_json = json.dumps(self.asset_data, indent=2)

        # UI for chatbot
        self.chat_ui, self.data_ui = st.tabs(["Chatbot", "Raw data"])

        self.user_input = st.chat_input("Question: ...")
        if self.user_input:
            self.message()

    # Function to Create Message
    def message(self):
        st.session_state.messages.append(HumanMessage(content=self.user_input))

        # Construct prompt with context
        prompt = f"""
        Now, answer the following question based on the JSON data:
        {self.user_input}
        Do not give JSON data in the answer.

        Here is the info about the asset tracking with Blutooth Low Energy (BLE) technology.
        {self.context_json}

        JSON data contains the following fields:
        "asset" is the asset ID
        "status" is the status of query operation
        "data" is the record of observations containing the following three fields:
            "timestamp" is the time of observation
            "rssi" is the received signal strength indicator, bigger is closer to the station
            "station" is the id of BLE station that made the observation

        """

        # Generate response
        self.response = self.llm.invoke([HumanMessage(content=prompt)])
        
    # Function to Display ChatUI
    def chat_tab(self):  
        with self.chat_ui:
            for msg in st.session_state.messages:
                role = "user" if isinstance(msg, HumanMessage) else "assistant"
                with st.chat_message(role):
                    st.write(msg.content)
            if self.user_input:
                with st.chat_message("assistant"):
                    st.write_stream(self.stream_data(self.response.content))
                    st.session_state.messages.append(AIMessage(content=self.response.content))

    # Function to Display DataUI
    def data_tab(self):
        with self.data_ui:
            st.json(self.asset_data)

    # Function to fetch user data from an API
    def fetch_asset_data(self, asset_id):
        url = f"http://localhost:5000/api/asset/{asset_id}?mins=100000&rssi=-100"
        response = requests.get(url)
        return response.json()

    # Function to Write Stream Message
    def stream_data(self, data:str):
        for word in data:
            yield word
            time.sleep(0.005)

if __name__ == "__main__":
    app = Assistance()