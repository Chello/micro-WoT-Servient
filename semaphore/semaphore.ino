
#include <ArduinoJson.h>
#include <embeddedWoT_HTTP_LongPoll.h>
#include <embeddedWoT_WebSocket.h>
#include <HTTPClient.h>

const char *ssid = "Rachelli-net";
const char *password = "3eKLtrdFwfQXgpv!";
String protocolServer = "http";
int portServer = 80;
String urlServer = "";
String protocolSocket = "ws";
int portSocket = 81;
String urlSocket = "";

String thingName = "semaphore";
String td = "";

DynamicJsonDocument es_doc(20);
// Json Array to store the ip addresses of clients connected to WebSocket channel for Events requests
JsonArray ipe_arr;
DeserializationError err;

int properties_number = 1;
int objectProperties_number = 0;
int actions_number = 1;
int events_number = 1;

// Properties
const char *property0_name = "semaphore";
bool property0_value = false;

// Actions
const char *action1_name = "statusChanged";
int action1_inputsNumber = 1;
String action1_schema[1] = {"{\"name\":\"resp\",\"type\":\"string\"}"};

// Events
const char *event1_name = "hasParkChanged";
bool events_subscriptionSchema[1] = {false};
bool events_dataSchema[1] = {false};
bool events_cancellationSchema[1] = {false};

// Endpoints
String req6 = "/" + thingName + "/events/" + event1_name;
String req5 = "/" + thingName + "/actions/" + action1_name;
String req4 = "/" + thingName + "/properties/" + property0_name;
String req3 = "/" + thingName + "/all/properties";
String req2 = "/" + thingName;
String req1 = "/";

IPAddress ipS;
//Longpoll object handler
embeddedWoT_HTTP_LongPoll *hlp;
//WebSocket object handler
embeddedWoT_WebSocket *wsb;

const int GREENLED = 12;
const int REDLED = 33;
const char *HTTPUrl = "http://192.168.1.106/bike-rack/events/hasParkChanged";
int i, j, k, n;

String request1();
String request2();
String request3();
String request4();
String request5(String body);

//HTTP - actions
const int http_actions_num = 1;
const String http_actions_endpoint[http_actions_num] = {req5};
actions_handler http_actions_callback[http_actions_num] = {request5};

//WS - actions
const int ws_actions_num = 0;
const String ws_actions_endpoint[ws_actions_num] = {};
actions_handler ws_actions_callback[ws_actions_num] = {};

//HTTP - Properties
const int http_properties_num = 4;
const String http_properties_endpoint[http_properties_num] = {req1, req2, req3, req4};
properties_handler http_properties_callback[http_properties_num] = {request1, request2, request3, request4};

//WS - Properties
const int ws_properties_num = 4;
const String ws_properties_endpoint[ws_properties_num] = {req1, req2, req3, req4};
properties_handler ws_properties_callback[ws_properties_num] = {request1, request2, request3, request4};

//HTTP - events
const int http_events_num = 1;
const String http_events_endpoint[http_events_num] = {req6};
//WS - events
const int ws_events_num = 1;
const String ws_events_endpoint[ws_events_num] = {req6};

void setup()
{
    Serial.begin(115200);
    Serial.println();

    // events data

    connection(ssid, password);

    td = "{\"title\":\"semaphore\",\"id\":\"semaphore\",\"@context\":[\"https://www.w3.org/2019/wot/td/v1\"],\"security\":\"nosec_sc\",\"securityDefinitions\":{\"nosec_sc\":{\"scheme\":\"nosec\"}},\"forms\":[{\"contentType\":\"application/json\",\"href\":\"" + urlServer + "/all/properties\",\"op\":[\"readallproperties\",\"readmultipleproperties\"]},{\"contentType\":\"application/json\",\"href\":\"" + urlSocket + "/all/properties\",\"op\":[\"readallproperties\",\"readmultipleproperties\"]}],\"links\":[],\"properties\":{\"semaphore\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\"" + urlServer + "/properties/" + property0_name + "\",\"op\":[\"readproperty\"]},{\"contentType\":\"application/json\",\"href\":\"" + urlSocket + "/properties/" + property0_name + "\",\"op\":[\"readproperty\"]}],\"type\":\"boolean\",\"items\":{\"type\":\"boolean\"},\"observable\":true,\"readOnly\":true,\"writeOnly\":true}},\"actions\":{\"statusChanged\":{\"forms\":[{\"contentType\":\"application/json\",\"href\":\"" + urlServer + "/actions/" + action1_name + "\",\"op\":\"invokeaction\"}],\"input\":{\"resp\":{\"type\":\"string\"}},\"safe\":true,\"idempotent\":false}},\"events\":{\"hasParkChanged\":{\"eventName\":\"hasParkChanged\",\"forms\":[{\"contentType\":\"application/json\",\"href\":\"" + urlServer + "/events/" + event1_name + "\",\"subprotocol\":\"longpoll\",\"op\":[]},{\"contentType\":\"application/json\",\"href\":\"" + urlSocket + "/events/" + event1_name + "\",\"op\":[\"subscribeevent\"]}],\"actionsTriggered\":[\"statusChanged\"],\"condition\":\"true\"}}}";

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

    // property0_value[0] = false;

    // property0_value[1] = false;

    // property0_value[2] = false;

    // This statement will declare pin 22 as digital output
    pinMode(GREENLED, OUTPUT);

    pinMode(REDLED, OUTPUT);

    // This statement will declare pin 15 as digital input
    // pinMode(SENSOR0, INPUT);

    // pinMode(SENSOR1, INPUT);
}

