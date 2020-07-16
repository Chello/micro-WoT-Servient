#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
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

String thingName = "test0";
String td = "";
/*
// document to handle Interaction Affordances WebSocket requests
DynamicJsonDocument ia_doc(2000);
// document to store the ip addresses of clients connected to WebSocket channel for Interaction Affordances requests   
DynamicJsonDocument ipia_doc(2000);
JsonArray ipia_arr;
// document to handle Events WebSocket requests
DynamicJsonDocument e_doc(2000);
// document to store the ip addresses of clients connected to WebSocket channel for Events requests   
DynamicJsonDocument ipe_doc(2000);*/
// document to handle Events Schemas
DynamicJsonDocument es_doc(356);
// Json Array to store the ip addresses of clients connected to WebSocket channel for Events requests   
JsonArray ipe_arr;
DeserializationError err;

int properties_number = 1;
int objectProperties_number = 0;
int actions_number = 2;
int events_number = 2;

// Properties
const char* property1_name = "proprieta";
int property1_value = 0;


// Actions
const char* action1_name = "azione1";
int action1_inputsNumber = 1;
String action1_schema[1] = {"{\"name\":\"stringa\",\"type\":\"string\"}"};
const char* action2_name = "azione2";
int action2_inputsNumber = 2;
String action2_schema[2] = {"{\"name\":\"intero\",\"type\":\"integer\"}","{\"name\":\"booleano\",\"type\":\"boolean\"}"};

// Events
const char* event1_name = "evento1";
String event1_subscriptionSchema[1] = {"{\"name\":\"sbs1\",\"value\":\"true\"}"};
String event1_dataSchema[1] = {"{\"name\":\"dataschema1\",\"value\":\"100\"}"};
String event1_cancellationSchema[1] = {"{\"name\":\"cnc1\",\"value\":\"true\"}"};
const char* event2_name = "evento2";
String event2_subscriptionSchema[1] = {"{\"name\":\"sbs2\",\"value\":\"true\"}"};
String event2_cancellationSchema[1] = {"{\"name\":\"cnc2\",\"value\":\"3\"}"};
bool events_subscriptionSchema[2] = {true,true};
bool events_dataSchema[2] = {true,false};
bool events_cancellationSchema[2] = {true,true};
String events_list[2] = {event1_name,event2_name,};

// Requests


    String req5 = "/" + thingName + "/actions/" + action1_name;
    String req6 = "/" + thingName + "/actions/" + action2_name;

    String req4 = "/" + thingName + "/properties/" + property1_name;

String req3 = "/" + thingName + "/all/properties";

String req2 = "/" + thingName;

String req1 = "/";

int ws_requestsNumber = 4;
int ws_actionsNumber = 2;
String ws_requests[4] = {thingName,property1_name,action1_name,action2_name};
String ws_endpoint[4] = {req3,req4,req5,req6}; 
String ws_actions[2] = {action1_name,action2_name};

// AsyncWebServer server(portServer);

IPAddress ipS;
//Longpoll object handler
HTTP_LongPoll *hlp;
//WebSocket object handler
WebSocketBinding *wsb;

const String ae[2] = {req5, req6};
const String pe[4] = {req4, req3, req2, req1};
const String events_endpoint[2] = {"/" + thingName + "/events/" + event1_name,"/" + thingName + "/events/" + event2_name};

String handleReq1();
String handleReq2();
String handleReq3();
String handleReq4();

String request5(String body);
String request6(String body);

properties_handler ph[4] = {handleReq4, handleReq3, handleReq2, handleReq1};
actions_handler ah[2] = {request5, request6};

int i, j, k, n;

