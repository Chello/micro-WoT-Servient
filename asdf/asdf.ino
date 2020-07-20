

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

// Requests
String req5 = "/" + thingName + "/actions/" + action1_name;
String req6 = "/" + thingName + "/actions/" + action2_name;
String req4 = "/" + thingName + "/properties/" + property1_name;
String req3 = "/" + thingName + "/all/properties";

String req2 = "/" + thingName;

String req1 = "/";

int ws_requestsNumber = 3;
int ws_actionsNumber = 1;
String ws_requests[3] = {thingName,property1_name,action1_name};
String ws_endpoint[3] = {req3,req4,req5}; 
String ws_actions[1] = {action1_name,};

AsyncWebServer server(portServer);
WebSocketsServer webSocket = WebSocketsServer(portSocket);

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


// handle Events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* pl, size_t length) {
    IPAddress ip;
    String ip_s = "";
    const char* payload = (char *) pl;
    String resp = "";

    switch(type) {
        case WStype_DISCONNECTED: {
            bool deleted = false;
            Serial.printf("\n%d", num);
            Serial.println(" -> Disconnected");

            // remove all client subscriptions
            for(i=0; i<ipe_arr.size(); i++)
                if(ipe_arr[i]["num"] == num) {
                    deleted = true;
                    serializeJson(ipe_arr[i]["ip"], ip_s);
                    ip_s.replace("\"", "");
                    ipe_arr.remove(i);
                    ipe_doc.remove(ip_s);
                    e_doc.remove(ip_s);
                    break;
                }
    
            
            if(!deleted)
                // remove all client requests
                for(i=0; i<ipia_arr.size(); i++)
                    if(ipia_arr[i]["num"] == num) {
                        ip_s = "";
                        serializeJson(ipia_arr[i]["ip"], ip_s);
                        ip_s.replace("\"", "");
                        ipia_arr.remove(i);
                        ipia_doc.remove(ip_s);
                        ia_doc.remove(ip_s);
                        break;
                    }     
    

            serializeJson(e_doc, Serial);
            Serial.println();
            serializeJson(ipe_doc, Serial);
            Serial.println();
            serializeJson(ia_doc, Serial);
            Serial.println();
            serializeJson(ipia_doc, Serial);
            Serial.println();
        }
        break;

        case WStype_CONNECTED: {
            bool done = false;
            bool conn = false;
            JsonObject obj_e;
            JsonObject obj_ipe;
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
            while(!done && i<events_number) {
                if(events_endpoint[i].equals(payload)) {
                    if(e_doc[ip_s].isNull()) {
                        obj_e = e_doc.createNestedArray(ip_s).createNestedObject();
                        obj_ipe = ipe_arr.createNestedObject();
                        obj_ipe["ip"] = ip_s;
                        obj_ipe["num"] = num;
                        ipe_doc[ip_s] = num;
                    }
                    else {
                        j = 0;
                        while(!conn && j<e_doc[ip_s].size()) {
                            if(!e_doc[ip_s][j][events_list[i]].isNull())
                                conn = true;
                            j++;    
                        }
                        if(!conn)
                            obj_e = e_doc[ip_s].createNestedObject();  
                    }
                    
                    if(conn) {
                        done = true;
                        webSocket.sendTXT(num, "Connection already established");
                    }
                    else {
                        done = true;
                        webSocket.sendTXT(num, "Connection confirmed");
                        if(events_subscriptionSchema[i]) 
                            obj_e[events_list[i]] = false;
                        else {
                            obj_e[events_list[i]] = true;  
                            webSocket.sendTXT(num, "Subscription confirmed");  
                        }
                    }
                }
                i++;
            }
            
            if(!done) {
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
                                                        resp = request5("{}");
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
                                else if(ws_requests[i].equals(property1_name))
                                    resp = request4();

                                webSocket.sendTXT(num, resp);  
                            }
                        }
                    }
                    i++;
                }
            }

            serializeJson(e_doc, Serial);
            Serial.println();
            serializeJson(ipe_doc, Serial);
            Serial.println();
            serializeJson(ia_doc, Serial);
            Serial.println();
            serializeJson(ipia_doc, Serial);
            Serial.println();
        }
        break;

        case WStype_TEXT:
        { 
            DynamicJsonDocument resp_doc(40);
            bool parsingDone = false;
            int validInput = 0;
            String t_name = "";
            String validate = "";
            String message = "";

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
                String message = "";
                serializeJson(resp_doc, message);
                i = 0;
                while(!parsingDone && i<e_doc[ip_s].size()) {
                    j = 0;
                    while(!parsingDone && j<events_number) {
                        if(!e_doc[ip_s][i][events_list[j]].isNull()) {
                            if(!e_doc[ip_s][i][events_list[j]] && events_subscriptionSchema[j]) {
                                DynamicJsonDocument tmp_doc(40);
                                JsonObject tmp = tmp_doc.createNestedObject();
                                validInput = 0;
                                k = 0;
                                while(!parsingDone && k<es_doc[events_list[j]]["subscription"].size()) {
                                    t_name = "";
                                    serializeJson(es_doc[events_list[j]]["subscription"][k]["name"], t_name);
                                    t_name.replace("\"", "");
                                    if(!resp_doc[t_name].isNull()) {
                                        tmp[t_name] = es_doc[events_list[j]]["subscription"][k]["value"];
                                        validInput++;
                                    }
                                    else  
                                        parsingDone = false;
                                    k++;
                                }
                                if(validInput == k) {
                                    parsingDone = true;
                                    validate = "";
                                    serializeJson(tmp, validate);
                                    if(validate.equals(message)) {
                                        e_doc[ip_s][i][events_list[j]] = true;
                                        webSocket.sendTXT(num, "Subscription confirmed");
                                    }
                                    else 
                                        webSocket.sendTXT(num, "Subscription refused");
                                }
                            }
                            else if(e_doc[ip_s][i][events_list[j]] && events_cancellationSchema[j]) {
                                DynamicJsonDocument tmp_doc(40);
                                JsonObject tmp = tmp_doc.createNestedObject();
                                validInput = 0;
                                k = 0;
                                while(!parsingDone && k<es_doc[events_list[j]]["cancellation"].size()) {
                                    t_name = "";
                                    serializeJson(es_doc[events_list[j]]["cancellation"][k]["name"], t_name);
                                    t_name.replace("\"", "");
                                    if(!resp_doc[t_name].isNull()) {                                   
                                        tmp[t_name] = es_doc[events_list[j]]["cancellation"][k]["value"];
                                        validInput++;
                                    }
                                    else
                                        parsingDone = false;
                                    k++;
                                }
                                if(validInput == k) {
                                    parsingDone = true;
                                    validate = "";
                                    serializeJson(tmp, validate);
                                    if(validate.equals(message)) {
                                        e_doc[ip_s][i][events_list[j]] = false;
                                        webSocket.sendTXT(num, "Cancellation confirmed");
                                    }
                                    else
                                        webSocket.sendTXT(num, "Cancellation refused");
                                }
                            }  
                        }
                        j++;
                    }
                    i++;
                }
                if(!parsingDone) {
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
                                                    resp = request5(payload);
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
                }
                
                if(!parsingDone) 
                    webSocket.sendTXT(num, "Invalid message");
            }

            serializeJson(e_doc, Serial);
            Serial.println();
            serializeJson(ipe_doc, Serial);
            Serial.println();
            serializeJson(ia_doc, Serial);
            Serial.println();
            serializeJson(ipia_doc, Serial);
            Serial.println();
        }
        break;
    }
}
