#include "Arduino.h"
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "WebSocketBinding.h"

WebSocketBinding::WebSocketBinding(int portSocket): ia_doc(1000), ipia_doc(2000), e_doc(1000), es_doc(20), ipe_doc(2000), webSocket(portSocket) {    
    // events data
    ipe_arr = ipe_doc.createNestedArray("clients_list");
    ipia_arr = ipia_doc.createNestedArray("clients_list");

    webSocket.begin();
    using namespace std::placeholders;

    webSocket.onEvent([this] (uint8_t num, WStype_t type, uint8_t* pl, size_t length) {
        IPAddress ip;
        String ip_s = "";
        const char* payload = (char *) pl;
        String resp = "";

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
}

void WebSocketBinding::sendWebSocketTXT(String txt, const char* event_name) {
    for(i=0; i<ipe_arr.size(); i++) {
        String ws_ip = ipe_arr[i]["ip"];
        JsonArray ae = e_doc[ws_ip];
        for(j=0; j<ae.size(); j++) {
            if(!ae[j][event_name].isNull() && ae[j][event_name]) {
                unsigned char ws_num = ipe_doc[ws_ip];
                webSocket.sendTXT(ws_num, txt);
            }
        }
    }
}

void WebSocketBinding::webSocketLoop() {
    webSocket.loop();
}

void WebSocketBinding::bindEvents(String ws_endpoints[]) {
    //events_endpoint = &ws_endpoints;
}

void WebSocketBinding::bindEventSchema(DynamicJsonDocument doc) {
    this->es_doc = doc;
}

void WebSocketBinding::exposeProperties(String *properties_endpoint, properties_handler callbacks[]) {
    this->properties_endpoint = properties_endpoint;
    this->properties_cb = callbacks;
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

    
    if(!deleted)
        // remove all client requests
        for(i=0; i<ipia_arr.size(); i++)
            if(ipia_arr[i]["num"] == num) {
                ip_s = "";
                serializeJson(ipia_arr[i]["ip"], ip_s);
                ip_s.replace("\"", "");
                ipia_arr.remove(i);
                ipia_doc.remove(ip_s);
                ia_doc.remove(ip_s);
                break;
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

void WebSocketBinding::_clientConnect(uint8_t num, uint8_t* pl, size_t length) {
    IPAddress ip;
    String ip_s = "";
    const char* payload = (char *) pl;
    String resp = "";
    bool done = false;
    bool conn = false;
    bool isAction = false;
    JsonObject obj_e;
    JsonObject obj_ipe;
    JsonObject obj_ia;
    JsonObject obj_ipia;
    bool parsingDone = false;

    ip = webSocket.remoteIP(num);
    ip_s = ip.toString();
    Serial.printf("\n%d", num);
    Serial.print(" -> Connection request from ");
    Serial.println(ip);
    Serial.printf("Payload: %s\n", payload);

    //FIRST CHECK IF IS CALLED AN EVENT ENDPOINT
    //Foreach endpoint defined
    for(i = 0; !done && i < events_number; i++) {
        //if the endpoint called matches one of the endpoint previously set
        if(events_endpoint[i].equals(payload)) {
            if(e_doc[ip_s].isNull()) {
                obj_e = e_doc.createNestedArray(ip_s).createNestedObject();
                obj_ipe = ipe_arr.createNestedObject();
                obj_ipe["ip"] = ip_s;
                obj_ipe["num"] = num;
                ipe_doc[ip_s] = num;
            }
            else {
                j = 0;
                while(!conn && j<e_doc[ip_s].size()) {
                    if(!e_doc[ip_s][j][events_list[i]].isNull())
                        conn = true;
                    j++;    
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
                webSocket.sendTXT(num, "Connection confirmed");
                if(events_subscriptionSchema[i]) 
                    obj_e[events_list[i]] = false;
                else {
                    obj_e[events_list[i]] = true;  
                    webSocket.sendTXT(num, "Subscription confirmed");  
                }
            }
        }
    }
    
    //PREAMBOLO PER LA CONNESSIONE DI UN NUOVO HOST
    //Se é giá connesso lo avviso e faccio in modo che non venga fatto altro,
    //manipolando il fatto che si é ripresentato di nuovo lo stesso host di prima.
    for(i = 0; !done && i<ws_requestsNumber; i++) {
        if(ws_endpoint[i].equals(payload)) {
            //If I can find the endpoint
            if(ia_doc[ip_s].isNull()) {
                obj_ia = ia_doc.createNestedArray(ip_s).createNestedObject();
                obj_ipia = ipia_arr.createNestedObject();
                obj_ipia["ip"] = ip_s;
                obj_ipia["num"] = num;
                ipia_doc[ip_s] = num;
            }
            else {
                for(j = 0; !conn && j<ia_doc[ip_s].size(); j++) {
                    if(!ia_doc[ip_s][j][ws_requests[i]].isNull())
                        conn = true;
                }
                if(!conn)
                    obj_ia = ia_doc[ip_s].createNestedObject();  
            }
            
            if(conn) {
                done = true;
                webSocket.sendTXT(num, "Connection already established");
            }
        }
    }
    //mine
    //
    for(i = 0; !done && i < actions_number; i++) {
        if (actions_endpoint[i].equals(payload)) {
            done = true;
            obj_ia[actions_endpoint[i]] = true;
            webSocket.sendTXT(num, "Connection confirmed");
            resp = actions_cb[i]("{}");
            webSocket.sendTXT(num, resp);
        }
    }

    for(i = 0; !done && i < properties_number; i++) {
        if (properties_endpoint[i].equals(payload)) {
            done = true;
            obj_ia[properties_endpoint[i]] = true;
            webSocket.sendTXT(num, "Connection confirmed");
            resp = properties_cb[i]();
            webSocket.sendTXT(num, resp);
        }
    }

    //for(i = 0; )
    // verify if the endpoint of this connection request refers to an Action    
    /*for (j = 0; !done && j<ws_actionsNumber; j++) {
        if(ws_requests[i].equals(ws_actions[j])) {
            done = true;
            isAction = true;
            webSocket.sendTXT(num, "Connection confirmed");
            for(k = 0; !parsingDone && k<ws_actionsNumber; k++) {
                switch(k) {
                    case 0: {
                        if(k == j) {
                            if(action1_inputsNumber == 0) {
                                parsingDone = true;
                                obj_ia[ws_requests[i]] = true; 
                                //resp = request5("{}");
                                resp = "plutonzo";
                                webSocket.sendTXT(num, resp);
                            }
                        }
                    }
                    break;

                }
            }
            if(!parsingDone)
                obj_ia[ws_requests[i]] = false;
        }
    }*/
    //If is not an action, so it is a property
    if(!isAction) {
        done = true;
        obj_ia[ws_requests[i]] = true;  
        
        if(ws_requests[i].equals(thingName))
            //resp = request3();
            ;
        else if(ws_requests[i].equals(property1_name))
            //resp = request4();
            ;

        webSocket.sendTXT(num, resp);  
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
    DynamicJsonDocument resp_doc(40);
    bool parsingDone = false;
    int validInput = 0;
    String t_name = "";
    String validate = "";
    String message = "";

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
        String message = "";
        serializeJson(resp_doc, message);
        i = 0;
        while(!parsingDone && i<e_doc[ip_s].size()) {
            j = 0;
            while(!parsingDone && j<events_number) {
                if(!e_doc[ip_s][i][events_list[j]].isNull()) {
                    if(!e_doc[ip_s][i][events_list[j]] && events_subscriptionSchema[j]) {
                        DynamicJsonDocument tmp_doc(40);
                        JsonObject tmp = tmp_doc.createNestedObject();
                        validInput = 0;
                        k = 0;
                        while(!parsingDone && k<es_doc[events_list[j]]["subscription"].size()) {
                            t_name = "";
                            serializeJson(es_doc[events_list[j]]["subscription"][k]["name"], t_name);
                            t_name.replace("\"", "");
                            if(!resp_doc[t_name].isNull()) {
                                tmp[t_name] = es_doc[events_list[j]]["subscription"][k]["value"];
                                validInput++;
                            }
                            else  
                                parsingDone = false;
                            k++;
                        }
                        if(validInput == k) {
                            parsingDone = true;
                            validate = "";
                            serializeJson(tmp, validate);
                            if(validate.equals(message)) {
                                e_doc[ip_s][i][events_list[j]] = true;
                                webSocket.sendTXT(num, "Subscription confirmed");
                            }
                            else 
                                webSocket.sendTXT(num, "Subscription refused");
                        }
                    }
                    else if(e_doc[ip_s][i][events_list[j]] && events_cancellationSchema[j]) {
                        DynamicJsonDocument tmp_doc(40);
                        JsonObject tmp = tmp_doc.createNestedObject();
                        validInput = 0;
                        k = 0;
                        while(!parsingDone && k<es_doc[events_list[j]]["cancellation"].size()) {
                            t_name = "";
                            serializeJson(es_doc[events_list[j]]["cancellation"][k]["name"], t_name);
                            t_name.replace("\"", "");
                            if(!resp_doc[t_name].isNull()) {                                   
                                tmp[t_name] = es_doc[events_list[j]]["cancellation"][k]["value"];
                                validInput++;
                            }
                            else
                                parsingDone = false;
                            k++;
                        }
                        if(validInput == k) {
                            parsingDone = true;
                            validate = "";
                            serializeJson(tmp, validate);
                            if(validate.equals(message)) {
                                e_doc[ip_s][i][events_list[j]] = false;
                                webSocket.sendTXT(num, "Cancellation confirmed");
                            }
                            else
                                webSocket.sendTXT(num, "Cancellation refused");
                        }
                    }  
                }
                j++;
            }
            i++;
        }
        if(!parsingDone) {
            i = 0;
            while(!parsingDone && i<ia_doc[ip_s].size()) {
                j = 0;
                while(!parsingDone && j<ws_actionsNumber) { 
                    if(!ia_doc[ip_s][i][ws_actions[j]].isNull()) {
                        if(!ia_doc[ip_s][i][ws_actions[j]]) {
                            k = 0;
                            while(!parsingDone && k<ws_actionsNumber) {
                                switch(k) {
                                    case 0: {
                                        if(k == j) {
                                            parsingDone = true;
                                            ia_doc[ip_s][i][ws_actions[j]] = true;
                                            //resp = request5(payload);
                                            webSocket.sendTXT(num, resp);
                                        }
                                    }
                                    break;

                                }
                                k++;
                            }
                        }
                    }
                    j++;  
                }        
                i++; 
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
