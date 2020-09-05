
#include <ArduinoJson.h>
#include <embeddedWoT_HTTP_LongPoll.h>
#include <embeddedWoT_WebSocket.h>

const char* ssid = "Rachelli-net";
const char* password = "3eKLtrdFwfQXgpv!";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";
String protocolSocket = "ws";
int portSocket = 81;
String urlSocket = "";

String thingName = "bike-rack_tmp";
String td = "";


DynamicJsonDocument es_doc(20);
// Json Array to store the ip addresses of clients connected to WebSocket channel for Events requests   
JsonArray ipe_arr;
DeserializationError err;

int properties_number = 1;
int objectProperties_number = 0;
int actions_number = 1;
int events_number = 0;

// Properties
const char* property1_name = "parks";
DynamicJsonDocument property1_jdoc(300);
JsonArray property1_value = property1_jdoc.to<JsonArray>();


// Actions
const char* action1_name = "isParkFree";
int action1_inputsNumber = 1;
String action1_schema[1] = {"{\"name\":\"rack_num\",\"type\":\"integer\"}"};


// Endpoints
String req5 = "/" + thingName + "/actions/" + action1_name;
String req4 = "/" + thingName + "/properties/" + property1_name;
String req3 = "/" + thingName + "/all/properties";
String req2 = "/" + thingName;
String req1 = "/";

IPAddress ipS;
//Longpoll object handler
embeddedWoT_HTTP_LongPoll *hlp;
//WebSocket object handler
embeddedWoT_WebSocket *wsb;

int i, j, k, n;

String request1();
String request2();
String request3();
String request4();
String request5(String body);

//HTTP - actions
const int http_actions_num = 1;
const String http_actions_endpoint[http_actions_num] = { req5 };
actions_handler http_actions_callback[http_actions_num] = { request5 };

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
    
    td = "{\"title\":\"bike-rack_tmp\",\"id\":\"bike-rack\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"readallproperties\",\"readmultipleproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"readallproperties\",\"readmultipleproperties\"]}],\"links\":[],\"properties\":{\"parks\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]}],\"type\":\"array\",\"items\":{\"type\":\"boolean\"},\"observable\":false,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{\"isParkFree\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"}],\"input\":{\"rack_num\":{\"type\":\"integer\"}},\"output\":{\"type\":\"boolean\"},\"safe\":true,\"idempotent\":false}},\"events\":{}}";

    hlp = new embeddedWoT_HTTP_LongPoll(portServer);

    hlp->exposeActions(http_actions_endpoint, http_actions_callback, http_actions_num);
    hlp->exposeEvents(http_events_endpoint, http_events_num);
    hlp->exposeProperties(http_properties_endpoint, http_properties_callback, http_properties_num);

    hlp->begin();
    wsb = new embeddedWoT_WebSocket(portSocket);

    wsb->bindEventSchema(es_doc);
    wsb->exposeActions(ws_actions_endpoint, ws_actions_callback, ws_actions_num);
    wsb->exposeEvents(ws_events_endpoint, ws_events_num);
    wsb->exposeProperties(ws_properties_endpoint, ws_properties_callback, ws_properties_num);
    Serial.println("Server started");
    Serial.println(urlServer);

    property1_value[0] = false;
	
property1_value[1] = false;
	
property1_value[2] = false;
	
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
    String tmp = "";

    Serial.printf("\nGET %s value\n", property1_name);
    serializeJson(property1_value, tmp);
    resp = "{\"" + (String) property1_name + "\":" + tmp + "}";
    
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
        if(resp_doc["rack_num"].isNull())
            resp = "InvalidInput";
        else {
            bool validInput = true;
            String value = "";

            String action1_input[1] = {};    
            int action1_input1_value = 0;

            i = 0;
            while(validInput and i<action1_inputsNumber) {
                switch(i) {
                    case 0: {
                        value = "";
                        serializeJson(resp_doc["rack_num"], value);
                        action1_input[0] = value;
                        validInput = handleInputType(value,action1_schema[0]);
                    }
                    break;

                }
                i++;
            }    

            if(validInput) {

                action1_input1_value = action1_input[0].toInt();

                bool output = isParkFree(action1_input1_value);    
                resp = (String) output;
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
    
		if(type.equals("integer") || type.equals("number")) {
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
bool isParkFree(int rack_num) {
	return property1_value[rack_num].as<bool>();
	
}

