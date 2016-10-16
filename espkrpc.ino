#include "pb_encode.h"
#include "pb_decode.h"
#include "krpc.pb.h"

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

#define SERIAL_BITRATE 115200

#define DEFAULT_SERVER "192.168.2.63" //"krpclocal"
#define DEFAULT_RPC_PORT 50000
#define DEFAULT_STREAM_PORT 50001
#define OTA_PORT ((DEFAULT_RPC_PORT)-1)
#define MAX_MSG_LEN 512

//#include "wificonfig-example.h" // this file should be in repository
#include "wificonfig-real.h" // this file should never go to repository 

const char ssid[] = MYSSID;
const char pass[] = MYPASSWORD;

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

// Use WiFiClient class to create TCP connections
WiFiClient client;

IPAddress kRPCServerIP;
const char* kRPCServerName = DEFAULT_SERVER;

const char* kRPCHello = "\x48\x45\x4C\x4C\x4F\x2D\x52\x50\x43\x00\x00\x00";
uint8_t clientid[16];

void connectkRPC() {
  char clientname[32]; // buffer for client name and client id
  memset(clientname, 0, sizeof(clientname));
  snprintf(clientname, sizeof(clientname), "ESP.%ux", system_get_chip_id());
  // open socket (global)
  if (!client.connect(kRPCServerIP, DEFAULT_RPC_PORT)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Sending hello and name");
  // send hello and client name (zero-padded to 32 bytes)
  client.write((const uint8_t*)kRPCHello, sizeof(kRPCHello));
  client.write((const uint8_t*)clientname, sizeof(clientname));
  // wait for response
  while (client.available() == 0);
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
  bool needreconnect = false;
  while (WiFi.status() != WL_CONNECTED) {
    needreconnect = true;
    delay(500);
    Serial.print(".");
  }
  if (needreconnect) {
    // get IP of IoT server
    WiFi.hostByName(kRPCServerName, kRPCServerIP);
    Serial.print("kRPC IP: ");
    Serial.println(kRPCServerIP);
    connectkRPC();
  }
  Serial.print("\nLocal IP: ");
  Serial.println(WiFi.localIP());
}



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
    Request(const char *service, const char *procedure, Argument **arguments) : m_service(service), m_procedure(procedure), m_arguments(arguments) { };
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
      return pb_encode(stream, krpc_schema_Request_fields, &message);
    };
  private:
    const char *m_service;
    const char *m_procedure;
    Argument **m_arguments;
};



void setup() {
  Serial.begin(SERIAL_BITRATE);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); // seems to be required for OTA
  WiFi.begin(ssid, pass);
  connect();


  // OTA setup
  // Port would default to 8266
  ArduinoOTA.setPort(OTA_PORT);
  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");
  // No authentication by default
  ArduinoOTA.setPassword((const char *)MYOTAPASSWORD);
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  //


}

void loop() {
  yield();
  // reconnect if necessary
  connect();
  ArduinoOTA.handle();

  /* Encode our message */
  {
    /* This is the buffer where we will store our message. */
    uint8_t buffer[MAX_MSG_LEN];
    /* Create a stream that will write to our buffer. */
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    /* Prepare message */
    Argument* args[] = { new Argument(0, new pbBytesUInt32(2)), new Argument(1, new pbBytesFloat(0.5f)), NULL };
    Request rq("SpaceCenter", "Control_set_Throttle", args);

    /* Now we are ready to encode the message! */
    /* Then just check for any errors.. */
    if (!rq.encode(&stream))
    {
      Serial.print("Encoding failed:");
      Serial.println(PB_GET_ERROR(&stream));
      //return 1;
    }
    Serial.print("Encoded ");
    Serial.println(stream.bytes_written);

    for (unsigned int i = 0; i < stream.bytes_written; i++) {
      Serial.print(buffer[i], HEX);
    }
    Serial.println();
    // expected output
    Serial.println("0a0b537061636543656e7465721214436f6e74726f6c5f7365745f5468726f74746c651a0508011201021a08080212040000003f");

    // send message - first its length then body
    uint8_t lenbuf[4];
    pb_ostream_t lenstream = pb_ostream_from_buffer(lenbuf, sizeof(lenbuf));
    pb_encode_varint(&lenstream, stream.bytes_written);
    // send length
    client.write((const uint8_t*)lenbuf, lenstream.bytes_written);
    // send body
    client.write((const uint8_t*)buffer, stream.bytes_written);
    yield();
    client.flush();
    yield();
  }

  //return 0;

  // wait for response
  while (client.available() == 0);
  // dump response
  while (client.available()) {
    Serial.print(client.read(), HEX);
  }

  // close connection
  client.stop();
  // put your main code here, to run repeatedly:
  ESP.deepSleep(15 * 60 * 1000000); // 15 minutes
}
