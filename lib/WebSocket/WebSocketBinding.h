#ifndef WebSocketBinding_h
#define WebSocketBinding_h
#include "Arduino.h"
//#include <WebSocketsServer.h>

typedef String (*properties_handler)();
typedef String (*actions_handler)(String);

class WebSocketBinding {
    public:
        WebSocketBinding(int portSocket);//: ia_doc(1000), ipia_doc(2000), e_doc(1000), es_doc(20), ipe_doc(2000);
        void webSocketEvent(uint8_t num, WStype_t type, uint8_t* pl, size_t length);
        void sendWebSocketTXT(String txt, const char* event_name);
        void webSocketLoop();

        void test();

        //void bindEvents(String ws_endpoints[]);//may be useless

        void bindEventSchema(DynamicJsonDocument doc);

        void exposeProperties(const String *endpoints, properties_handler callbacks[], int prop_num);
        void exposeActions(const String *endpoints, actions_handler callbacks[], int act_num);
        void exposeEvents(const String *endpoints, int evt_num);

    private:
        void _clientDisconnect(uint8_t num, uint8_t* pl);
        void _clientConnect(uint8_t num, uint8_t* pl, size_t length);
        void _clientText(uint8_t num, uint8_t* pl, size_t length);
        bool _setEventHandled(String ip_s, int num);
        bool _setIAHandled(String ip_s, int num, String endpoint);
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
        // document to handle Actions WebSocket requests
        DynamicJsonDocument ac_doc;

        JsonArray ipia_arr;
        JsonArray ipe_arr;

        WebSocketsServer webSocket;

        // const String thingName = "test0";

        /*
        // Requests
        const String req5 = "/" + thingName + "/actions/" + action1_name;
        const String req4 = "/" + thingName + "/properties/" + property1_name;
        const String req3 = "/" + thingName + "/all/properties";
        const String req2 = "/" + thingName;
        const String req1 = "/";
        */

        bool events_subscriptionSchema[];// = {false,false};
        //*bool events_dataSchema[2] = {false,false};
        bool events_cancellationSchema[];// = {false,false};

        // DeserializationError err;
        /*
        // Properties
        const char* property1_name = "proprieta";
        const bool property1_value = false;
        */

        /*       
        // Actions
        const char* action1_name = "azione";
        const int action1_inputsNumber = 1;
        */

        // Events
        // const char* event1_name = "evento";
        // const char* event2_name = "evento2";

        //String events_list[2] = {event1_name,event2_name};
        //String events_endpoint[2] = {"/" + thingName + "/events/" + event1_name,"/" + thingName + "/events/" + event2_name};

        //mine
        const String* events_endpoint;
        const String* actions_endpoint;
        const String* properties_endpoint;//[/*2*/];// = {req3, req4};

        actions_handler *actions_cb;
        properties_handler *properties_cb;

        int properties_number;
        int actions_number;
        int events_number;
        //endmine

        //const String ws_requests[3] = {thingName,property1_name,action1_name};
        //const String ws_endpoint[3] = {req3,req4,req5}; 
        //const String ws_actions[1] = {action1_name};
        //const String ws_properties[2] = {thingName, property1_name};

        //const int events_number = 2;

        // const int objectProperties_number = 0;
        // const int ws_actionsNumber = 1;

        // const int ws_requestsNumber = 3;


        int i, j, k, n;

};

#include "WebSocketBinding.cpp"
#endif