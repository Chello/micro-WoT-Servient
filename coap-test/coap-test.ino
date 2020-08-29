
#include <ArduinoJson.h>
#include <embeddedWoT_HTTP_LongPoll.h>
#include <embeddedWoT_WebSocket.h>
#include <embeddedWoT_CoAP.h>

const char* ssid = "Rachelli-net";
const char* password = "3eKLtrdFwfQXgpv!";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";
String protocolSocket = "ws";
int portSocket = 81;
String urlSocket = "";
int portCoap = 5683;

String thingName = "coap-test";
String td = "";


DynamicJsonDocument ws_es_doc(20);
// Json Array to store the ip addresses of clients connected to WebSocket channel for Events requests   
JsonArray ipe_arr;
DeserializationError err;

int properties_number = 1;
int objectProperties_number = 0;
int actions_number = 0;
int events_number = 0;

// Properties
const char* property1_name = "prop";
bool property1_value = false;




// Endpoints
String req4 = "/" + thingName + "/properties/" + property1_name;
String req3 = "/" + thingName + "/all/properties";
String req2 = "/" + thingName;
String req1 = "/";

IPAddress ipS;
//Longpoll object handler
embeddedWoT_HTTP_LongPoll *hlp;
//WebSocket object handler
embeddedWoT_WebSocket *wsb;
//CoAP object handler
embeddedWoT_CoAP *coap;

int i, j, k, n;

String request1();
String request2();
String request3();
String request4();

//HTTP - actions
const int http_actions_num = 0;
const String http_actions_endpoint[http_actions_num] = {  };
actions_handler http_actions_callback[http_actions_num] = {  };

//WS - actions
const int ws_actions_num = 0;
const String ws_actions_endpoint[ws_actions_num] = {  };
actions_handler ws_actions_callback[ws_actions_num] = {  };

//HTTP - Properties
const int http_properties_num = 4;
const String http_properties_endpoint[http_properties_num] = { req1, req2, req3, req4 };
properties_handler http_properties_callback[http_properties_num] = { request1, request2, request3, request4 };

//WS - Properties
const int ws_properties_num = 4;
const String ws_properties_endpoint[ws_properties_num] = { req1, req2, req3, req4 };
properties_handler ws_properties_callback[ws_properties_num] = { request1, request2, request3, request4 };

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
    
    td = "{\"title\":\"coap-test\",\"id\":\"coap-test\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"writemultipleproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"readmultipleproperties\"]}],\"links\":[],\"properties\":{\"prop\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/properties/"+property1_name+"\",\"op\":[\"readproperty\",\"writeproperty\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]}],\"type\":\"boolean\",\"observable\":false,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{},\"events\":{}}";

    hlp = new embeddedWoT_HTTP_LongPoll(portServer);

    hlp->exposeActions(http_actions_endpoint, http_actions_callback, http_actions_num);
    hlp->exposeEvents(http_events_endpoint, http_events_num);
    hlp->exposeProperties(http_properties_endpoint, http_properties_callback, http_properties_num);

    hlp->begin();
    wsb = new embeddedWoT_WebSocket(portSocket);

    wsb->bindEventSchema(ws_es_doc);
    wsb->exposeActions(ws_actions_endpoint, ws_actions_callback, ws_actions_num);
    wsb->exposeEvents(ws_events_endpoint, ws_events_num);
    wsb->exposeProperties(ws_properties_endpoint, ws_properties_callback, ws_properties_num);

    coap = new embeddedWoT_CoAP(portCoap);
    coap->exposeActions(ws_actions_endpoint, ws_actions_callback, ws_actions_num);
    coap->exposeProperties(http_properties_endpoint, http_properties_callback, http_properties_num);
    coap->start();

    Serial.println("Server started");
    Serial.println(urlServer);
}    

void loop() {
    
    // handle Requests via WebSocket
    wsb->loop();
    coap->loop();
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
    urlSocket = protocolSocket + "://" + ipS.toString() + ":" + portSocket + "/" + thingName;
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
    DynamicJsonDocument tmp(220);
    String resp = "";
    JsonObject obj = tmp.createNestedObject();

    Serial.println("\nGET all properties");
    obj[property1_name] = property1_value;
    serializeJson(obj, resp);

    return resp;
}

String request4() {
    String resp = "";

    Serial.printf("\nGET %s value\n", property1_name);
    resp = "{\"" + (String) property1_name + "\":" + property1_value + "}";
    
    return resp;
}


