#include "Arduino.h"
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "WebSocketBinding.h"

WebSocketBinding::WebSocketBinding(int portSocket): ac_doc(2000), ia_doc(1000), ipia_doc(2000), e_doc(1000), es_doc(20), ipe_doc(2000), webSocket(portSocket) {    
    // events data
    ipe_arr = ipe_doc.createNestedArray("clients_list");
    ipia_arr = ipia_doc.createNestedArray("clients_list");
    //handling callback
    webSocket.onEvent([this] (uint8_t num, WStype_t type, uint8_t* pl, size_t length) {
        // this->test();
        switch(type) {
            case WStype_DISCONNECTED: {
                this->_clientDisconnect(num, pl);
                break;
            }
            case WStype_CONNECTED: {
                this->_clientConnect(num, pl, length);
                break;
            }
            case WStype_TEXT:
            { 
                this->_clientText(num, pl, length);
                break;
            }
        }
    });
    //begin websocket
    webSocket.begin();
}

void WebSocketBinding::sendWebSocketTXT(String txt, String event_endpoint) {
    for(i=0; i<ipe_arr.size(); i++) {
        String ws_ip = ipe_arr[i]["ip"];
        JsonArray ae = e_doc[ws_ip];
        Serial.printf("Sending Websocket string %s. Triggered event %s. Sending to ip %s\n", txt.c_str(), event_endpoint.c_str(), ws_ip.c_str());
        for(j=0; j<ae.size(); j++) {
            if(!ae[j][event_endpoint.c_str()].isNull() && ae[j][event_endpoint.c_str()]) {
                unsigned char ws_num = ipe_doc[ws_ip];
                webSocket.sendTXT(ws_num, txt);
                Serial.println("Done");
            }
        }
    }
}

void WebSocketBinding::loop() {
    webSocket.loop();
}

void WebSocketBinding::test() {
    Serial.printf("Nel test %s\n", this->events_endpoint[0].c_str());
}

bool WebSocketBinding::_setEventHandled(String ip_s, int num) {
    bool done = false; 
    bool conn = false;
    JsonObject obj_e;
    JsonObject obj_ipe;
    
    if(e_doc[ip_s].isNull()) {
        obj_e = e_doc.createNestedArray(ip_s).createNestedObject();
        obj_ipe = ipe_arr.createNestedObject();
        obj_ipe["ip"] = ip_s;
        obj_ipe["num"] = num;
        ipe_doc[ip_s] = num;
    }
    else {
        for(j = 0; !conn && j<e_doc[ip_s].size(); j++) {
            if(!e_doc[ip_s][j][events_endpoint[i]].isNull())
                conn = true;    
        }
        if(!conn)
            obj_e = e_doc[ip_s].createNestedObject();  
    }
    
    if(conn) {
        done = true;
        webSocket.sendTXT(num, "Connection already established");
    }
    else {
        done = true;
        webSocket.sendTXT(num, "Connection confirmed - event accomplished");
        if(events_subscriptionSchema[i]) 
            obj_e[events_endpoint[i]] = false;
        else {
            obj_e[events_endpoint[i]] = true;  
            webSocket.sendTXT(num, "Subscription confirmed");  
        }
    }
    return conn;
}

bool WebSocketBinding::_setIAHandled(String ip_s, int num, String endpoint) {
    bool done = false; 
    bool conn = false;
    JsonObject obj_ia;
    JsonObject obj_ipia;
    //PREAMBOLO PER LA CONNESSIONE DI UN NUOVO HOST
    //Se é giá connesso lo avviso e faccio in modo che non venga fatto altro,
    //manipolando il fatto che si é ripresentato di nuovo lo stesso host di prima.
    if(ia_doc[ip_s].isNull()) {
        obj_ia = ia_doc.createNestedArray(ip_s).createNestedObject();
        obj_ia[endpoint] = true;
        obj_ipia = ipia_arr.createNestedObject();
        obj_ipia["ip"] = ip_s;
        obj_ipia["num"] = num;
        ipia_doc[ip_s] = num;
    }
    else {
        for(j = 0; !conn && j<ia_doc[ip_s].size(); j++) {
            if(!ia_doc[ip_s][j][endpoint].isNull())
                conn = true;
        }
        if(!conn)
            obj_ia = ia_doc[ip_s].createNestedObject();  
    }
    
    if(conn) {
        done = true;
        webSocket.sendTXT(num, "Connection already established");
    }
    return done;
}