void setup() {
    Serial.begin(115200);
    Serial.println();

    // events data
    int schema_size = 0;
    JsonArray arr;
    JsonObject obj;

    schema_size = sizeof(event1_subscriptionSchema) / sizeof(String);
    if(es_doc[events_endpoint[0]].isNull())
        arr = es_doc.createNestedObject(events_endpoint[0]).createNestedArray("subscription");
    else
        arr = es_doc[events_endpoint[0]].createNestedArray("subscription");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(82);
        deserializeJson(tmp_doc, event1_subscriptionSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }

    schema_size = sizeof(event1_dataSchema) / sizeof(String);
    if(es_doc[events_endpoint[0]].isNull())
        arr = es_doc.createNestedObject(events_endpoint[0]).createNestedArray("data");
    else
        arr = es_doc[events_endpoint[0]].createNestedArray("data");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(82);
        deserializeJson(tmp_doc, event1_dataSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }

    schema_size = sizeof(event1_cancellationSchema) / sizeof(String);
    if(es_doc[events_endpoint[0]].isNull())
        arr = es_doc.createNestedObject(events_endpoint[0]).createNestedArray("cancellation");
    else
        arr = es_doc[events_endpoint[0]].createNestedArray("cancellation");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(82);
        deserializeJson(tmp_doc, event1_cancellationSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }

    if(es_doc[events_endpoint[1]].isNull())
        arr = es_doc.createNestedObject(events_endpoint[1]).createNestedArray("subscription");
    else
        arr = es_doc[events_endpoint[1]].createNestedArray("subscription");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(82);
        deserializeJson(tmp_doc, event2_subscriptionSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }

    if(es_doc[events_endpoint[1]].isNull())
        arr = es_doc.createNestedObject(events_endpoint[1]).createNestedArray("cancellation");
    else
        arr = es_doc[events_endpoint[1]].createNestedArray("cancellation");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(82);
        deserializeJson(tmp_doc, event2_cancellationSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }

/*
    ipe_arr = ipe_doc.createNestedArray("clients_list");
    ipia_arr = ipia_doc.createNestedArray("clients_list");
    */
    connection(ssid, password);
    
    td = "{\"title\":\"test0\",\"id\":\"test0\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]}],\"links\":[],\"properties\":{\"proprieta\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/properties/"+property1_name+"\",\"op\":[\"readproperty\",\"writeproperty\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/properties/"+property1_name+"\",\"op\":[\"readproperty\",\"writeproperty\"]}],\"type\":\"integer\",\"observable\":false,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{\"azione1\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"}],\"input\":{\"stringa\":{\"type\":\"string\"}},\"output\":{\"type\":\"boolean\"},\"safe\":false,\"idempotent\":false},\"azione2\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action2_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action2_name+"\",\"op\":\"invokeaction\"}],\"input\":{\"intero\":{\"type\":\"integer\"},\"booleano\":{\"type\":\"boolean\"}},\"output\":{\"type\":\"string\"},\"safe\":false,\"idempotent\":false}},\"events\":{\"evento1\":{\"eventName\":\"evento1\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"],\"subprotocol\":\"longpoll\"}],\"actionsTriggered\":[\"azione1\"],\"condition\":\"true\",\"subscription\":{\"sbs1\":{\"type\":\"boolean\",\"value\":\"true\"}},\"data\":{\"dataschema1\":{\"type\":\"integer\",\"value\":\"100\"}},\"cancellation\":{\"cnc1\":{\"type\":\"boolean\",\"value\":\"true\"}}},\"evento2\":{\"eventName\":\"evento2\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"],\"subprotocol\":\"longpoll\"}],\"actionsTriggered\":[\"azione2\"],\"condition\":\"true\",\"subscription\":{\"sbs2\":{\"type\":\"boolean\",\"value\":\"true\"}},\"cancellation\":{\"cnc2\":{\"type\":\"integer\",\"value\":\"3\"}}}}}";

    // Server requests
    //events
    // server.on(events_endpoint[1].c_str(),HTTP_GET,handleReq10);
    // server.on(events_endpoint[0].c_str(),HTTP_GET,handleReq9);
    // //actions not allowed
    // server.on(req5.c_str(),HTTP_GET,handleReq8);
    // server.on(req6.c_str(),HTTP_GET,handleReq7);
    // //actions allowed
    // server.on(req5.c_str(),HTTP_POST,handleReq6);
    // server.on(req6.c_str(),HTTP_POST,handleReq5);
    // //properties
    // server.on(req4.c_str(),HTTP_GET,handleReq4);
    // server.on(req3.c_str(),HTTP_GET,handleReq3);
    // server.on(req2.c_str(),HTTP_GET,handleReq2);
    // server.on(req1.c_str(),HTTP_GET,handleReq1);

    // server.begin();
    
    hlp = new HTTP_LongPoll(portServer);

    hlp->exposeActions(ae, ah, 2);
    hlp->exposeEvents(events_endpoint, 2);
    hlp->exposeProperties(pe, ph, 4);

    hlp->begin();

    wsb = new WebSocketBinding(portSocket);

    wsb->bindEventSchema(es_doc);
    wsb->exposeActions(ae, ah, 2);
    wsb->exposeEvents(events_endpoint, 2);
    wsb->exposeProperties(pe, ph, 4);

    Serial.println("Server started");
    Serial.println(urlServer);
}    

