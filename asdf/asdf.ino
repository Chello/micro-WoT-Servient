#include <ESP32WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char* ssid = "asdf";
const char* password = "";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";

String thingName = "asdf";
String td = "";

DeserializationError err;

int properties_number = 0;
int objectProperties_number = 0;
int actions_number = 0;
int events_number = 0;




// Requests
String req1 = "/";
String req2 = "/" + thingName;
String req3 = "/" + thingName + "/all/properties";

ESP32WebServer server(portServer);

IPAddress ipS;


int i, j, k, n;

void setup() {
    Serial.begin(115200);
    Serial.println();
  
    connection(ssid, password);
    
    td = "{\"title\":\"asdf\",\"id\":\"asdf\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"writeallproperties\"]}]}";

    // Server requests
    server.on(req1,HTTP_GET,handleReq1);
    server.on(req2,HTTP_GET,handleReq2);
    server.on(req3,HTTP_GET,handleReq3);

    server.begin();

    Serial.println("Server started");
    Serial.println(urlServer);
}    

void loop() {

    // handle Requests
    server.handleClient();
}

void connection(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    
    Serial.print("\nConnecting to ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected");
    Serial.print("IP address: ");
    ipS = WiFi.localIP();
    Serial.println(ipS);

    urlServer = protocolServer + "://" + ipS.toString() + ":" + portServer + "/" + thingName;
}

// Request functions
void handleReq1() {
    String resp = "";

    Serial.println("\nGET Thing URL");
    resp = "[\"" + urlServer + "\"]";
    server.send(200, "application/ld+json", resp);
}

void handleReq2() {
    Serial.println("\nGET Thing Description"); 
    server.send(200, "application/ld+json", td);
}

void handleReq3() {
    String resp = "";

    resp = request3();
    server.send(200, "application/ld+json", resp);
}



String request3() {
    DynamicJsonDocument tmp(20);
    String resp = "";
    JsonObject obj = tmp.createNestedObject();

    Serial.println("\nGET all properties");
    serializeJson(obj, resp);

    return resp;
}