void WebSocketBinding::_clientConnect(uint8_t num, uint8_t* pl, size_t length) {
    IPAddress ip;
    String ip_s = "";
    const char* payload = (char *) pl;
    String resp = "";
    bool done = false;

    ip = webSocket.remoteIP(num);
    ip_s = ip.toString();
    Serial.printf("\n%d", num);
    Serial.print(" -> Connection request from ");
    Serial.println(ip);
    Serial.printf("Payload: %s\n", payload);

    //FIRST CHECK IF IS CALLED AN EVENT ENDPOINT
    //Foreach event endpoint defined
    for(i = 0; !done && i < events_number; i++) {
        Serial.printf("Siamo negli events, %d %d %s\n", events_number, i, this->events_endpoint[i].c_str());
        //if the endpoint called matches one of the endpoint previously set
        if(events_endpoint[i].equals(payload)) {
            done = this->_setEventHandled(ip_s, num);
            done = true;
        }
    }
    // THEN CHECK IF IS CALLED AN ACTION ENDPOINT
    //Foreach action endpoint defined

    //Serial.printf("Nella callback %s\n", this->properties_endpoint[0].c_str());
    for(i = 0; !done && i < actions_number; i++) {
        Serial.printf("Siamo nelle actions, %d %d %s\n", actions_number, i, this->actions_endpoint[i].c_str());
        //if the endpoint called matches one of the actions endpoint
        if (actions_endpoint[i].equals(payload)) {
            done = this->_setIAHandled(ip_s, num, actions_endpoint[i]);
            done = true;
            // obj_ia[actions_endpoint[i]] = true;
            webSocket.sendTXT(num, "Connection confirmed - action accomplished");
            resp = actions_cb[i]("{}");
            webSocket.sendTXT(num, resp);
        }
    }

    for(i = 0; !done && i < properties_number; i++) {
        Serial.printf("Siamo nelle properties, %d %d %s\n", properties_number, i, this->properties_endpoint[i].c_str());
        if (properties_endpoint[i].equals(payload)) {
            done = this->_setIAHandled(ip_s, num, properties_endpoint[i]);
            done = true;
            // obj_ia[properties_endpoint[i]] = true;
            webSocket.sendTXT(num, "Connection confirmed - property accomplished");
            resp = properties_cb[i]();
            webSocket.sendTXT(num, resp);
        }
    }

    serializeJsonPretty(e_doc, Serial);
    Serial.println();
    serializeJsonPretty(es_doc, Serial);
    Serial.println();
    serializeJsonPretty(ipe_doc, Serial);
    Serial.println();
    serializeJsonPretty(ia_doc, Serial);
    Serial.println();
    serializeJsonPretty(ipia_doc, Serial);
    Serial.println();
}

void WebSocketBinding::bindEventSchema(DynamicJsonDocument doc) {
    int eventsBound = doc.size();
    JsonObject obj = doc.as<JsonObject>();
    //JsonObject::iterator it = obj.begin();
    int i = 0;
    //setter
    this->es_doc = doc;
    //for debug
    // serializeJsonPretty(this->es_doc, Serial);
    this->events_subscriptionSchema[eventsBound];
    this->events_cancellationSchema[eventsBound];

    for (JsonPair kw : obj) {
        // String key = it->key;
        Serial.printf("Subscription for #%d is ", i);
        if (!doc[kw.key()]["subscription"].isNull()) {
            Serial.println("true");
            this->events_subscriptionSchema[i] = true;
        } else {
            Serial.println("false");
            this->events_subscriptionSchema[i] = false;
        }

        Serial.printf("Cancellation for #%d is ", i);
        if (!doc[kw.key()]["cancellation"].isNull()) {
            Serial.println("true");
            this->events_subscriptionSchema[i] = true;
        } else {
            Serial.println("false");
            this->events_subscriptionSchema[i] = false;
        }

        i++;

    }
}

void WebSocketBinding::exposeProperties(const String *endpoints, properties_handler callbacks[], int prop_num) {
    this->properties_endpoint = endpoints;
    this->properties_cb = callbacks;

    this->properties_number = prop_num;
}

void WebSocketBinding::exposeActions(const String *endpoints, actions_handler callbacks[], int act_num) {
    this->actions_endpoint = endpoints;
    this->actions_cb = callbacks;

    this->actions_number = act_num;
}

