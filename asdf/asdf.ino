

//pippoo
{'td': ['http', 'ws'], 'properties': {'proprieta': ['http', 'ws']}, 'events': {'evt': ['http', 'ws']}, 'actions': {'act1': ['ws', 'http'], 'act2': ['ws']}}#include <ArduinoJson.h>
#include "HTTP_LongPoll/HTTP_LongPoll.h"
#include "WebSocket/WebSocketBinding.h"

const char* ssid = "Rachelli-net";
const char* password = "3eKLtrdFwfQXgpv!";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";
String protocolSocket = "ws";
int portSocket = 81;
String urlSocket = "";

String thingName = "asdf";
String td = "";


DynamicJsonDocument ws_es_doc(20);
// Json Array to store the ip addresses of clients connected to WebSocket channel for Events requests   
JsonArray ipe_arr;
DeserializationError err;

int properties_number = 1;
int objectProperties_number = 0;
int actions_number = 2;
int events_number = 1;

// Properties
const char* property1_name = "proprieta";
bool property1_value = false;


// Actions
const char* action1_name = "act1";
int action1_inputsNumber = 0;
String action1_schema[0] = {};
const char* action2_name = "act2";
int action2_inputsNumber = 0;
String action2_schema[0] = {};

// Events
const char* event1_name = "evt";
bool events_subscriptionSchema[1] = {false};
bool events_dataSchema[1] = {false};
bool events_cancellationSchema[1] = {false};
String events_list[1] = {event1_name};
String events_endpoint[1] = {"/" + thingName + "/events/" + event1_name};

// Endpoints
String req5 = "/" + thingName + "/actions/" + action1_name;
String req6 = "/" + thingName + "/actions/" + action2_name;
String req4 = "/" + thingName + "/properties/" + property1_name;
String req3 = "/" + thingName + "/all/properties";

String req2 = "/" + thingName;

String req1 = "/";

IPAddress ipS;
//Longpoll object handler
HTTP_LongPoll *hlp;
//WebSocket object handler
WebSocketBinding *wsb;

//HTTP - actions
const String http_actions_endpoint[1] = { req5 };
actions_handler http_actions_callback[1] = { request5 };

//WS - actions
const String ws_actions_endpoint[2] = { req5, req6 };
actions_handler ws_actions_callback[2] = { request5, request6 };

//HTTP - Properties
const String http_properties_endpoint[4] = { req1, req2, req3, req4 };
properties_handler http_properties_callback[4] = { request1, request2, request3, request4 };

//WS - Properties
const String ws_properties_endpoint[4] = { req1, req2, req3, req4 };
properties_handler ws_properties_callback[4] = { request1, request2, request3, request4 };
int i, j, k, n;

void setup() {
    Serial.begin(115200);
    Serial.println();

    // events data

    ipe_arr = ipe_doc.createNestedArray("clients_list");
    ipia_arr = ipia_doc.createNestedArray("clients_list");
  
    connection(ssid, password);
    
    td = "{\"title\":\"asdf\",\"id\":\"asdf\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"writeallproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]}],\"links\":[],\"properties\":{\"proprieta\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/properties/"+property1_name+"\",\"op\":[\"readproperty\",\"writeproperty\"]}],\"type\":\"boolean\",\"observable\":false,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{\"act1\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"}],\"safe\":false,\"idempotent\":false},\"act2\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action2_name+"\",\"op\":\"invokeaction\"}],\"safe\":false,\"idempotent\":false}},\"events\":{\"evt\":{\"eventName\":\"evt\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[]},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]}],\"actionsTriggered\":[\"act\"],\"condition\":\"true\"}}}";

    // Server requests
    server.on(r.c_str(),e,handleReq4);
    server.on(r.c_str(),e,handleReq3);
    server.on(r.c_str(),e,handleReq2);
    server.on(r.c_str(),e,handleReq1);

    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("Server started");
    Serial.println(urlServer);
}    

void loop() {
    // handle Requests via WebSocket
    wsb->loop();
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

String request5(String body) {
    DynamicJsonDocument resp_doc(20);
    String resp = "";

    Serial.printf("\nPOST invokeaction %s\n", action1_name);
    Serial.printf("Body received: %s\n", body.c_str());
    
    err = deserializeJson(resp_doc, body);
    if(err) {
        Serial.printf("deserializeJson() failed with code %s", err.c_str());
        resp = err.c_str();
        return resp;
    }
    else {
        act1(); 
        resp = "";
    }
    return resp;
}
String request6(String body) {
    DynamicJsonDocument resp_doc(20);
    String resp = "";

    Serial.printf("\nPOST invokeaction %s\n", action2_name);
    Serial.printf("Body received: %s\n", body.c_str());
    
    err = deserializeJson(resp_doc, body);
    if(err) {
        Serial.printf("deserializeJson() failed with code %s", err.c_str());
        resp = err.c_str();
        return resp;
    }
    else {
        act2(); 
        resp = "";
    }
    return resp;
}

// handle Input Types
bool handleInputType(String value, String schema) {   
	DynamicJsonDocument schema_doc(20);
    bool validInput = true;

    deserializeJson(schema_doc, schema);
    JsonObject obj_schema = schema_doc.as<JsonObject>();
    String type = obj_schema["type"];
    
    if(value[0] == '"')
        value.remove(0);
    if(value[value.length()-1] == '"')    
        value.remove(value.length()-1);
    
    return validInput;
}

// Action functions
void act1() {
	return in1;
	
}

void act2() {
	delay(90);
	
}

