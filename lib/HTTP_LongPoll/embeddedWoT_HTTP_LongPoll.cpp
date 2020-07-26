#include "Arduino.h"
#include <ESPAsyncWebServer.h>
#include "embeddedWoT_HTTP_LongPoll.h"

embeddedWoT_HTTP_LongPoll::embeddedWoT_HTTP_LongPoll(int serverPort) : server(serverPort) {
    //Setup Longpoll hosts list
    for (int i = 0; i < MAX_LONGPOLL_HOSTS; i++){
        longPollRequests[i] = NULL;
    }
}

void embeddedWoT_HTTP_LongPoll::_longPollHandler(AsyncWebServerRequest *req, const char* eventName) {
    int i = 0;
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

void embeddedWoT_HTTP_LongPoll::sendLongPollTXT(String txt, String eventName) {
    int i;
    Serial.printf("Sending text %s to broadcast LongPoll event %s\n", txt.c_str(), eventName.c_str());
    for(i = 0; i < MAX_LONGPOLL_HOSTS; i++) {
        // Serial.printf("\tCurrently analyzing mem endpoint %s\n", longPollBoundEvents[i] == NULL ? "NULL" : longPollBoundEvents[i]);
        if (longPollRequests[i] != NULL && eventName.equals(longPollBoundEvents[i])) {
            Serial.printf("Handling request from IP %s for event %s, ", 
                            longPollRequests[i]->client()->remoteIP().toString().c_str(),
                            eventName.c_str());
            longPollRequests[i]->send(200, "application/ld+json", txt/*longPollResponse[i]*/);
            Serial.printf("request satisfied.\n");
            break;
        }
    }
}

void embeddedWoT_HTTP_LongPoll::exposeProperties(const String *endpoints, properties_handler callbacks[], int prop_num) {
    int i = 0;
    this->properties_endpoint = endpoints;
    this->properties_cb = callbacks;

    this->properties_number = prop_num;

    for(; i < prop_num; i++) {
        Serial.printf("Ok defined %s\n", endpoints[i].c_str());
        this->server.on(endpoints[i].c_str(), HTTP_GET, [this, i] (AsyncWebServerRequest *req) {
            Serial.printf("Ok called %s\n", req->url().c_str());
            String toSend = this->properties_cb[i]();
            req->send(200, "application/ld+json", toSend);
        });
    }
}

void embeddedWoT_HTTP_LongPoll::exposeActions(const String *endpoints, actions_handler callbacks[], int act_num) {
    int i = 0;
    this->actions_endpoint = endpoints;
    this->actions_cb = callbacks;

    this->actions_number = act_num;

    for(; i < act_num; i++) {
        //For HTTP_POST
        this->server.on(endpoints[i].c_str(), HTTP_POST, [this, i] (AsyncWebServerRequest *req) {
            String body = req->arg("plain");
            String toSend = this->actions_cb[i](body);
            req->send(200, "application/ld+json", toSend);
        });

        //For HTTP_GET method not allowed
        this->server.on(endpoints[i].c_str(), HTTP_GET, [this] (AsyncWebServerRequest *req) {
            String toSend = "Method Not Allowed";
            req->send(405, "text/plain", toSend);
        });
    }
}

void embeddedWoT_HTTP_LongPoll::exposeEvents(const String *endpoints, int evt_num) {
    int i = 0;
    this->events_endpoint = endpoints;

    this->events_number = evt_num;

    for(; i < evt_num; i++) {
        this->server.on(endpoints[i].c_str(), HTTP_GET, [this, i] (AsyncWebServerRequest *req) {
            this->_longPollHandler(req, this->events_endpoint[i].c_str());
        });
    }
}

void embeddedWoT_HTTP_LongPoll::begin() {
    this->server.begin();
}