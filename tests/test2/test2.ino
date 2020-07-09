#include "WiFi.h"
#include "ESPAsyncWebServer.h"

const char* ssid = "Rachelli-net";
const char* password = "3eKLtrdFwfQXgpv!";

AsyncWebServer server(80);
const int pubPin = 14;

AsyncWebServerRequest *request1 = NULL;
AsyncWebServerRequest *request2 = NULL;

 
void setup(){
  pinMode(pubPin, INPUT);
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println(WiFi.localIP());
 
  server.on("/test1", HTTP_GET, fun);
  server.on("/test2", HTTP_GET, fun2);
 
  server.begin();
}
 
void loop(){ 
  if (request1) {
    //Serial.printf("The state is %d\n", digitalRead(pubPin));
    if (digitalRead(pubPin) == HIGH) {
      request1->send(200, "text/plain", "accomplished request1");
      request1 = NULL;
    }
  }
  if (request2) {
    request2->send(200, "text/plain", "accomplished request2");
    request2 = NULL;
  }
}

void wait() {

}

void fun(AsyncWebServerRequest *request){
  request1 = request;
  //request->send(200, "text/plain", "Hello!");
}

void fun2(AsyncWebServerRequest *request){
  request2 = request;

  // Serial.printf("Entered in fun2 with url %s\n", request->url());

  // int pubState;
  // while ((pubState = digitalRead(pubPin)) == LOW) {
  //   delay(90);
  // } 
  // if (request->url().equals("/test2/properties/proprieta")) {
  //   request->send(200, "text/plain", "proprieta");
  // } else {
  //   request->send(200, "text/plain", "something else");
  // }
}