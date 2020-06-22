//#include <ESP32WebServer.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

const char* ssid = "Rachelli-net";
const char* password = "3eKLtrdFwfQXgpv!";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";
String protocolSocket = "ws";
int portSocket = 81;
String urlSocket = "";

String thingName = "test1";
String td = "";

// document to handle Interaction Affordances WebSocket requests
DynamicJsonDocument ia_doc(1000);
// document to store the ip addresses of clients connected to WebSocket channel for Interaction Affordances requests   
DynamicJsonDocument ipia_doc(2000);
JsonArray ipia_arr;
// document to handle Events WebSocket requests
DynamicJsonDocument e_doc(1000);
// document to handle Events Schemas
DynamicJsonDocument es_doc(150);
// document to store the ip addresses of clients connected to WebSocket channel for Events requests   
DynamicJsonDocument ipe_doc(2000);
JsonArray ipe_arr;
DeserializationError err;

int properties_number = 1;
int objectProperties_number = 0;
int actions_number = 1;
int events_number = 1;

// Properties
const char* property1_name = "proprieta";
int property1_value = 0;


// Actions
const char* action1_name = "azione";
int action1_inputsNumber = 1;
String action1_schema[1] = {"{\"name\":\"in\",\"type\":\"string\"}"};

// Events
const char* event1_name = "evento";
String event1_dataSchema[2] = {"{\"name\":\"datas\",\"value\":\"true\"}","{\"name\":\"datassi\",\"value\":\"pippo\"}"};
bool events_subscriptionSchema[1] = {false};
bool events_dataSchema[1] = {true};
bool events_cancellationSchema[1] = {false};
String events_list[1] = {event1_name};
String events_endpoint[1] = {"/" + thingName + "/events/" + event1_name};

// Requests
String req1 = "/";
String req2 = "/" + thingName;
String req3 = "/" + thingName + "/all/properties";
String req4 = "/" + thingName + "/properties/" + property1_name;
String req5 = "/" + thingName + "/actions/" + action1_name;

int ws_requestsNumber = 3;
int ws_actionsNumber = 1;
String ws_requests[3] = {thingName,property1_name,action1_name};
String ws_endpoint[3] = {req3,req4,req5}; 
String ws_actions[1] = {action1_name};

AsyncWebServer server(portServer);
WebSocketsServer webSocket = WebSocketsServer(portSocket);

IPAddress ipS;


int i, j, k, n;

const int ledPin = 13;
const int pubPin = 14;

void setup() {

    pinMode(ledPin, OUTPUT);
    pinMode(pubPin, INPUT);

    Serial.begin(115200);
    Serial.println();

    // events data
    int schema_size = 0;
    JsonArray arr;
    JsonObject obj;

    schema_size = sizeof(event1_dataSchema) / sizeof(String);
    if(es_doc[event1_name].isNull())
        arr = es_doc.createNestedObject(event1_name).createNestedArray("data");
    else
        arr = es_doc[event1_name].createNestedArray("data");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(150);
        deserializeJson(tmp_doc, event1_dataSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }

    ipe_arr = ipe_doc.createNestedArray("clients_list");
    ipia_arr = ipia_doc.createNestedArray("clients_list");
  
    connection(ssid, password);
    
    td = "{\"title\":\"test1\",\"id\":\"test1\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\"]}],\"properties\":{\"proprieta\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]}],\"type\":\"integer\",\"observable\":false,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{\"azione\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"}],\"input\":{\"in\":{\"type\":\"string\"}},\"output\":{\"type\":\"string\"},\"safe\":false,\"idempotent\":false}},\"events\":{\"evento\":{\"eventName\":\"evento\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]}],\"actionsTriggered\":[\"azione\"],\"condition\":\"true\",\"data\":{\"datas\":{\"type\":\"boolean\",\"value\":\"true\"},\"datassi\":{\"type\":\"string\",\"value\":\"pippo\"}}}}}";

    // Server requests
    server.on("/", HTTP_GET, handleReq1);
    // server.on(req1.c_str(),HTTP_GET,handleReq1);
    server.on("/test1", HTTP_GET, requestHandler);
    // server.on(req3.c_str(),HTTP_GET,handleReq3);
    // server.on(req4.c_str(),HTTP_GET,handleReq4);
    server.on(req5.c_str(),HTTP_POST,handleReq5);
    // server.on(req5.c_str(),HTTP_GET,handleReq6);

    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    Serial.println("Server started");
    Serial.println(urlServer);
}

