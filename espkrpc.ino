#include "pb_encode.h"
#include "pb_decode.h"
#include "krpc.pb.h"

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

namespace KRPC {

class pbElement {
  public:
    pbElement() { };
    virtual bool encode(pb_ostream_t *stream, const pb_field_t *field, void * const * arg = NULL) = 0;
};

static bool write_pbElement(pb_ostream_t *stream, const pb_field_t *field, void * const * arg) {
  return ((pbElement*)*arg)->encode(stream, field, NULL);
}

class pbBytesUInt32 : public pbElement {
  public:
    pbBytesUInt32(const uint32_t value) : m_value(value) { };
    bool encode(pb_ostream_t *stream, const pb_field_t *field, void * const * arg = NULL) {
      uint8_t buffer[16];
      pb_ostream_t local_stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
      pb_encode_varint(&local_stream, (uint64_t)m_value);
      return pb_encode_tag_for_field(stream, field) &&
             pb_encode_varint(stream, local_stream.bytes_written) && // length of data
             pb_write(stream, buffer, local_stream.bytes_written);   // data itself
    };
  private:
    const uint32_t m_value;
};

class pbBytesFloat : public pbElement {
  public:
    pbBytesFloat(const float value) : m_value(value) { };
    bool encode(pb_ostream_t *stream, const pb_field_t *field, void * const * arg = NULL) {
      uint8_t buffer[16];
      pb_ostream_t local_stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
      pb_encode_fixed32(&local_stream, (void*)&m_value);
      return pb_encode_tag_for_field(stream, field) &&
             pb_encode_varint(stream, local_stream.bytes_written) && // length of data
             pb_write(stream, buffer, local_stream.bytes_written);   // data itself
    };
  private:
    const float m_value;
};

class Argument : public pbElement {
  public:
    Argument(const uint32_t position = 0, pbElement *value = NULL) : m_position(position), m_value(value) { };
    bool encode(pb_ostream_t *stream, const pb_field_t *field, void * const * arg = NULL) {
      krpc_schema_Argument a = krpc_schema_Argument_init_zero;
      a.position = m_position;
      a.has_position = (m_position > 0);
      a.value.funcs.encode = &write_pbElement;
      a.value.arg = (void*)m_value;
      return pb_encode_tag_for_field(stream, field) && pb_encode_submessage(stream, krpc_schema_Argument_fields, &a);
    };
  private:
    const uint32_t m_position;
    const pbElement *m_value;
};

static bool write_repeated_args(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
  bool b = true;
  for (Argument **args = (Argument **)*arg; *args != NULL; args++) {
    b = b && (*args)->encode(stream, field);
  }
  return b;
}

static bool write_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
  return pb_encode_tag_for_field(stream, field)
         && pb_encode_string(stream, (const pb_byte_t*)*arg, strlen((const char*)*arg));
}

class Request : public pbElement {
  public:
    Request(const char *service, const char *procedure, Argument **arguments = NULL) : m_service(service), m_procedure(procedure), m_arguments(arguments) { };
    bool encode(pb_ostream_t *stream, const pb_field_t *field = NULL, void * const * arg = NULL) {
      krpc_schema_Request message = krpc_schema_Request_init_zero;
      message.service.funcs.encode = &write_string;
      message.service.arg = (void*)m_service;
      message.procedure.funcs.encode = &write_string;
      message.procedure.arg = (void*)m_procedure;
      if (m_arguments != NULL) {
        message.arguments.funcs.encode = &write_repeated_args;
      };
      message.arguments.arg = m_arguments;
      return pb_encode_delimited(stream, krpc_schema_Request_fields, &message);
    };
  private:
    const char *m_service;
    const char *m_procedure;
    Argument **m_arguments;
};

}

void setup() {
  Serial.begin(SERIAL_BITRATE);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); // seems to be required for OTA
  WiFi.begin(ssid, pass);
}

/* This binds the pb_ostream_t into the stdout stream */
// it works, but is very slow - each call (even single byte) goes in separate packet
bool wifiostreamcallback(pb_ostream_t *stream, const uint8_t *buf, size_t count) {
#if 0
  Serial.print("Ostream bytes:[");
  Serial.println(count);
  /* dump to terminal */
  for (unsigned int i = 0; i < count; i++) {
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
#endif
  return client.write((const uint8_t*)buf, count);
}

using namespace std;

bool sendRequest(KRPC::Request &rq)  {
  /* Create a stream that will write to our buffer. */
  pb_ostream_t stream = {&wifiostreamcallback, NULL, SIZE_MAX, 0};

  /* Now we are ready to encode the message! */
  /* Then just check for any errors.. */
  if (!rq.encode(&stream)) {
    Serial.print("Encoding failed:");
    Serial.println(PB_GET_ERROR(&stream));
    return false;
  }
  Serial.print("Encoded ");
  Serial.println(stream.bytes_written);
  delay(0);
  return true;
}

bool getResponse() {
  // wait for response
  while (client.available() == 0) { };
  // dump response
  while (client.available()) {
    Serial.print(client.read(), HEX);
    Serial.print(" ");
  }
  Serial.println();
  // decode response
  return true;
}

void loop() {
  Serial.println("wait 1s");
  delay(1000);
  // reconnect if necessary
  connect();

  /* Prepare message */
  {
    KRPC::Request rq("SpaceCenter", "get_ActiveVessel");
    sendRequest(rq);
    getResponse();
  }

  {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytesUInt32(1)), NULL };
    KRPC::Request rq("SpaceCenter", "Vessel_get_Control", args);
    sendRequest(rq);
    getResponse();
  }

  {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytesUInt32(2)), new KRPC::Argument(1, new KRPC::pbBytesFloat(0.5f)), NULL };
    KRPC::Request rq("SpaceCenter", "Control_set_Throttle", args);
    sendRequest(rq);
    getResponse();
  }
  /*
    response na getactivessel, odpowiedz value=1 (i has_value=true)
    E 9 EA 2B A4 70 3D 27 B4 40 20 1 2A 1 1
    response na getcontrol, odpowiedz value=2 (i has_value=true)
    E 9 22 42 1F 85 EB 5B B6 40 20 1 2A 1 2
    response na ustawienie throttle:
    9 9 EB 62 3D A D7 1C A0 40
    (chyba tylko timestamp, reszta pól domyślnie 0)
    40 9 3B A8 F6 28 DC 2F BA 40 10 1 1A 33 4E 6F 20 73 75 63 68 20 76 65 73 73 65 6C 20 62 62 39 63 33 37 65 35 2D 36 65 32 38 2D 34 65 31 66 2D 62 36 62 35 2D 65 39 31 32 63 37 35 35 36 33 34 34
    jakiś błąd
  */

  // close connection
  Serial.println("shutdown");
  client.stop();
  // put your main code here, to run repeatedly:
  ESP.deepSleep(15 * 60 * 1000000); // 15 minutes
}

