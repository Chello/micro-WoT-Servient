#ifndef WebSocketBinding_h
#define WebSocketBinding_h
#include "Arduino.h"
#include <WebSocketsServer.h>

#ifndef WOT_HANDLER_FUNC
#define WOT_HANDLER_FUNC
typedef String (*properties_handler)();
typedef String (*actions_handler)(String);
#endif

/**
 * Class for managing WebSocket requests for WoT Embedded devices.
 */
class WebSocketBinding {
    public:
        /**
         * Constructor. Requires WebSocket port
         * @param portSocket the port of the WebSocket
         */
        WebSocketBinding(int portSocket);//: ia_doc(1000), ipia_doc(2000), e_doc(1000), es_doc(20), ipe_doc(2000);

        /**
         * Sends a WebSocket message for a certain event. 
         * It will send the same message to all hosts subscribed in the specified endpoint.
         * @param txt the text to send
         * @param event_endpoint the endpoint to send message to
         */
        void sendWebSocketTXT(String txt, String event_endpoint);

        /**
         * Loops the webSocket for accepting requests.
         */
        void loop();

        void test();

        /**
         * Binds the event schema for subscriptions and/or events_cancellations.
         * The document should have the following form:
         * {"/test0/events/event1": {
         *      "subscription": [
         *          {
         *          "name": "sbs1",
         *          "value": "true"
         *          }
         *      ],
         *      "data": [
         *          {
         *          "name": "dataschema1",
         *          "value": "100"
         *          }
         *      ],
         *      "cancellation": [
         *          {
         *          "name": "cnc1",
         *          "value": "true"
         *          }
         *      ]
         *  }
         * 
         * @param doc the document containing the events schema.
         */
        void bindEventSchema(DynamicJsonDocument doc);

        /**
         * Expose properties via WebSocket protocol.
         * It requires an array of endpoints and an array of callback functions of type properties_handler (String fun(void)).
         * When the i endpoint of the array will be called, the response will be given by the i function of the array of callbacks.
         * @param endpoints array of endpoints
         * @param callbacks array of properties_handler callbacks
         * @param prop_num the total number of endpoints/callbacks 
         */
        void exposeProperties(const String *endpoints, properties_handler callbacks[], int prop_num);
        /**
         * Expose actions via WebSocket protocol.
         * It requires an array of endpoints and an array of callback functions of type actions_handler (String fun(String)).
         * When the i endpoint of the array will be called, the response will be given by the i function of the array of callbacks.
         * @param endpoints array of endpoints
         * @param callbacks array of actions_handler callbacks
         * @param act_num the total number of endpoints/callbacks 
         */
        void exposeActions(const String *endpoints, actions_handler callbacks[], int act_num);
        /**
         * Expose properties via WebSocket protocol.
         * For emitting an event, use sendWebSocketTXT() and refer the defined endpoint.
         * @param endpoints array of endpoints
         * @param evt_num the total number of endpoints/callbacks 
         */
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
        // the array inside ipia_doc
        JsonArray ipia_arr;
        // document to handle Events WebSocket requests
        DynamicJsonDocument e_doc;
        // document to handle Events Schemas
        DynamicJsonDocument es_doc;
        // document to store the ip addresses of clients connected to WebSocket channel for Events requests   
        DynamicJsonDocument ipe_doc;
        // the array inside ipe_doc
        JsonArray ipe_arr;
        // document to handle Actions WebSocket requests
        DynamicJsonDocument ac_doc;
        // websocket connection handler
        WebSocketsServer webSocket;

        //on i position contains true if i event requires subscription
        bool events_subscriptionSchema[];
        //on i position contains true if i event requires cancellation
        bool events_cancellationSchema[];

        //contains the event endpoints
        const String* events_endpoint;
        //contains the actions endpoints
        const String* actions_endpoint;
        //contains the properties endpoints
        const String* properties_endpoint;

        //contains the action callbacks
        actions_handler *actions_cb;
        //contains the properties callbacks
        properties_handler *properties_cb;

        //Number of properties exposed
        int properties_number;
        //Number of actions exposed
        int actions_number;
        //Number of events exposed
        int events_number;
        
        int i, j, k, n;

};

#include "WebSocketBinding.cpp"
#endif