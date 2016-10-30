
// Arduino IDE settings: WeMos D1 R2 & mini, 160 MHz, 921600, 4M (1M SPIFFS)

//  tcpdump -Xvvv -n -s 1000 -i wlan0 src host 192.168.2.54 and port 50000 and tcp
//  tcpdump -Xvvv -n -s 1000 -i any host 192.168.2.168 and port 50000 and tcp

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

#define SERIAL_BITRATE 115200

#define DEFAULT_SERVER "zenit" //"krpclocal"
#define DEFAULT_RPC_PORT 50000
#define DEFAULT_STREAM_PORT 50001
#define OTA_PORT ((DEFAULT_RPC_PORT)-1)
#define MAX_MSG_LEN 512

/* This is the buffer where we will store our messages to encode/send, receive/decode. */
/* this is global to avoid new/malloc and RAM fragmentation */
uint8_t buffer[MAX_MSG_LEN];

//#include "wificonfig-example.h" // this file should be in repository
#include "wificonfig-real.h" // this file should never go to repository 

const char ssid[] = MYSSID;
const char pass[] = MYPASSWORD;

#include <ESP8266WiFi.h>

// Use WiFiClient class to create TCP connections
WiFiClient client;

// krpc-nanopb wrappers
#include "krpc-nbp.h"
// SpaceCenter service
#include "spacecenter.h"

const char kRPCServerName[] = DEFAULT_SERVER;
const char kRPCHello[] = "\x48\x45\x4C\x4C\x4F\x2D\x52\x50\x43\x00\x00\x00";
uint8_t clientid[16];

void connectkRPC() {
  char clientname[32]; // buffer for client name and client id
  memset(clientname, 0, sizeof(clientname));
  snprintf(clientname, sizeof(clientname), "ESP.%ux", system_get_chip_id());

  Serial.print("connecting to kRPC IP as: ");
  Serial.println(clientname);

  // open socket (global)
  if (!client.connect(kRPCServerName, DEFAULT_RPC_PORT)) {
    Serial.println("connection failed");
    return;
  }
  client.setNoDelay(true);
  delay(0);
  Serial.print("Connected:");
  Serial.println(client.connected());
  Serial.print("Remote ip:");
  Serial.println(client.remoteIP());
  Serial.print("Remote port:");
  Serial.println(client.remotePort());
  // send hello and client name (zero-padded to 32 bytes)
  Serial.println("Sending hello");
  Serial.println(client.connected());
  Serial.println(client.write((const uint8_t*)kRPCHello, sizeof(kRPCHello) - 1)); // -1 because without terminating 0
  delay(0);
  Serial.println(client.write((const uint8_t*)clientname, sizeof(clientname)));
  delay(0);
  // wait for response
  while (client.available() == 0) { };
  // receive client id
  client.readBytes(clientid, sizeof(clientid));
  // success
  Serial.print("Connected! ID=");
  for (unsigned int i = 0; i < sizeof(clientid); i++) {
    Serial.print(clientid[i], HEX);
  }
  Serial.println();
}

void connect() {

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if (!client.connected()) {
    connectkRPC();
  }
}

void setup() {
  Serial.begin(SERIAL_BITRATE);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); // seems to be required for OTA
  WiFi.begin(ssid, pass);
}

void loop() {
  Serial.println("wait 1s");
  delay(1000);
  // reconnect if necessary
  connect();

  uint32 hcount=0;

  // Part One: Preparing for Launch
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  KRPC::services::Vessel vessel = KRPC::services::SpaceCenter.active_vessel();
  KRPC::services::Control control = vessel.control();
  KRPC::services::Flight flight = vessel.flight();
  KRPC::services::Resources resources = vessel.resources();
  KRPC::services::AutoPilot autopilot = vessel.auto_pilot();
  KRPC::services::Orbit orbit = vessel.orbit();

Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  control.throttle(1.0f);
  delay(500);
  control.throttle(0.0f);
  for (int i = 5; i >= 0; i--) {
    Serial.println(i);
    delay(1000);
  }

Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  autopilot.set_target_pitch_and_heading(90.0f, 90.0f);
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  autopilot.engage();
  // vessel.auto_pilot().engage(); // this will call autopilot destructor
  control.throttle(1.0f);
  delay(1000);

  // Part Two: Lift-off!
  Serial.println("Launch!");
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  control.activate_next_stage();

  float fuel = 1;
  while (fuel > 0.1) {
    Serial.print("Fuel:");
    fuel = resources.amount("LiquidFuel");
    Serial.print("\t");
    Serial.print(fuel);
    fuel = resources.amount("SolidFuel");
    Serial.print("\t");
    Serial.println(fuel);
    delay(1000);
  }
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  Serial.println("Booster separation");
  control.activate_next_stage();
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());

  // Part Three: Reaching Apoapsis
  double alt = 0;
  while (alt < 10000) {
    //delay(1000); how fast we can go? about 20rpc/s
    Serial.print("Alt:");
    alt = flight.mean_altitude();
    Serial.println(alt);
  }
  Serial.println("Gravity turn");
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  autopilot.set_target_pitch_and_heading(60.0f, 90.0f);

  alt = 0;
  while (alt < 100000) {
    delay(1000);
    Serial.print("ApoAlt:");
    alt = orbit.apoapsis_altitude();
    Serial.println(alt);
  }
  //
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  control.throttle(0.0f);
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  delay(1000);
  control.activate_next_stage();
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());
  autopilot.disengage();
Serial.printf("heap size %i: %u\n", hcount++, ESP.getFreeHeap());

  // Part Four: Returning Safely to Kerbin
  alt = 100000;
  while (alt > 1000) {
    if (alt > 2000) {
      delay(1000);  // sample with lower frequency until we are close to the ground
    }
    Serial.print("SurfAlt:");
    alt = flight.surface_altitude();
    Serial.println(alt);
  }
  control.activate_next_stage();

  // close connection
  Serial.println("shutdown");
  client.stop();
  // put your main code here, to run repeatedly:
  ESP.deepSleep(15 * 60 * 1000000); // 15 minutes
}
