#ifndef HTTP_LongPoll_h
#define HTTP_LongPoll_h
#include "Arduino.h"

#ifndef WOT_HANDLER_FUNC
#define WOT_HANDLER_FUNC
typedef String (*properties_handler)();
typedef String (*actions_handler)(String);
#endif

class HTTP_LongPoll {
    public: 
        /**
         * Constructor. Sets up the object
         * @param serverPort the port which the server will respond
         */
        HTTP_LongPoll(int serverPort);
        
        /**
         * Function that sends the specified text for longpoll HTTP hosts connected to a specified event endpoint
         * @param txt the text to send
         * @param eventName the event endpoint triggered
         **/
        void sendLongPollTXT(String txt, String eventName);

        /**
         * Expose properties via HTTP protocol.
         * It requires an array of endpoints and an array of callback functions of type properties_handler (String fun(void)).
         * When the i endpoint of the array will be called, the response will be given by the i function of the array of callbacks.
         * @param endpoints array of endpoints
         * @param callbacks array of properties_handler callbacks
         * @param prop_num the total number of endpoints/callbacks 
         */
        void exposeProperties(const String *endpoints, properties_handler callbacks[], int prop_num);
        /**
         * Expose actions via HTTP protocol.
         * It requires an array of endpoints and an array of callback functions of type actions_handler (String fun(String)).
         * When the i endpoint of the array will be called, the response will be given by the i function of the array of callbacks.
         * @param endpoints array of endpoints
         * @param callbacks array of actions_handler callbacks
         * @param act_num the total number of endpoints/callbacks 
         */
        void exposeActions(const String *endpoints, actions_handler callbacks[], int act_num);
        /**
         * Expose properties via HTTP protocol.
         * For emitting an event, use sendLongPollTXT() and refer the defined endpoint.
         * @param endpoints array of endpoints
         * @param evt_num the total number of endpoints/callbacks 
         */
        void exposeEvents(const String *endpoints, int evt_num);

        void begin();
    private:
        // Max number of longpoll host that can be managed
        static const int MAX_LONGPOLL_HOSTS = 16;

        /**
         * Handles a new connection of a HTTP Longpoll host, specifying the event 
         * @param req the Async WS request
         * @param eventName the event endpoint name which the host connected
         **/
        void _longPollHandler(AsyncWebServerRequest *req, const char* eventName);

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

        //Server handler
        AsyncWebServer server;
        
        // Array of pending longpoll requests
        AsyncWebServerRequest *longPollRequests[MAX_LONGPOLL_HOSTS];
        // Contains the correspondance between requests and endpoint
        const char* longPollBoundEvents[MAX_LONGPOLL_HOSTS];
};

#include "HTTP_LongPoll.cpp"
#endif