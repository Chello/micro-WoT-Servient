
#include <ArduinoJson.h>
#include <embeddedWoT_HTTP_LongPoll.h>

const char* ssid = "Rachelli-net";
const char* password = "3eKLtrdFwfQXGpv!";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";

String thingName = "coap-test";
String td = "";


DynamicJsonDocument ws_es_doc(20);
// Json Array to store the ip addresses of clients connected to WebSocket channel for Events requests   
JsonArray ipe_arr;
DeserializationError err;

int properties_number = 0;
int objectProperties_number = 0;
int actions_number = 0;
int events_number = 0;




// Endpoints
String req3 = "/" + thingName + "/all/properties";
String req2 = "/" + thingName;
String req1 = "/";

IPAddress ipS;
//Longpoll object handler
embeddedWoT_HTTP_LongPoll *hlp;

int i, j, k, n;

String request1();
String request2();
String request3();

//HTTP - actions
const int http_actions_num = 0;
const String http_actions_endpoint[http_actions_num] = {  };
actions_handler http_actions_callback[http_actions_num] = {  };

//WS - actions
const int ws_actions_num = 0;
const String ws_actions_endpoint[ws_actions_num] = {  };
actions_handler ws_actions_callback[ws_actions_num] = {  };

//HTTP - Properties
const int http_properties_num = 3;
const String http_properties_endpoint[http_properties_num] = { req1, req2, req3 };
properties_handler http_properties_callback[http_properties_num] = { request1, request2, request3 };

//WS - Properties
const int ws_properties_num = 0;
const String ws_properties_endpoint[ws_properties_num] = {  };
properties_handler ws_properties_callback[ws_properties_num] = {  };

//HTTP - events
const int http_events_num = 0;
const String http_events_endpoint[http_events_num] = {  };
//WS - events
const int ws_events_num = 0;
const String ws_events_endpoint[ws_events_num] = {  };

void setup() {
    Serial.begin(115200);
    Serial.println();
  
    connection(ssid, password);
    
    td = "{\"title\":\"coap-test\",\"id\":\"coap-test\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"writemultipleproperties\"]}],\"links\":[],\"properties\":{},\"actions\":{},\"events\":{}}";

    hlp = new embeddedWoT_HTTP_LongPoll(portServer);

    hlp->exposeActions(http_actions_endpoint, http_actions_callback, http_actions_num);
    hlp->exposeEvents(http_events_endpoint, http_events_num);
    hlp->exposeProperties(http_properties_endpoint, http_properties_callback, http_properties_num);

    hlp->begin();
    Serial.println("Server started");
    Serial.println(urlServer);

    
}    

void loop() {
    
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
String request1() {
    String resp = "";

    Serial.println("\nGET Thing URL");
    resp = "[\"" + urlServer + "\"]";
    return resp;
}

String request2() {
    Serial.println("\nGET Thing Description"); 
    return td;
}

String request3() {
    DynamicJsonDocument tmp(20);
    String resp = "";
    JsonObject obj = tmp.createNestedObject();

    Serial.println("\nGET all properties");
    serializeJson(obj, resp);

    return resp;
}


