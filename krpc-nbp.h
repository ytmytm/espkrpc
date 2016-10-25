#include "pb_encode.h"
#include "pb_decode.h"
#include "krpc.pb.h"

using namespace std;

namespace KRPC {

class pbElement {
  public:
    pbElement() { };
    virtual bool encode(pb_ostream_t *stream, const pb_field_t *field, void * const * arg = NULL) = 0;
};

static bool write_pbElement(pb_ostream_t *stream, const pb_field_t *field, void * const * arg) {
  return ((pbElement*)*arg)->encode(stream, field, NULL);
}

enum pbBytes_t { pbvarint, pbfloat, pbstring };

class pbBytes : public pbElement {
  public:
    pbBytes(const uint32_t value) : m_type(pbvarint) {
      m_value.m_uint32 = value;
    };
    pbBytes(const float value) : m_type(pbfloat) {
      m_value.m_float = value;
    };
    pbBytes(char *value) : m_type(pbstring) {
      m_value.m_char = value;
    };
    bool encode(pb_ostream_t *stream, const pb_field_t *field, void * const * arg = NULL) {
      size_t bufsize = 6;
      if (m_type==pbstring) bufsize += strlen((const char*)m_value.m_char);
      uint8_t buffer[bufsize];
      pb_ostream_t local_stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
      switch (m_type) {
        case pbvarint:
          pb_encode_varint(&local_stream, (uint64_t)m_value.m_uint32);
          break;
        case pbfloat:
          pb_encode_fixed32(&local_stream, (void*)&m_value.m_float);
          break;
        case pbstring:
          pb_encode_string(&local_stream, (const pb_byte_t*)m_value.m_char, strlen((const char*)m_value.m_char));
          break;
      };
      return pb_encode_tag_for_field(stream, field) &&
             pb_encode_varint(stream, local_stream.bytes_written) && // length of data
             pb_write(stream, buffer, local_stream.bytes_written);   // data itself
    };
  private:
    union {
      uint32_t m_uint32;
      float m_float;
      char *m_char;
    } m_value;
    const pbBytes_t m_type;
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

static bool write_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
  return pb_encode_tag_for_field(stream, field)
         && pb_encode_string(stream, (const pb_byte_t*)*arg, strlen((const char*)*arg));
}

// decoding part

bool read_print_string(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  size_t len = stream->bytes_left;
  unsigned char buf[2];

  Serial.print("string length is ");
  Serial.println(len);
  Serial.print("[");
  memset(buf, 0, sizeof(buf));
  for (unsigned i = 0; i < len; i++) {
    if (!pb_read(stream, buf, 1)) {
      return false;
    }
    Serial.print((const char*)buf);
  }
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

bool read_fixed64(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  uint64_t value;
  if (!pb_decode_fixed64(stream, &value)) {
    return false;
  }
  *((uint64_t*)*arg) = value;
  return true;
}

bool wifiistreamcallback(pb_istream_t *stream, uint8_t *buf, size_t count) {
  // if end of file or no data or connection closed/dropped, set stream->bytes_left to 0
  return (client.read(buf, count) == count);
}

class Response {
  public:
    Response() {
      resp = krpc_schema_Response_init_zero;
      m_stream = {&wifiistreamcallback, NULL, SIZE_MAX};
    };
    bool decode(uint32_t &val) {
      resp.return_value.funcs.decode = &read_varint;
      resp.return_value.arg = &val;
      return v_decode() || resp.has_error;
    };
    bool decode(float &val) {
      resp.return_value.funcs.decode = &read_fixed32;
      resp.return_value.arg = &val;
      return v_decode() || resp.has_error;
    };
    bool decode(double &val) {
      resp.return_value.funcs.decode = &read_fixed64;
      resp.return_value.arg = &val;
      return v_decode() || resp.has_error;
    };
    bool decode_fixed(uint32_t &val) {
      resp.return_value.funcs.decode = &read_fixed32;
      resp.return_value.arg = &val;
      return v_decode() || resp.has_error;
    };
    krpc_schema_Response resp;
  private:
    bool v_decode() {
      resp.error.funcs.decode = &read_print_string;
      // wait for data to come
      while (client.available() == 0) { };
      return pb_decode_delimited(&m_stream, krpc_schema_Response_fields, &resp);
    };
    pb_istream_t m_stream;
};

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
    bool getResponse() {
      uint32_t val;
      send();
      return m_response.decode(val);
    }
    bool getResponse(uint32_t &val) {
      send();
      return m_response.decode(val);
    }
    bool getResponse(float &val) {
      send();
      return m_response.decode(val);
    }
    bool getResponse(double &val) {
      send();
      return m_response.decode(val);
    }
  private:
    KRPC::Response m_response;
    const char *m_service;
    const char *m_procedure;
    Argument **m_arguments;
    bool send()  {
      /* Create a stream that will write to our buffer. */
      pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

      /* Now we are ready to encode the message! */
      /* Then just check for any errors.. */
      if (!encode(&stream)) {
        Serial.print("Encoding failed:");
        Serial.println(PB_GET_ERROR(&stream));
        return false;
      }
      size_t res = client.write((const uint8_t*)buffer, stream.bytes_written);
      delay(0);
      return true;
      Serial.print("Encoded ");
      Serial.println(stream.bytes_written);
      /* dump to terminal */
      for (unsigned int i = 0; i < stream.bytes_written; i++) {
        Serial.print(buffer[i], HEX);
      }
      Serial.println();
      // send message - first its length then body
      Serial.println(res);
      return true;
    }
};

}