void WebSocketBinding::exposeEvents(const String *endpoints, int evt_num) {
    this->events_endpoint = endpoints;

    this->events_number = evt_num;
    //Serial.printf("Events endpoint 0 is %s\n", this->events_endpoint[0].c_str());
}

void WebSocketBinding::_clientDisconnect(uint8_t num, uint8_t* pl) {
    IPAddress ip;
    String ip_s = "";
    const char* payload = (char *) pl;
    String resp = "";
    bool deleted = false;
    Serial.printf("\n%d", num);
    Serial.println(" -> Disconnected");

    // remove all client subscriptions
    for(i=0; i<ipe_arr.size(); i++)
        if(ipe_arr[i]["num"] == num) {
            deleted = true;
            serializeJson(ipe_arr[i]["ip"], ip_s);
            ip_s.replace("\"", "");
            ipe_arr.remove(i);
            ipe_doc.remove(ip_s);
            e_doc.remove(ip_s);
            break;
        }

    
    if(!deleted) {
        // remove all client requests
        for(i=0; i<ipia_arr.size(); i++) {
            if(ipia_arr[i]["num"] == num) {
                ip_s = "";
                serializeJson(ipia_arr[i]["ip"], ip_s);
                ip_s.replace("\"", "");
                ipia_arr.remove(i);
                ipia_doc.remove(ip_s);
                ia_doc.remove(ip_s);
                break;
            }
        }
    }


    serializeJsonPretty(e_doc, Serial);
    Serial.println();
    serializeJsonPretty(es_doc, Serial);
    Serial.println();
    serializeJsonPretty(ipe_doc, Serial);
    Serial.println();
    serializeJsonPretty(ia_doc, Serial);
    Serial.println();
    serializeJsonPretty(ipia_doc, Serial);
    Serial.println();
}

