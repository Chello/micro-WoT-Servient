#include <ESP32WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char* ssid = "mmmmmm";
const char* password =
String protocolServer = "http";
int portServer = 80;
String urlServer = "";
String protocolSocket = "ws";
int portSocket = 80;
String urlSocket = "";

String thingName = "cosa";
String td = "";

// document to handle Interaction Affordances WebSocket requests
DynamicJsonDocument ia_doc(0);
// document to store the ip addresses of clients connected to WebSocket channel for Interaction Affordances requests   
DynamicJsonDocument ipia_doc(2000);
JsonArray ipia_arr;
DeserializationError err;

int properties_number = 0;
int objectProperties_number = 0;
int actions_number = 0;
int events_number = 0;




// Requests
String req1 = "/";
String req2 = "/" + thingName;
String req3 = "/" + thingName + "/all/properties";

int ws_requestsNumber = 1;
int ws_actionsNumber = 0;
String ws_requests[1] = {thingName};
String ws_endpoint[1] = {req3}; 

ESP32WebServer server(portServer);
WebSocketsServer webSocket = WebSocketsServer(portSocket);

IPAddress ipS;


int i, j, k, n;

void setup() {
    Serial.begin(115200);
    Serial.println();
    ipia_arr = ipia_doc.createNestedArray("clients_list");
  
    connection(ssid, password);
    
    td = "{\"title\":\"cosa\",\"id\":\"cosa\",\"@context\":\"https://www.w3.org/2019/wot/td/v1\",\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"href\":\""+urlServer+"/all/properties\",\"contentType\":\"application/json\",\"op\":[\"readallproperties\",\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]},{\"href\":\""+urlSocket+"/all/properties\",\"contentType\":\"application/json\",\"op\":[\"readallproperties\",\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]}],\"@type\":[\"meta/type1\",\"meta/type2\"],\"description\":\"Thing description here\",\"version\":\"1.2.3\",\"modified\":\"05-26-2020T19:13:47\",\"support\":\"support.uri\",\"base\":\"base.uri\",\"links\":[{\"href\":\"link1href\",\"type\":\"link1media/type\",\"rel\":\"link1relation-type\",\"anchor\":\"link1anchor\"},{\"href\":\"link2href\"}]}";

    // Server requests
    server.on(req1,HTTP_GET,handleReq1);
    server.on(req2,HTTP_GET,handleReq2);
    server.on(req3,HTTP_GET,handleReq3);

    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    Serial.println("Server started");
    Serial.println(urlServer);
}    

void loop() {

    // handle Requests
    webSocket.loop();
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
    urlSocket = protocolSocket + "://" + ipS.toString() + ":" + portSocket + "/" + thingName;
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
                        done = true;
                        obj_ia[ws_requests[i]] = true;  
                        
                        if(ws_requests[i].equals(thingName))
                            resp = request3();

                        webSocket.sendTXT(num, resp);  
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

        }
        break;
    }
}
