#ifndef __COMM_H__
#define __COMM_H__

#include <PubSubClient.h>
#include <WiFi.h>

void setup_wifi(void);
void setup_mqtt(void);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect(void);

extern const char* ssid;
extern const char* password;
extern const char* mqtt_server; 
 
extern const char* device_name; 
extern const char* pair_topic;
extern const char* dev_topic;

extern char data_topic[100];
extern char cmd_topic[100];
 
extern WiFiClient espClient;
extern PubSubClient client;

#endif