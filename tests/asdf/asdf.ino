#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "AsyncLongPoll/AsyncLongPoll.h"

const char* ssid = "asfsss";
const char* password = "";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";
String protocolSocket = "ws";
int portSocket = 81;
String urlSocket = "";

String thingName = "asdf";
String td = "";

// document to handle Interaction Affordances WebSocket requests
DynamicJsonDocument ia_doc(1000);
// document to store the ip addresses of clients connected to WebSocket channel for Interaction Affordances requests   
DynamicJsonDocument ipia_doc(2000);
JsonArray ipia_arr;
DeserializationError err;

int properties_number = 0;
int objectProperties_number = 0;
int actions_number = 1;
int events_number = 0;


// Actions
const char* action1_name = "act";
int action1_inputsNumber = 0;
String action1_schema[0] = {};


// Requests


    String req4 = "/" + thingName + "/actions/" + action1_name;


String req3 = "/" + thingName + "/all/properties";

String req2 = "/" + thingName;

String req1 = "/";

int ws_requestsNumber = 2;
int ws_actionsNumber = 1;
String ws_requests[2] = {thingName,action1_name};
String ws_endpoint[2] = {req3,req4}; 
String ws_actions[1] = {action1_name};

AsyncWebServer server(portServer);
WebSocketsServer webSocket = WebSocketsServer(portSocket);

IPAddress ipS;
//Longpoll object handler
AsyncLongPoll *alp;


int i, j, k, n;

void setup() {
    Serial.begin(115200);
    Serial.println();
    ipia_arr = ipia_doc.createNestedArray("clients_list");
  
    connection(ssid, password);
    
    td = "{\"title\":\"asdf\",\"id\":\"asdf\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"readallproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"readallproperties\"]}],\"actions\":{\"act\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"}],\"function\":{\"input\":[{\"inputName\":\"in\",\"type\":\"string\"}],\"output\":{\"type\":\"string\"},\"body\":\"return "popo";\"},\"safe\":false,\"idempotent\":false}}}";

    // Server requests
    server.on(req4.c_str(),HTTP_GET,handleReq5);
    server.on(req4.c_str(),HTTP_POST,handleReq4);
    server.on(req3.c_str(),HTTP_GET,handleReq3);
    server.on(req2.c_str(),HTTP_GET,handleReq2);
    server.on(req1.c_str(),HTTP_GET,handleReq1);

    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("Server started");
    Serial.println(urlServer);
}    

void loop() {

    // handle Requests
    webSocket.loop();
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
    String body = req->arg("plain");
    
    resp = request4(body);
    req->send(200, "application/ld+json", resp);
}

void handleReq5(AsyncWebServerRequest *req) {
    char* resp = "Method Not Allowed";
    req->send(405, "text/plain", resp);
}


String request3() {
    DynamicJsonDocument tmp(20);
    String resp = "";
    JsonObject obj = tmp.createNestedObject();

    Serial.println("\nGET all properties");
    serializeJson(obj, resp);

    return resp;
}