void requestHandler(AsyncWebServerRequest *req) {
    Serial.println("Opened requestHandler");
    if (req->url().equals(req1)) {
        Serial.println("Handeled request 1");
        handleReq1(req);
    }
    if (req->url().equals(req2)) {
        Serial.println("Handeled request 2");
        handleReq2(req);
    }
    if (req->url().equals(req3)) {
        Serial.println("Handeled request 3");
        handleReq3(req);
    }
    if (req->url().equals(req4)) {
        Serial.println("Handeled request 4");
        handleReq4(req);
    }
    if (req->url().equals(req5)) {
        Serial.println("Handeled request 5");
        handleReq5(req);
    }
    //if (req->url().equals(req6)) handleReq6(req);
}


void loop() {
    //Serial.println(xPortGetCoreID());
    int pubState = digitalRead(pubPin);
    if ( pubState == HIGH ) { 
        digitalWrite(ledPin, HIGH); 
    } else {
        digitalWrite(ledPin, LOW); 
    }

    // handle Requests
    webSocket.loop();
    //server.handleClient();
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

void handleReq2(AsyncWebServerRequest * req) {
    Serial.println("\nGET Thing Description"); 
    req->send(200, "application/ld+json", td);
}

void handleReq3(AsyncWebServerRequest * req) {
    String resp = "";

    resp = request3();
    req->send(200, "application/ld+json", resp);
}

void handleReq4(AsyncWebServerRequest * req) {
    String resp = "";
    
    resp = request4();
    Serial.println("req4 called");

    int pubState;
    while ((pubState = digitalRead(pubPin)) == LOW) {
        delay(90);
    } 

    req->send(200, "application/ld+json", resp);
}

void handleReq5(AsyncWebServerRequest * req) {
    String resp = "";
    String body = req->arg("plain");
    
    resp = request5(body);
    req->send(200, "application/ld+json", resp);
}

void handleReq6(AsyncWebServerRequest * req) {
    char* resp = "Method Not Allowed";
    req->send(405, "text/plain", resp);
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

                // evento condition
                String ws_msg = "pluto";
                String t_name = "";
                DynamicJsonDocument tmp_doc(150);
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
                    Serial.printf("ipe_arr.size() (clients connected on ws) = %d", ipe_arr.size());//num ipaddress 
                    for(i=0; i<ipe_arr.size(); i++) {
                        String ws_ip = ipe_arr[i]["ip"];
                        Serial.println("\t" + ws_ip);
                        JsonArray ae = e_doc[ws_ip];
                        Serial.printf("Dimenzione: %d\n", ae.size());
                        for(j=0; j<ae.size(); j++) {
                            Serial.printf("Dentro al for per la %d volta\n", i);
                            unsigned char ws_num = ipe_doc[ws_ip];
                            String namemm = ae[j][event1_name];
                            Serial.println(ws_num);
                            Serial.println(namemm);
                            if(!ae[j][event1_name].isNull() && ae[j][event1_name]) {
                                Serial.printf("ws_msg ok to send %s\n", ws_msg);
                                webSocket.sendTXT(ws_num, ws_msg);
                            }
                        }
                    }
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
	return "pippo";
	
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
            DynamicJsonDocument resp_doc(150);
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
                                DynamicJsonDocument tmp_doc(150);
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
                                DynamicJsonDocument tmp_doc(150);
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
