#include "pb_encode.h"
#include "pb_decode.h"
#include "krpc.pb.h"

using namespace std;

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

// decoding part

bool read_print_string(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  size_t len = stream->bytes_left;
  Serial.print("string length is ");
  Serial.println(len);

  if (len > (MAX_MSG_LEN - 1)) {
    len = MAX_MSG_LEN - 1;
  }

  unsigned char buf[len + 1]; // gcc extension syntax
  memset(buf,0,len);
  if (!pb_read(stream, buf, len)) {
    return false;
  }

  Serial.print("[");
  Serial.print((const char*)buf);
  Serial.println("]");
  return true;
}

bool read_varint(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  uint64_t value;
  if (!pb_decode_varint(stream, &value)) {
    return false;
  }
  *((uint64_t*)*arg) = value;
  return true;
}

// floats are also encoded as fixed32
bool read_fixed32(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  uint32_t value;
  if (!pb_decode_fixed32(stream, &value)) {
    return false;
  }
  *((uint32_t*)*arg) = value;
  return true;
}


class Response {
  public:
    Response(pb_istream_t *stream) : m_stream(stream) {
      resp = krpc_schema_Response_init_zero;
    };
    uint64_t decode_uint64() {
      uint64_t val;
      resp.return_value.funcs.decode = &read_varint;
      resp.return_value.arg = &val;
      if (v_decode()) {
        return val;
      }
      return -1;
    };
    uint32_t decode_uint32() {
      uint32_t val;
      resp.return_value.funcs.decode = &read_fixed32;
      resp.return_value.arg = &val;
      if (v_decode()) {
        return val;
      }
      return -1;
    };
    float decode_float() {
      uint32_t val = v_decode();
      if (resp.has_return_value) {
        return (float)val;
      } else {
        return -1.0;
      }
    };
    krpc_schema_Response resp;
  private:
    bool v_decode() {
      resp.error.funcs.decode = &read_print_string;
      return pb_decode_delimited(m_stream, krpc_schema_Response_fields, &resp);
    };
    pb_istream_t *m_stream;
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

using namespace std;

bool sendRequest(KRPC::Request &rq)  {
  /* Create a stream that will write to our buffer. */
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  /* Now we are ready to encode the message! */
  /* Then just check for any errors.. */
  if (!rq.encode(&stream)) {
    Serial.print("Encoding failed:");
    Serial.println(PB_GET_ERROR(&stream));
    return false;
  }
  Serial.print("Encoded ");
  Serial.println(stream.bytes_written);
  /* dump to terminal */
  for (unsigned int i = 0; i < stream.bytes_written; i++) {
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
  // send message - first its length then body
  Serial.println(client.write((const uint8_t*)buffer, stream.bytes_written));
  delay(0);
  return true;
}

bool wifiistreamcallback(pb_istream_t *stream, uint8_t *buf, size_t count) {
  return (client.read(buf, count) == count);
}

uint32_t getIntResponse() {
  // wait for response to come
  while (client.available() == 0) { };
  // decode response
  pb_istream_t stream = {&wifiistreamcallback, NULL, SIZE_MAX};
  KRPC::Response R(&stream);
  uint32_t val = R.decode_uint64();
  Serial.print("v=");
  Serial.print(val);
  Serial.print("\thas_err=");
  Serial.print(R.resp.has_error);
  Serial.print("\thas_val=");
  Serial.print(R.resp.has_return_value);
  Serial.print("\ttime=");
  Serial.println(R.resp.time);
  return val;
}

void loop() {
  Serial.println("wait 1s");
  delay(1000);
  // reconnect if necessary
  connect();

  uint32_t active_vessel;
  uint32_t vessel_control;
  /* Prepare message */
  {
    KRPC::Request rq("SpaceCenter", "get_ActiveVessel");
    sendRequest(rq);
    active_vessel = getIntResponse();
  }

  {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytesUInt32(active_vessel)), NULL };
    KRPC::Request rq("SpaceCenter", "Vessel_get_Control", args);
    sendRequest(rq);
    vessel_control = getIntResponse();
  }

  {
    KRPC::Argument* args[] = { new KRPC::Argument(0, new KRPC::pbBytesUInt32(vessel_control)), new KRPC::Argument(1, new KRPC::pbBytesFloat(0.5f)), NULL };
    KRPC::Request rq("SpaceCenter", "Control_set_Throttle", args);
    sendRequest(rq);
    getIntResponse();
  }
  /*
    response na getactivessel, odpowiedz value=1 (i has_value=true)
    E 9 EA 2B A4 70 3D 27 B4 40 20 1 2A 1 1
    response na getcontrol, odpowiedz value=2 (i has_value=true)
    E 9 22 42 1F 85 EB 5B B6 40 20 1 2A 1 2
    response na ustawienie throttle:
    9 9 EB 62 3D A D7 1C A0 40
    (chyba tylko timestamp, reszta pól domyślnie 0)
  */

  // close connection
  Serial.println("shutdown");
  client.stop();
  // put your main code here, to run repeatedly:
  ESP.deepSleep(15 * 60 * 1000000); // 15 minutes
}

