#include <EthernetUdp.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include <Arduino.h>
#include <coap-simple.h>

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
IPAddress dev_ip/*(192,168,1,106)*/;

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port);

// UDP and CoAP class
WiFiUDP Udp;
Coap coap(Udp);

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Coap Response got]");

  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  Serial.println(p);
}

void connection(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    
    Serial.print("\nConnecting to ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConnected");
    Serial.print("IP address: ");
    dev_ip = WiFi.localIP();
    Serial.println(dev_ip);

}

void setup() {
  Serial.begin(115200);

  connection("Rachelli-net", "3eKLtrdFwfQXgpv!");
  // Serial.print("My IP address: ");
  // Serial.print(Ethernet.localIP());
  // Serial.println();


  // client response callback.
  // this endpoint is single callback.
  Serial.println("Setup Response Callback");
  coap.response(callback_response);

  // start coap server/client
  coap.start();
}
bool check = true;
void loop() {
  // send GET or PUT coap request to CoAP server.
  // To test, use libcoap, microcoap server...etc
  if (check) {
    int msgid = coap.put(IPAddress(192, 168, 1, 106), 5683, "light", "1");
    check = false;
    Serial.println("Check is false");
  }
  else if (!check) {
    int msgid = coap.put(IPAddress(192, 168, 1, 106), 5683, "light", "0");
    check = true;
    Serial.println("Check is true");
  }
//  int msgid = coap.put(IPAddress(192, 168, 1, 106), 5683, "light", c);
  Serial.print("Send Request, led: ");
  Serial.println(check);
  //int msgid = coap.get(IPAddress(192, 168, 1, 3), 5683, "date");

  delay(1000);
  coap.loop();
}
