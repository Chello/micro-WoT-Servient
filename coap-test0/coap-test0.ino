
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

String thingName = "coap-test0";
String td = "";


// document to handle Events Schemas in WebSocket
DynamicJsonDocument es_doc(206);
// Json Array to store the ip addresses of clients connected to WebSocket channel for Events requests   
JsonArray ipe_arr;
DeserializationError err;

int properties_number = 1;
int objectProperties_number = 0;
int actions_number = 1;
int events_number = 1;

// Properties
const char* property1_name = "prop";
bool property1_value = false;


// Actions
const char* action1_name = "act1";
int action1_inputsNumber = 1;
String action1_schema[1] = {"{\"name\":\"in\",\"type\":\"string\"}"};

// Events
const char* event1_name = "evt";
String event1_subscriptionSchema[1] = {"{\"name\":\"sbs\",\"value\":\"true\"}"};
String event1_dataSchema[1] = {"{\"name\":\"dt\",\"value\":\"false\"}"};
String event1_cancellationSchema[1] = {"{\"name\":\"cnc\",\"value\":\"false\"}"};
bool events_subscriptionSchema[1] = {true};
bool events_dataSchema[1] = {true};
bool events_cancellationSchema[1] = {true};

// Endpoints
String req6 = "/" + thingName + "/events/" + event1_name;
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
//CoAP object handler
embeddedWoT_CoAP *coap;

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
const int ws_actions_num = 1;
const String ws_actions_endpoint[ws_actions_num] = { req5 };
actions_handler ws_actions_callback[ws_actions_num] = { request5 };

//HTTP - Properties
const int http_properties_num = 4;
const String http_properties_endpoint[http_properties_num] = { req1, req2, req3, req4 };
properties_handler http_properties_callback[http_properties_num] = { request1, request2, request3, request4 };

//WS - Properties
const int ws_properties_num = 4;
const String ws_properties_endpoint[ws_properties_num] = { req1, req2, req3, req4 };
properties_handler ws_properties_callback[ws_properties_num] = { request1, request2, request3, request4 };

//HTTP - events
const int http_events_num = 1;
const String http_events_endpoint[http_events_num] = { req6 };
//WS - events
const int ws_events_num = 1;
const String ws_events_endpoint[ws_events_num] = { req5 };

void setup() {
    Serial.begin(115200);
    Serial.println();

    // events data
    int schema_size = 0;
    JsonArray arr;
    JsonObject obj;

    schema_size = sizeof(event1_subscriptionSchema) / sizeof(String);
    if(es_doc[event1_name].isNull())
        arr = es_doc.createNestedObject(event1_name).createNestedArray("subscription");
    else
        arr = es_doc[event1_name].createNestedArray("subscription");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(70);
        deserializeJson(tmp_doc, event1_subscriptionSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }

    schema_size = sizeof(event1_dataSchema) / sizeof(String);
    if(es_doc[event1_name].isNull())
        arr = es_doc.createNestedObject(event1_name).createNestedArray("data");
    else
        arr = es_doc[event1_name].createNestedArray("data");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(70);
        deserializeJson(tmp_doc, event1_dataSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }

    schema_size = sizeof(event1_cancellationSchema) / sizeof(String);
    if(es_doc[event1_name].isNull())
        arr = es_doc.createNestedObject(event1_name).createNestedArray("cancellation");
    else
        arr = es_doc[event1_name].createNestedArray("cancellation");
    for(i=0; i<schema_size; i++) {
        DynamicJsonDocument tmp_doc(70);
        deserializeJson(tmp_doc, event1_cancellationSchema[i]);
        JsonObject obj = arr.createNestedObject();
        obj["name"] = tmp_doc["name"];
        obj["value"] = tmp_doc["value"];
    }
  
    connection(ssid, password);
    
    td = "{\"title\":\"coap-test0\",\"id\":\"coap-test0\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/all/properties\",\"op\":[\"readallproperties\",\"writeallproperties\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/all/properties\",\"op\":[\"readmultipleproperties\",\"writemultipleproperties\"]}],\"links\":[],\"properties\":{\"prop\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/properties/"+property1_name+"\",\"op\":[\"readproperty\"]},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/properties/"+property1_name+"\",\"op\":[\"readproperty\",\"writeproperty\"]}],\"type\":\"boolean\",\"observable\":false,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{\"act1\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"},{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/actions/"+action1_name+"\",\"op\":\"invokeaction\"}],\"input\":{\"in\":{\"type\":\"string\"}},\"output\":{\"type\":\"string\"},\"safe\":false,\"idempotent\":false}},\"events\":{\"evt\":{\"eventName\":\"evt\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\""+urlSocket+"/events/"+event1_name+"\",\"subprotocol\":\"longpoll\",\"op\":[]},{\"contentType\":\"application/json\",\"href\":\""+urlServer+"/events/"+event1_name+"\",\"op\":[\"subscribeevent\"],\"subprotocol\":\"longpoll\"}],\"actionsTriggered\":[\"act1\"],\"condition\":\"true\",\"subscription\":{\"sbs\":{\"type\":\"boolean\",\"value\":\"true\"}},\"data\":{\"dt\":{\"type\":\"boolean\",\"value\":\"false\"}},\"cancellation\":{\"cnc\":{\"type\":\"boolean\",\"value\":\"false\"}}}}}";

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

                String output = act1(action1_input1_value);    
                resp = (String) output;
                String ws_msg = "";

                // evt condition
                String t_name = "";
                DynamicJsonDocument tmp_doc(68);
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
                    hlp->sendLongPollTXT(ws_msg, http_events_endpoint[0]);
                    wsb->sendWebSocketTXT(ws_msg, ws_events_endpoint[0]);
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
String act1(String in) {
	return "act1 sent " + in;
	
}

