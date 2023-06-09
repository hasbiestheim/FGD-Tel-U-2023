//************************************************************
// this is a simple example that uses the painlessMesh library to
// connect to a another network and broadcast message from a webpage to the edges of the mesh network.
// This sketch can be extended further using all the abilities of the AsyncWebserver library (WS, events, ...)
// for more details
// https://gitlab.com/painlessMesh/painlessMesh/wikis/bridge-between-mesh-and-another-network
// for more details about my version
// https://gitlab.com/Assassynv__V/painlessMesh
// and for more details about the AsyncWebserver library
// https://github.com/me-no-dev/ESPAsyncWebServer
//************************************************************

#include "IPAddress.h"
#include "painlessMesh.h"
#include <Arduino_JSON.h>

#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
#else
#include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define   MESH_PREFIX     "Mesh@ESP"
#define   MESH_PASSWORD   "12345678"
#define   MESH_PORT       5555

#define   STATION_SSID     "ESPMesh"
#define   STATION_PASSWORD "12345678"

#define HOSTNAME "ESP-Mesh Network"

// Prototype
void receivedCallback(const uint32_t &from, const String &msg);
String readTemperature(const uint32_t &from, const String &msg);
String readHeartRate(const uint32_t &from, const String &msg);
String readPulseOximeter(const uint32_t &from, const String &msg);
IPAddress getlocalIP();

painlessMesh  mesh;
AsyncWebServer server(80);
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

void receivedCallback(const uint32_t &from, const String &msg) {
  //Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  double heartRate = myObject["heartRate"];
  double pulseOximeter = myObject["pulseOximeter"];
  double temperature = myObject["temperature"];
  Serial.print("Node : ");
  Serial.println(node);
  // server.send(200, "text/html", index)
  Serial.print("Heart Rate : ");
  Serial.print(heartRate);
  Serial.println(" bpm");
  Serial.print("Pulse Oximeter : ");
  Serial.print(pulseOximeter);
  Serial.println(" %");
  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" C");
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}

String readHeartRate(const uint32_t &from, const String &msg){
  JSONVar myObject = JSON.parse(msg.c_str());
  double heartRate = myObject["heartRate"];
  return String(heartRate);
}

String readPulseOximeter(const uint32_t &from, const String &msg){
  JSONVar myObject = JSON.parse(msg.c_str());
  double pulseOximeter = myObject["pulseOximeter"];   
  return String(pulseOximeter);
}

String readTemperature(const uint32_t &from, const String &msg){
  JSONVar myObject = JSON.parse(msg.c_str());
  double temperature = myObject["temperature"];
  return String(temperature);
}

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 11 );
  mesh.onReceive(&receivedCallback);

  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);

  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());

// Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/heartrate", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readHeartRate());
  });
  server.on("/pulseoximeter", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readPulseOximeter());
  });
  server.on("/pulseoximeter", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readTemperature());
  });

  // Start server
  server.begin();

}

void loop() {
  mesh.update();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
}