void loop()
{
    HTTPClient http;

    http.begin(HTTPUrl);

    int httpResponseCode = http.GET();

    Serial.println("Speriamo1");

    if (httpResponseCode > 0)
    {

        String payload = http.getString();
        http.end();

        DynamicJsonDocument doc(100);

        Serial.println("Speriamo2");

        Serial.println(payload);

        deserializeJson(doc, payload);

        // extract the values
        JsonObject obj = doc.as<JsonObject>();

        JsonArray array = obj["parks"];

        bool tot = true;

        for (JsonVariant v : array)
        {

            Serial.println("Speriamo3-4");

            bool vas = v.as<bool>();

            tot = tot && vas;
        }

        Serial.println("Speriamo5");

        if (tot)
        {

            Serial.println("Rosso");

            digitalWrite(GREENLED, LOW);

            digitalWrite(REDLED, HIGH);
        }
        else
        {

            Serial.println("Verde");

            digitalWrite(GREENLED, HIGH);

            digitalWrite(REDLED, LOW);
        }

        Serial.println("Speriamo6");
        doc.clear();
    }

    Serial.println("Speriamo7");

    // http.end();

    Serial.println("Speriamo8");

    // handle Requests via WebSocket
    // wsb->loop();
}

void connection(const char *ssid, const char *password)
{
    WiFi.begin(ssid, password);

    Serial.print("\nConnecting to ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED)
    {
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
String request1()
{
    String resp = "";

    Serial.println("\nGET Thing URL");
    resp = "[\"" + urlServer + "\"]";
    return resp;
}

String request2()
{
    Serial.println("\nGET Thing Description");
    return td;
}

String request3()
{
    DynamicJsonDocument tmp(2220);
    String resp = "";
    JsonObject obj = tmp.createNestedObject();

    Serial.println("\nGET all properties");
    obj[property0_name] = property0_value;
    serializeJson(obj, resp);

    return resp;
}

String request4()
{
    String resp = "";

    Serial.printf("\nGET %s value\n", property0_name);
    resp = "{\"" + (String)property0_name + "\":" + property0_value + "}";

    return resp;
}

String request5(String body)
{
    DynamicJsonDocument resp_doc(200);
    String resp = "";

    Serial.printf("\nPOST invokeaction %s\n", action1_name);
    Serial.printf("Body received: %s\n", body.c_str());

    err = deserializeJson(resp_doc, body);
    if (err)
    {
        Serial.printf("deserializeJson() failed with code %s", err.c_str());
        resp = err.c_str();
        return resp;
    }
    else
    {
        if (resp_doc["resp"].isNull())
            resp = "InvalidInput";
        else
        {
            bool validInput = true;
            String value = "";

            String action1_input[1] = {};
            String action1_input1_value = "";

            i = 0;
            while (validInput and i < action1_inputsNumber)
            {
                switch (i)
                {
                case 0:
                {
                    value = "";
                    serializeJson(resp_doc["resp"], value);
                    action1_input[0] = value;
                    validInput = handleInputType(value, action1_schema[0]);
                }
                break;
                }
                i++;
            }

            if (validInput)
            {

                action1_input1_value = action1_input[0];

                statusChanged(action1_input1_value);
                resp = "";
                String ws_msg = "";

                // hasParkChanged condition
                if (true)
                {
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
bool handleInputType(String value, String schema)
{
    DynamicJsonDocument schema_doc(200);
    bool validInput = true;

    deserializeJson(schema_doc, schema);
    JsonObject obj_schema = schema_doc.as<JsonObject>();
    String type = obj_schema["type"];

    if (value[0] == '"')
        value.remove(0);
    if (value[value.length() - 1] == '"')
        value.remove(value.length() - 1);

    if (type.equals("string"))
    {
        if (value.equalsIgnoreCase("null"))
            validInput = false;
    }
    return validInput;
}

void emitEvent(String txt, String event_endpoint)
{
    String endpoint = "/" + thingName + "/events/" + event_endpoint;
    hlp->sendLongPollTXT(txt, endpoint);
    wsb->sendWebSocketTXT(txt, endpoint);
}

// Action functions
void statusChanged(String resp)
{
    return;
}
