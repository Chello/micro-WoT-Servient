#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "AsyncLongPoll/AsyncLongPoll.h"
#include "WebSocket/WebSocketBinding.h"

const char* ssid = "Rachelli-net";
const char* password = "3eKLtrdFwfQXgpv!";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";
String protocolSocket = "ws";
int portSocket = 81;
String urlSocket = "";

String thingName = "test4";
String td = "";

DeserializationError err;

int properties_number = 1;
int objectProperties_number = 0;
int actions_number = 1;
int events_number = 2;

// Properties
const char* property1_name = "proprieta";
bool property1_value = false;


// Actions
const char* action1_name = "azione";
int action1_inputsNumber = 1;
String action1_schema[1] = {"{\"name\":\"in\",\"type\":\"string\"}"};

// Events
const char* event1_name = "evento";
const char* event2_name = "evento2";
bool events_subscriptionSchema[2] = {false,false};
bool events_dataSchema[2] = {false,false};
bool events_cancellationSchema[2] = {false,false};
String events_list[2] = {event1_name,event2_name,};
String events_endpoint[2] = {"/" + thingName + "/events/" + event1_name,"/" + thingName + "/events/" + event2_name,};

// Requests


    String req5 = "/" + thingName + "/actions/" + action1_name;

    String req4 = "/" + thingName + "/properties/" + property1_name;

String req3 = "/" + thingName + "/all/properties";

String req2 = "/" + thingName;

String req1 = "/";

int ws_requestsNumber = 3;
int ws_actionsNumber = 1;
String ws_requests[3] = {thingName,property1_name,action1_name};
String ws_endpoint[3] = {req3,req4,req5}; 
String ws_actions[1] = {action1_name};

AsyncWebServer server(portServer);
//WebSocketsServer webSocket = WebSocketsServer(portSocket);

IPAddress ipS;
//Longpoll object handler
AsyncLongPoll *alp;
//WebSocket object handler
WebSocketBinding *wsb;


int i, j, k, n;

void setup() {
    Serial.begin(115200);
    Serial.println();
  
    connection(ssid, password);
    
    td = "{\"title\":\"test4\",\"id\":\"test4\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]}],\"properties\":{\"proprieta\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]}],\"type\":\"boolean\",\"observable\":false,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{\"azione\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"}],\"input\":{\"in\":{\"type\":\"string\"}},\"output\":{\"type\":\"string\"},\"safe\":false,\"idempotent\":false}},\"events\":{\"evento\":{\"eventName\":\"evento\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"],\"subprotocol\":\"longpoll\"}],\"actionsTriggered\":[\"azione\"],\"condition\":\"true\"},\"evento2\":{\"eventName\":\"evento2\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"],\"subprotocol\":\"longpoll\"}],\"actionsTriggered\":[\"azione\"],\"condition\":\"true\"}}}";

    // Server requests
    server.on(events_endpoint[1].c_str(),HTTP_GET,handleReq8);
    server.on(events_endpoint[0].c_str(),HTTP_GET,handleReq7);
    server.on(req5.c_str(),HTTP_GET,handleReq6);
    server.on(req5.c_str(),HTTP_POST,handleReq5);
    server.on(req4.c_str(),HTTP_GET,handleReq4);
    server.on(req3.c_str(),HTTP_GET,handleReq3);
    server.on(req2.c_str(),HTTP_GET,handleReq2);
    server.on(req1.c_str(),HTTP_GET,handleReq1);

    wsb = new WebSocketBinding(portSocket);
    server.begin();
    alp = new AsyncLongPoll();
    Serial.printf("Server started at IP ");
    Serial.println(urlServer);
}    

void loop() {

    // handle Requests
    wsb->webSocketLoop();
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
void handleReq1(AsyncWebServerRequest *req) {
    String resp = "";

    Serial.println("\nGET Thing URL");
    resp = "[\"" + urlServer + "\"]";
    req->send(200, "application/ld+json", resp);
}

void handleReq2(AsyncWebServerRequest *req) {
    Serial.println("\nGET Thing Description"); 
    req->send(200, "application/ld+json", td);
}

void handleReq3(AsyncWebServerRequest *req) {
    String resp = "";

    resp = request3();
    req->send(200, "application/ld+json", resp);
}

void handleReq4(AsyncWebServerRequest *req) {
    String resp = "";
    
    resp = request4();
    req->send(200, "application/ld+json", resp);
}

void handleReq5(AsyncWebServerRequest *req) {
    String resp = "";
    String body = req->arg("plain");
    
    resp = request5(body);
    req->send(200, "application/ld+json", resp);
}

void handleReq6(AsyncWebServerRequest *req) {
    char* resp = "Method Not Allowed";
    req->send(405, "text/plain", resp);
}

void handleReq7(AsyncWebServerRequest *req) {
    alp->longPollHandler(req, event1_name);
}

void handleReq8(AsyncWebServerRequest *req) {
    alp->longPollHandler(req, event2_name);
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
    DynamicJsonDocument resp_doc(200);
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
        if(resp_doc["in"].isNull())
            resp = "InvalidInput";
        else {
            bool validInput = true;
            String value = "";

            String action1_input[1] = {};    
            String action1_input1_value = "";

            i = 0;
            while(validInput and i<action1_inputsNumber) {
                switch(i) {
                    case 0: {
                        value = "";
                        serializeJson(resp_doc["in"], value);
                        action1_input[0] = value;
                        validInput = handleInputType(value,action1_schema[0]);
                    }
                    break;

                }
                i++;
            }    

            if(validInput) {

                action1_input1_value = action1_input[0];

                String output = azione(action1_input1_value);    
                resp = (String) output;
                String ws_msg = "{\"output\" : \"pippo\"}";

                // evento condition
                if(true) {
                    alp->sendLongPollTXT(ws_msg, event1_name);
                    wsb->sendWebSocketTXT(ws_msg, event1_name);
                }

                // evento2 condition
                if(true) {
                    alp->sendLongPollTXT(ws_msg, event2_name);
                    wsb->sendWebSocketTXT(ws_msg, event2_name);
                    // for(i=0; i<ipe_arr.size(); i++) {
                    //     String ws_ip = ipe_arr[i]["ip"];
                    //     JsonArray ae = e_doc[ws_ip];
                    //     for(j=0; j<ae.size(); j++) {
                    //         if(!ae[j][event2_name].isNull() && ae[j][event2_name]) {
                    //             unsigned char ws_num = ipe_doc[ws_ip];
                    //             webSocket.sendTXT(ws_num, ws_msg);
                    //         }
                    //     }
                    // }
                }
            }
            else
                resp = "InvalidInput";
        }
    }
    return resp;
}

// handle Input Types
bool handleInputType(String value, String schema) {   
	DynamicJsonDocument schema_doc(200);
    bool validInput = true;

    deserializeJson(schema_doc, schema);
    JsonObject obj_schema = schema_doc.as<JsonObject>();
    String type = obj_schema["type"];
    
    if(value[0] == '"')
        value.remove(0);
    if(value[value.length()-1] == '"')    
        value.remove(value.length()-1);
    
    if(type.equals("string")) {
        if(value.equalsIgnoreCase("null")) 
            validInput = false;
    
    }
    return validInput;
}

// Action functions
String azione(String in) {
	return "in";
}
