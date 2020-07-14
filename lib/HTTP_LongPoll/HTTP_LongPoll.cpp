#include "Arduino.h"
#include <ESPAsyncWebServer.h>
#include "HTTP_LongPoll.h"

HTTP_LongPoll::HTTP_LongPoll() {
    //Setup Longpoll hosts list
    for (int i = 0; i < MAX_LONGPOLL_HOSTS; i++){
        longPollRequests[i] = NULL;
    }
}

void HTTP_LongPoll::longPollHandler(AsyncWebServerRequest *req, const char* eventName) {
    int i = 0;
    bool found = false;
    for(; i < MAX_LONGPOLL_HOSTS; i++){
        if (longPollRequests[i] == NULL) {
            longPollRequests[i] = req;
            longPollBoundEvents[i] = eventName;
            break;
        }
    }
    //Serial.printf("Differenza tra puntatore %p e stringa %s.", &eventName, eventName);
    Serial.printf("Adding new host at position #%d\n", i);

    req->onDisconnect([req, this](){
        int i = 0;
        // Serial.printf("Client disconnected, %p\n", req);
        for(; i < MAX_LONGPOLL_HOSTS; i++) {
            if (req == this->longPollRequests[i]) {
                Serial.printf("Longpoll Host #%d disconnected\n", i);
                this->longPollRequests[i] = NULL;
                this->longPollBoundEvents[i] = NULL;
                break;
            }
        }
    });
}

void HTTP_LongPoll::sendLongPollTXT(String txt, const char* eventName) {
    int i;
    for(i = 0; i < MAX_LONGPOLL_HOSTS; i++) {
        if (longPollRequests[i] != NULL && longPollBoundEvents[i] == eventName) {
            Serial.printf("Handling request from IP %s for event %s, ", 
                            longPollRequests[i]->client()->remoteIP().toString().c_str(),
                            eventName);
            longPollRequests[i]->send(200, "application/ld+json", txt/*longPollResponse[i]*/);
            Serial.printf("request satisfied.\n");
        }
    }
}