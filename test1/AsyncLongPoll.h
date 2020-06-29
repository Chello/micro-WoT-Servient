#ifndef AsyncLongPoll_h
#define AsyncLongPoll_h
#include "Arduino.h"

class AsyncLongPoll {
    public: 
        AsyncLongPoll();
        /**
         * Handles a new connection of a HTTP Longpoll host, specifying the event
         * @param req the Async WS request
         * @param eventName the event name
         **/
        void longPollHandler(AsyncWebServerRequest *req, const char* eventName);
        /**
         * Function that sends the specified text for longpoll HTTP hosts connected to a specified event
         * @param txt the text to send
         * @param eventName the event triggered
         **/
        void sendLongPollTXT(String txt, const char* eventName);
    private:
        static const int MAX_LONGPOLL_HOSTS = 16;
        AsyncWebServerRequest *longPollRequests[MAX_LONGPOLL_HOSTS];
        const char* longPollBoundEvents[MAX_LONGPOLL_HOSTS];
};

#endif