String request4(String body) {
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
        act(); 
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
void act() {
	
}


// handle Events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* pl, size_t length) {
    IPAddress ip;
    String ip_s = "";
    const char* payload = (char *) pl;
    String resp = "";

    switch(type) {
        case WStype_DISCONNECTED: {
            Serial.printf("\n%d", num);
            Serial.println(" -> Disconnected");

  
            // remove all client requests
            for(i=0; i<ipia_arr.size(); i++)
                if(ipia_arr[i]["num"] == num) {
                    serializeJson(ipia_arr[i]["ip"], ip_s);
                    ip_s.replace("\"", "");
                    ipia_arr.remove(i);
                    ipia_doc.remove(ip_s);
                    ia_doc.remove(ip_s);
                    break;
                }   
    

            serializeJson(ia_doc, Serial);
            Serial.println();
            serializeJson(ipia_doc, Serial);
            Serial.println();
        }
        break;

        case WStype_CONNECTED: {
            bool done = false;
            bool conn = false;
            JsonObject obj_ia;
            JsonObject obj_ipia;
            bool parsingDone = false;

            ip = webSocket.remoteIP(num);
            ip_s = ip.toString();
            Serial.printf("\n%d", num);
            Serial.print(" -> Connection request from ");
            Serial.println(ip);
            Serial.printf("Payload: %s\n", payload);


            i = 0;
            while(!done && i<ws_requestsNumber) {
                if(ws_endpoint[i].equals(payload)) {
                    if(ia_doc[ip_s].isNull()) {
                        obj_ia = ia_doc.createNestedArray(ip_s).createNestedObject();
                        obj_ipia = ipia_arr.createNestedObject();
                        obj_ipia["ip"] = ip_s;
                        obj_ipia["num"] = num;
                        ipia_doc[ip_s] = num;
                    }
                    else {
                        j = 0;
                        while(!conn && j<ia_doc[ip_s].size()) {
                            if(!ia_doc[ip_s][j][ws_requests[i]].isNull())
                                conn = true;
                            j++;    
                        }
                        if(!conn)
                            obj_ia = ia_doc[ip_s].createNestedObject();  
                    }
                    
                    if(conn) {
                        done = true;
                        webSocket.sendTXT(num, "Connection already established");
                    }
                    else {
                        // verify if the endpoint of this connection request refers to an Action
                        bool isAction = false;
                        j = 0;
                        while(!done && j<ws_actionsNumber) {
                            if(ws_requests[i].equals(ws_actions[j])) {
                                done = true;
                                isAction = true;
                                webSocket.sendTXT(num, "Connection confirmed");
                                k = 0;
                                while(!parsingDone && k<ws_actionsNumber) {
                                    switch(k) {
                                        case 0: {
                                            if(k == j) {
                                                if(action1_inputsNumber == 0) {
                                                    parsingDone = true;
                                                    obj_ia[ws_requests[i]] = true; 
                                                    resp = request4("{}");
                                                    webSocket.sendTXT(num, resp);
                                                }
                                            }
                                        }
                                        break;

                                    }
                                    k++;
                                }
                                if(!parsingDone)
                                    obj_ia[ws_requests[i]] = false;
                            }
                            j++;
                        }

                        if(!isAction) {
                            done = true;
                            obj_ia[ws_requests[i]] = true;  
                            
                            if(ws_requests[i].equals(thingName))
                                resp = request3();

                            webSocket.sendTXT(num, resp);  
                        }
                    }
                }
                i++;
            }

            serializeJson(ia_doc, Serial);
            Serial.println();
            serializeJson(ipia_doc, Serial);
            Serial.println();
        }
        break;

        case WStype_TEXT:
        { 
            DynamicJsonDocument resp_doc(0);
            bool parsingDone = false;

            ip = webSocket.remoteIP(num);
            ip_s = ip.toString();
            Serial.printf("\n%d", num);
            Serial.print(" -> Get Text from ");
            Serial.println(ip);
            Serial.printf("Payload: %s\n", payload);

            err = deserializeJson(resp_doc, payload);
            if(err) {
                Serial.printf("deserializeJson() failed with code %s", err.c_str());
                webSocket.sendTXT(num, "deserializeJson failed");
            }
            else {
                i = 0;
                while(!parsingDone && i<ia_doc[ip_s].size()) {
                    j = 0;
                    while(!parsingDone && j<ws_actionsNumber) { 
                        if(!ia_doc[ip_s][i][ws_actions[j]].isNull()) {
                            if(!ia_doc[ip_s][i][ws_actions[j]]) {
                                k = 0;
                                while(!parsingDone && k<ws_actionsNumber) {
                                    switch(k) {
                                        case 0: {
                                            if(k == j) {
                                                parsingDone = true;
                                                ia_doc[ip_s][i][ws_actions[j]] = true;
                                                resp = request4(payload);
                                                webSocket.sendTXT(num, resp);
                                            }
                                        }
                                        break;

                                    }
                                    k++;
                                }
                            }
                        }
                        j++;  
                    }        
                    i++; 
                }
                
                if(!parsingDone) 
                    webSocket.sendTXT(num, "Invalid message");
            }

            serializeJson(ia_doc, Serial);
            Serial.println();
            serializeJson(ipia_doc, Serial);
            Serial.println();
        }
        break;
    }
}
