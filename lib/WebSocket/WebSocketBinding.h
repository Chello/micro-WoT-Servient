#ifndef WebSocketBinding_h
#define WebSocketBinding_h
#include "Arduino.h"
#include <WebSocketsServer.h>

class WebSocketBinding {
    public:
        WebSocketBinding(int portSocket);//: ia_doc(1000), ipia_doc(2000), e_doc(1000), es_doc(20), ipe_doc(2000);
        void webSocketEvent(uint8_t num, WStype_t type, uint8_t* pl, size_t length);
        void sendWebSocketTXT(String txt, const char* event_name);
        void webSocketLoop();

        void bindEvents(String ws_endpoints[]);

    private:
        void _clientDisconnect(uint8_t num, uint8_t* pl);
        void _clientConnect(uint8_t num, uint8_t* pl, size_t length);
        void _clientText(uint8_t num, uint8_t* pl, size_t length);
        // document to handle Interaction Affordances WebSocket requests
        DynamicJsonDocument ia_doc;
        // document to store the ip addresses of clients connected to WebSocket channel for Interaction Affordances requests   
        DynamicJsonDocument ipia_doc;
        // document to handle Events WebSocket requests
        DynamicJsonDocument e_doc;
        // document to handle Events Schemas
        DynamicJsonDocument es_doc;
        // document to store the ip addresses of clients connected to WebSocket channel for Events requests   
        DynamicJsonDocument ipe_doc;

        JsonArray ipia_arr;
        JsonArray ipe_arr;

        WebSocketsServer webSocket;

        const String thingName = "test4";

        // Requests
        const String req5 = "/" + thingName + "/actions/" + action1_name;
        const String req4 = "/" + thingName + "/properties/" + property1_name;
        const String req3 = "/" + thingName + "/all/properties";
        const String req2 = "/" + thingName;
        const String req1 = "/";

        const bool events_subscriptionSchema[2] = {false,false};
        const bool events_dataSchema[2] = {false,false};
        const bool events_cancellationSchema[2] = {false,false};

        String events_list[2];

        DeserializationError err;
        // Properties
        const char* property1_name = "proprieta";
        const bool property1_value = false;

        // Actions
        const char* action1_name = "azione";
        const int action1_inputsNumber = 1;

        // Events
        const char* event1_name = "evento";
        const char* event2_name = "evento2";

        const String ws_requests[3] = {thingName,property1_name,action1_name};
        const String ws_endpoint[3] = {req3,req4,req5}; 
        const String ws_actions[1] = {action1_name};

        const int properties_number = 1;
        const int actions_number = 1;
        const int events_number = 2;

        const int objectProperties_number = 0;
        const int ws_actionsNumber = 1;

        const int ws_requestsNumber = 3;

        String events_endpoint[2];

        int i, j, k, n;

};

#include "WebSocketBinding.cpp"
#endif