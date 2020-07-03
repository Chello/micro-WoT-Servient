#ifndef AsyncLongPoll_h
#define AsyncLongPoll_h
#include "Arduino.h"

class AsyncLongPoll {
    public: 
        /**
         * Constructor. Sets up the object
         */
        AsyncLongPoll();
        /**
         * Handles a new connection of a HTTP Longpoll host, specifying the event 
         * @param req the Async WS request
         * @param eventName the event endpoint name which the host connected
         **/
        void longPollHandler(AsyncWebServerRequest *req, const char* eventName);
        /**
         * Function that sends the specified text for longpoll HTTP hosts connected to a specified event endpoint
         * @param txt the text to send
         * @param eventName the event endpoint triggered
         **/
        void sendLongPollTXT(String txt, const char* eventName);
    private:
        /**
         * Max number of longpoll host that can be managed
         */
        static const int MAX_LONGPOLL_HOSTS = 16;
        /**
         * Array of pending longpoll requests
         */
        AsyncWebServerRequest *longPollRequests[MAX_LONGPOLL_HOSTS];
        /**
         * Contains the correspondance between requests and endpoint
         */
        const char* longPollBoundEvents[MAX_LONGPOLL_HOSTS];
};

#include "AsyncLongPoll.cpp"
#endif