void WebSocketBinding::_clientText(uint8_t num, uint8_t* pl, size_t length) {
    
    IPAddress ip;
    String ip_s = "";
    const char* payload = (char *) pl;
    String resp = "";
    DynamicJsonDocument resp_doc(400);
    DeserializationError err;
    bool parsingDone = false;
    int validInput = 0;
    String t_name = "";
    String t_value = "";
    String validate = "";
    String message = "";
    DynamicJsonDocument tmp_doc(400);
    JsonObject tmp = tmp_doc.createNestedObject();

    ip = webSocket.remoteIP(num);
    ip_s = ip.toString();
    Serial.printf("\n%d", num);
    Serial.print(" -> Get Text from ");
    Serial.println(ip);
    Serial.printf("Payload: %s\n", payload);

    err = deserializeJson(resp_doc, payload);
    if(err) {
        Serial.printf("deserializeJson() failed with code %s", err.c_str());
        webSocket.sendTXT(num, "deserializeJson failed");
    }
    else {
        //HERE CHECK IF THE HOST IS SUBSCRIBED TO A CERTAIN EVENT
        //SO YOU HAVE TO REFER TO es_doc, THAT IS THE JSON DOCUMENT CONTAINING SCHEMAS
        // String message = "";
        serializeJson(resp_doc, message);
        Serial.printf("Il message é %s\n", message.c_str());
        //foreach pending request for websocket events
        for(i = 0; !parsingDone && i<e_doc[ip_s].size(); i++) {
            //foreach event
            for(j = 0; !parsingDone && j<events_number; j++) {
                //if the j event is the connected event to the i request
                if(!e_doc[ip_s][i][events_endpoint[j]].isNull()) {
                    //if i'm not already subscribed && the subscription is needed
                    if(!e_doc[ip_s][i][events_endpoint[j]] && events_subscriptionSchema[j]) {
                        validInput = 0;
                        //foreach subscription in es_doc schema
                        for(k = 0; !parsingDone && k<es_doc[events_endpoint[j]]["subscription"].size(); k++) {
                            t_name = "";
                            serializeJson(es_doc[events_endpoint[j]]["subscription"][k]["name"], t_name);
                            t_name.replace("\"", "");
                            serializeJsonPretty(resp_doc, Serial);
                            Serial.printf("Prima c'era resp_doc, mentre t_name é %s\n\tevents_endpoint[j] é %s\n\tk é %d\n", t_name.c_str(), events_endpoint[j].c_str(), k);
                            //if in the text sent via ws there is a key like the one in the subscription schema
                            if(!resp_doc[t_name].isNull()) {
                                //set the value in the schema as the value in the temp object
                                t_value = "";
                                serializeJson(es_doc[events_endpoint[j]]["subscription"][k]["value"], t_value);
                                t_value.replace("\"", "");
                                //tmp["t_name"] = t_value;
                                tmp[t_name] = t_value.c_str();
                                //Serial.printf("\tes_doc[events_endpoint[j]]['subscription'][k]['value'] = %s\n", t_value.c_str());
                                validInput++;
                            }
                            else  
                                parsingDone = false;
                        }
                        //if all inputs scanned are valid
                        if(validInput == k) {
                            parsingDone = true;
                            validate = "";
                            serializeJson(tmp, validate);
                            serializeJsonPretty(tmp, Serial);
                            //Serial.printf("Sembrerebbe che sia stato parsato tutto bene\n");
                            //if the validating json array is like the sent message 
                            if(validate.equals(message)) {
                                e_doc[ip_s][i][events_endpoint[j]] = true;
                                webSocket.sendTXT(num, "Subscription confirmed");
                            }
                            else 
                                webSocket.sendTXT(num, "Subscription refused");
                        }
                    }
                    //else if i'm already subscribed and cancellationschema is needed
                    else if(e_doc[ip_s][i][events_endpoint[j]] && events_cancellationSchema[j]) {
                        // DynamicJsonDocument tmp_doc(40);
                        // JsonObject tmp = tmp_doc.createNestedObject();
                        validInput = 0;
                        for(k = 0; !parsingDone && k<es_doc[events_endpoint[j]]["cancellation"].size(); k++) {
                            t_name = "";
                            serializeJson(es_doc[events_endpoint[j]]["cancellation"][k]["name"], t_name);
                            t_name.replace("\"", "");
                            if(!resp_doc[t_name].isNull()) {  
                                t_value = "";
                                serializeJson(es_doc[events_endpoint[j]]["cancellation"][k]["value"], t_value);
                                tmp[t_name] = t_value;
                                validInput++;
                            }
                            else
                                parsingDone = false;
                        }
                        if(validInput == k) {
                            parsingDone = true;
                            validate = "";
                            serializeJson(tmp, validate);
                            if(validate.equals(message)) {
                                e_doc[ip_s][i][events_endpoint[j]] = false;
                                webSocket.sendTXT(num, "Cancellation confirmed");
                            }
                            else
                                webSocket.sendTXT(num, "Cancellation refused");
                        }
                    }  
                }
            }
        }
        //If has not matched for subscription or cancellations
        if(!parsingDone) {
            //foreach element in interaction afforcance array (so if it is an handled action or an handled property)
            for(i = 0; !parsingDone && i < ia_doc[ip_s].size(); i++) {
                //foreach action
                for(j = 0; !parsingDone && j < actions_number; j++) {
                    Serial.printf("Sono arrivato alle IA, j=%d\n", j);
                    //if there is an action handled via websocket
                    if(!ia_doc[ip_s][i][actions_endpoint[j]].isNull() && ia_doc[ip_s][i][actions_endpoint[j]]) {
                        //if() {
                            resp = "";
                            parsingDone = true;
                            ia_doc[ip_s][i][actions_endpoint[j]] = true;
                            //resp = request5(payload);
                            resp = actions_cb[j](payload);
                            webSocket.sendTXT(num, resp);
                            /*
                            for(k = 0; !parsingDone && k < actions_number; k++) {
                                switch(k) {
                                    case 0: {
                                        if(k == j) {
                                            parsingDone = true;
                                            ia_doc[ip_s][i][actions_endpoint[j]] = true;
                                            //resp = request5(payload);
                                            webSocket.sendTXT(num, resp);
                                        }
                                    }
                                    break;

                                }
                            }*/
                        //}
                    }
                }        
            }
        }
        
        if(!parsingDone) 
            webSocket.sendTXT(num, "Invalid message");
    }

    serializeJsonPretty(e_doc, Serial);
    Serial.println();
    serializeJsonPretty(es_doc, Serial);
    Serial.println();
    serializeJsonPretty(ipe_doc, Serial);
    Serial.println();
    serializeJsonPretty(ia_doc, Serial);
    Serial.println();
    serializeJsonPretty(ipia_doc, Serial);
    Serial.println();
    
}
