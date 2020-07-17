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
bool events_dataSchema[2] = {true,false};

// Requests
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

const String actions_endpoint[2] = {req5, req6};
const String properties_endpoint[4] = {req4, req3, req2, req1};
const String events_endpoint[2] = {"/" + thingName + "/events/" + event1_name,"/" + thingName + "/events/" + event2_name};

String handleReq1();
String handleReq2();
String handleReq3();
String handleReq4();

String request5(String body);
String request6(String body);

properties_handler properties_callback[4] = {handleReq4, handleReq3, handleReq2, handleReq1};
actions_handler actions_callback[2] = {request5, request6};

int i, j, k, n;

void setup() {

    // properties_callback[] = {handleReq4, handleReq3, handleReq2, handleReq1};
    // actions_callback[] = {request5, request6};

    int schema_size = 0;
    JsonArray arr;
    JsonObject obj;

    Serial.begin(115200);
    Serial.println();

    // events data
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

    connection(ssid, password);
    td = "{\"title\":\"test0\",\"id\":\"test0\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\",\"readmultipleproperties\",\"writemultipleproperties\"]}],\"links\":[],\"properties\":{\"proprieta\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/properties/"+property1_name+"\",\"op\":[\"readproperty\",\"writeproperty\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/properties/"+property1_name+"\",\"op\":[\"readproperty\",\"writeproperty\"]}],\"type\":\"integer\",\"observable\":false,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{\"azione1\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"}],\"input\":{\"stringa\":{\"type\":\"string\"}},\"output\":{\"type\":\"boolean\"},\"safe\":false,\"idempotent\":false},\"azione2\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action2_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action2_name+"\",\"op\":\"invokeaction\"}],\"input\":{\"intero\":{\"type\":\"integer\"},\"booleano\":{\"type\":\"boolean\"}},\"output\":{\"type\":\"string\"},\"safe\":false,\"idempotent\":false}},\"events\":{\"evento1\":{\"eventName\":\"evento1\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\",\"unsubscribeevent\"],\"subprotocol\":\"longpoll\"}],\"actionsTriggered\":[\"azione1\"],\"condition\":\"true\",\"subscription\":{\"sbs1\":{\"type\":\"boolean\",\"value\":\"true\"}},\"data\":{\"dataschema1\":{\"type\":\"integer\",\"value\":\"100\"}},\"cancellation\":{\"cnc1\":{\"type\":\"boolean\",\"value\":\"true\"}}},\"evento2\":{\"eventName\":\"evento2\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"]},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/events/"+event2_name+"\",\"op\":[\"subscribeevent\"],\"subprotocol\":\"longpoll\"}],\"actionsTriggered\":[\"azione2\"],\"condition\":\"true\",\"subscription\":{\"sbs2\":{\"type\":\"boolean\",\"value\":\"true\"}},\"cancellation\":{\"cnc2\":{\"type\":\"integer\",\"value\":\"3\"}}}}}";
    
    hlp = new HTTP_LongPoll(portServer);

    hlp->exposeActions(actions_endpoint, actions_callback, 2);
    hlp->exposeEvents(events_endpoint, 2);
    hlp->exposeProperties(properties_endpoint, properties_callback, 4);

    hlp->begin();

    wsb = new WebSocketBinding(portSocket);

    wsb->bindEventSchema(es_doc);
    wsb->exposeActions(actions_endpoint, actions_callback, 2);
    wsb->exposeEvents(events_endpoint, 2);
    wsb->exposeProperties(properties_endpoint, properties_callback, 4);

    Serial.println("Server started");
    Serial.println(urlServer);
}    

void loop() {
    // handle Requests
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