void loop() {

    // handle Requests
    wsb->loop();
    // delay(1000);
    // wsb->test();
    // Serial.printf("Nel loop, %p %s", ae[0], ae[0].c_str());
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
String handleReq1() {
    String resp = "";

    Serial.println("\nGET Thing URL");
    resp = "[\"" + urlServer + "\"]";
    return resp;
}

String handleReq2() {
    Serial.println("\nGET Thing Description"); 
    return td;
}

String handleReq3() {
    String resp = "";

    resp = request3();
    return resp;
}

String handleReq4() {
    String resp = "";
    
    resp = request4();
    return resp;
}

// void handleReq5(AsyncWebServerRequest *req) {
//     String resp = "";
//     String body = req->arg("plain");
    
//     resp = request5(body);
//     req->send(200, "application/ld+json", resp);
// }
// void handleReq6(AsyncWebServerRequest *req) {
//     String resp = "";
//     String body = req->arg("plain");
    
//     resp = request6(body);
//     req->send(200, "application/ld+json", resp);
// }

// void handleReq7(AsyncWebServerRequest *req) {
//     char* resp = "Method Not Allowed";
//     req->send(405, "text/plain", resp);
// }
// void handleReq8(AsyncWebServerRequest *req) {
//     char* resp = "Method Not Allowed";
//     req->send(405, "text/plain", resp);
// }

// void handleReq9(AsyncWebServerRequest *req) {
//     //hlp->longPollHandler(req, event1_name);
// }

// void handleReq10(AsyncWebServerRequest *req) {
//     //hlp->longPollHandler(req, event2_name);
// }


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
    DynamicJsonDocument resp_doc(2000);
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
        if(resp_doc["stringa"].isNull())
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
                        serializeJson(resp_doc["stringa"], value);
                        action1_input[0] = value;
                        validInput = handleInputType(value,action1_schema[0]);
                    }
                    break;

                }
                i++;
            }    

            if(validInput) {

                action1_input1_value = action1_input[0];

                bool output = azione1(action1_input1_value);    
                resp = (String) output;
                String ws_msg = "azione1 request5 triggered";

                // evento1 condition
                String t_name = "";
                DynamicJsonDocument tmp_doc(82);
                JsonObject tmp_obj = tmp_doc.createNestedObject();

                if(events_dataSchema[1]) {
                    for(i=0; i<es_doc[event1_name]["data"].size(); i++) {
                        t_name = "";
                        serializeJson(es_doc[event1_name]["data"][i]["name"], t_name);
                        t_name.replace("\"", "");
                        tmp_obj[t_name] = es_doc[event1_name]["data"][i]["value"];
                    }
                    serializeJson(tmp_obj, ws_msg);
                }
                if(true) {
                    hlp->sendLongPollTXT(ws_msg, events_endpoint[0]);
                    wsb->sendWebSocketTXT(ws_msg, events_endpoint[0].c_str());
                }
            }
            else
                resp = "InvalidInput";
        }
    }
    return resp;
}
String request6(String body) {
    DynamicJsonDocument resp_doc(400);
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
        if(resp_doc["intero"].isNull() || resp_doc["booleano"].isNull())
            resp = "InvalidInput";
        else {
            bool validInput = true;
            String value = "";

            String action2_input[2] = {};    
            int action2_input1_value = 0;
            bool action2_input2_value = false;

            i = 0;
            while(validInput and i<action2_inputsNumber) {
                switch(i) {
                    case 0: {
                        value = "";
                        serializeJson(resp_doc["intero"], value);
                        action2_input[0] = value;
                        validInput = handleInputType(value,action2_schema[0]);
                    }
                    break;

                    case 1: {
                        value = "";
                        serializeJson(resp_doc["booleano"], value);
                        action2_input[1] = value;
                        validInput = handleInputType(value,action2_schema[1]);
                    }
                    break;

                }
                i++;
            }    

            if(validInput) {

                action2_input1_value = action2_input[0].toInt();
                if(action2_input[1].equals("true")) {
                    action2_input2_value = true;
                }
                else { 
                    action2_input2_value = false;
                }

                String output = azione2(action2_input1_value,action2_input2_value);    
                resp = (String) output;
                String ws_msg = "";

                // evento2 condition
                String t_name = "";
                DynamicJsonDocument tmp_doc(20);
                JsonObject tmp_obj = tmp_doc.createNestedObject();

                if(events_dataSchema[2]) {
                    for(i=0; i<es_doc[event2_name]["data"].size(); i++) {
                        t_name = "";
                        serializeJson(es_doc[event2_name]["data"][i]["name"], t_name);
                        t_name.replace("\"", "");
                        tmp_obj[t_name] = es_doc[event2_name]["data"][i]["value"];
                    }
                    serializeJson(tmp_obj, ws_msg);
                }
                if(true) {
                    hlp->sendLongPollTXT(ws_msg, events_endpoint[1]);
                    wsb->sendWebSocketTXT(output, events_endpoint[1].c_str());
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
	DynamicJsonDocument schema_doc(400);
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
		else if(type.equals("boolean")) {
   
    if(!value.equals("true") && !value.equals("false"))
        validInput = false; 
    }
		else if(type.equals("integer") || type.equals("number")) {
        int dot_count = 0;
        i = 0;
        while(validInput && i<value.length()) {
            if(!isDigit(value[i])) 
                validInput = false;
            else if(value[i] == '.')
                if(i == 0 || i == value.length()-1 || dot_count > 1)
                    validInput = false;
                else 
                    dot_count++;    
            i++;          
        } 
    }
    return validInput;
}

// Action functions
bool azione1(String stringa) {
	return true;
	
}

String azione2(int intero,bool booleano) {
	return "pippofranco";
	
}

// handle Events
/*
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

                                            case 1: {
                                                if(k == j) {
                                                    if(action2_inputsNumber == 0) {
                                                        parsingDone = true;
                                                        obj_ia[ws_requests[i]] = true; 
                                                        resp = request6("{}");
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
            DynamicJsonDocument resp_doc(82);
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
                                DynamicJsonDocument tmp_doc(82);
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
                                DynamicJsonDocument tmp_doc(82);
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

                                            case 1: {
                                                if(k == j) {
                                                    parsingDone = true;
                                                    ia_doc[ip_s][i][ws_actions[j]] = true;
                                                    resp = request6(payload);
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
*/