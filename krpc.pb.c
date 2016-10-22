/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.7-dev at Fri Oct  7 00:07:58 2016. */
/* Generated from krpc.proto from krpc-0.3.6 */

#include "krpc.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t krpc_schema_Request_fields[4] = {
    PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, krpc_schema_Request, service, service, 0),
    PB_FIELD(  2, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_Request, procedure, service, 0),
    PB_FIELD(  3, MESSAGE , REPEATED, CALLBACK, OTHER, krpc_schema_Request, arguments, procedure, &krpc_schema_Argument_fields),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Argument_fields[3] = {
    PB_FIELD(  1, UINT32  , OPTIONAL, STATIC  , FIRST, krpc_schema_Argument, position, position, 0),
    PB_FIELD(  2, BYTES   , OPTIONAL, CALLBACK, OTHER, krpc_schema_Argument, value, position, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Response_fields[6] = {
    PB_FIELD(  1, DOUBLE  , OPTIONAL, STATIC  , FIRST, krpc_schema_Response, time, time, 0),
    PB_FIELD(  2, BOOL    , OPTIONAL, STATIC  , OTHER, krpc_schema_Response, has_error, time, 0),
    PB_FIELD(  3, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_Response, error, has_error, 0),
    PB_FIELD(  4, BOOL    , OPTIONAL, STATIC  , OTHER, krpc_schema_Response, has_return_value, error, 0),
    PB_FIELD(  5, BYTES   , OPTIONAL, CALLBACK, OTHER, krpc_schema_Response, return_value, has_return_value, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_StreamMessage_fields[2] = {
    PB_FIELD(  1, MESSAGE , REPEATED, CALLBACK, FIRST, krpc_schema_StreamMessage, responses, responses, &krpc_schema_StreamResponse_fields),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_StreamResponse_fields[3] = {
    PB_FIELD(  1, UINT32  , OPTIONAL, STATIC  , FIRST, krpc_schema_StreamResponse, id, id, 0),
    PB_FIELD(  2, MESSAGE , OPTIONAL, STATIC  , OTHER, krpc_schema_StreamResponse, response, id, &krpc_schema_Response_fields),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Services_fields[2] = {
    PB_FIELD(  1, MESSAGE , REPEATED, CALLBACK, FIRST, krpc_schema_Services, services, services, &krpc_schema_Service_fields),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Service_fields[6] = {
    PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, krpc_schema_Service, name, name, 0),
    PB_FIELD(  2, MESSAGE , REPEATED, CALLBACK, OTHER, krpc_schema_Service, procedures, name, &krpc_schema_Procedure_fields),
    PB_FIELD(  3, MESSAGE , REPEATED, CALLBACK, OTHER, krpc_schema_Service, classes, procedures, &krpc_schema_Class_fields),
    PB_FIELD(  4, MESSAGE , REPEATED, CALLBACK, OTHER, krpc_schema_Service, enumerations, classes, &krpc_schema_Enumeration_fields),
    PB_FIELD(  5, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_Service, documentation, enumerations, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Procedure_fields[7] = {
    PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, krpc_schema_Procedure, name, name, 0),
    PB_FIELD(  2, MESSAGE , REPEATED, CALLBACK, OTHER, krpc_schema_Procedure, parameters, name, &krpc_schema_Parameter_fields),
    PB_FIELD(  3, BOOL    , OPTIONAL, STATIC  , OTHER, krpc_schema_Procedure, has_return_type, parameters, 0),
    PB_FIELD(  4, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_Procedure, return_type, has_return_type, 0),
    PB_FIELD(  5, STRING  , REPEATED, CALLBACK, OTHER, krpc_schema_Procedure, attributes, return_type, 0),
    PB_FIELD(  6, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_Procedure, documentation, attributes, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Parameter_fields[5] = {
    PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, krpc_schema_Parameter, name, name, 0),
    PB_FIELD(  2, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_Parameter, type, name, 0),
    PB_FIELD(  3, BOOL    , OPTIONAL, STATIC  , OTHER, krpc_schema_Parameter, has_default_value, type, 0),
    PB_FIELD(  4, BYTES   , OPTIONAL, CALLBACK, OTHER, krpc_schema_Parameter, default_value, has_default_value, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Class_fields[3] = {
    PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, krpc_schema_Class, name, name, 0),
    PB_FIELD(  2, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_Class, documentation, name, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Enumeration_fields[4] = {
    PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, krpc_schema_Enumeration, name, name, 0),
    PB_FIELD(  2, MESSAGE , REPEATED, CALLBACK, OTHER, krpc_schema_Enumeration, values, name, &krpc_schema_EnumerationValue_fields),
    PB_FIELD(  3, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_Enumeration, documentation, values, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_EnumerationValue_fields[4] = {
    PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, krpc_schema_EnumerationValue, name, name, 0),
    PB_FIELD(  2, INT32   , OPTIONAL, STATIC  , OTHER, krpc_schema_EnumerationValue, value, name, 0),
    PB_FIELD(  3, STRING  , OPTIONAL, CALLBACK, OTHER, krpc_schema_EnumerationValue, documentation, value, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_List_fields[2] = {
    PB_FIELD(  1, BYTES   , REPEATED, CALLBACK, FIRST, krpc_schema_List, items, items, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Dictionary_fields[2] = {
    PB_FIELD(  1, MESSAGE , REPEATED, CALLBACK, FIRST, krpc_schema_Dictionary, entries, entries, &krpc_schema_DictionaryEntry_fields),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_DictionaryEntry_fields[3] = {
    PB_FIELD(  1, BYTES   , OPTIONAL, CALLBACK, FIRST, krpc_schema_DictionaryEntry, key, key, 0),
    PB_FIELD(  2, BYTES   , OPTIONAL, CALLBACK, OTHER, krpc_schema_DictionaryEntry, value, key, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Set_fields[2] = {
    PB_FIELD(  1, BYTES   , REPEATED, CALLBACK, FIRST, krpc_schema_Set, items, items, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Tuple_fields[2] = {
    PB_FIELD(  1, BYTES   , REPEATED, CALLBACK, FIRST, krpc_schema_Tuple, items, items, 0),
    PB_LAST_FIELD
};

const pb_field_t krpc_schema_Status_fields[20] = {
    PB_FIELD(  1, STRING  , OPTIONAL, CALLBACK, FIRST, krpc_schema_Status, version, version, 0),
    PB_FIELD(  2, UINT64  , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, bytes_read, version, 0),
    PB_FIELD(  3, UINT64  , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, bytes_written, bytes_read, 0),
    PB_FIELD(  4, FLOAT   , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, bytes_read_rate, bytes_written, 0),
    PB_FIELD(  5, FLOAT   , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, bytes_written_rate, bytes_read_rate, 0),
    PB_FIELD(  6, UINT64  , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, rpcs_executed, bytes_written_rate, 0),
    PB_FIELD(  7, FLOAT   , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, rpc_rate, rpcs_executed, 0),
    PB_FIELD(  8, BOOL    , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, one_rpc_per_update, rpc_rate, 0),
    PB_FIELD(  9, UINT32  , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, max_time_per_update, one_rpc_per_update, 0),
    PB_FIELD( 10, BOOL    , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, adaptive_rate_control, max_time_per_update, 0),
    PB_FIELD( 11, BOOL    , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, blocking_recv, adaptive_rate_control, 0),
    PB_FIELD( 12, UINT32  , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, recv_timeout, blocking_recv, 0),
    PB_FIELD( 13, FLOAT   , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, time_per_rpc_update, recv_timeout, 0),
    PB_FIELD( 14, FLOAT   , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, poll_time_per_rpc_update, time_per_rpc_update, 0),
    PB_FIELD( 15, FLOAT   , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, exec_time_per_rpc_update, poll_time_per_rpc_update, 0),
    PB_FIELD( 16, UINT32  , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, stream_rpcs, exec_time_per_rpc_update, 0),
    PB_FIELD( 17, UINT64  , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, stream_rpcs_executed, stream_rpcs, 0),
    PB_FIELD( 18, FLOAT   , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, stream_rpc_rate, stream_rpcs_executed, 0),
    PB_FIELD( 19, FLOAT   , OPTIONAL, STATIC  , OTHER, krpc_schema_Status, time_per_stream_update, stream_rpc_rate, 0),
    PB_LAST_FIELD
};


/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(krpc_schema_Request, arguments) < 65536 && pb_membersize(krpc_schema_StreamMessage, responses) < 65536 && pb_membersize(krpc_schema_StreamResponse, response) < 65536 && pb_membersize(krpc_schema_Services, services) < 65536 && pb_membersize(krpc_schema_Service, procedures) < 65536 && pb_membersize(krpc_schema_Service, classes) < 65536 && pb_membersize(krpc_schema_Service, enumerations) < 65536 && pb_membersize(krpc_schema_Procedure, parameters) < 65536 && pb_membersize(krpc_schema_Enumeration, values) < 65536 && pb_membersize(krpc_schema_Dictionary, entries) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_krpc_schema_Request_krpc_schema_Argument_krpc_schema_Response_krpc_schema_StreamMessage_krpc_schema_StreamResponse_krpc_schema_Services_krpc_schema_Service_krpc_schema_Procedure_krpc_schema_Parameter_krpc_schema_Class_krpc_schema_Enumeration_krpc_schema_EnumerationValue_krpc_schema_List_krpc_schema_Dictionary_krpc_schema_DictionaryEntry_krpc_schema_Set_krpc_schema_Tuple_krpc_schema_Status)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(krpc_schema_Request, arguments) < 256 && pb_membersize(krpc_schema_StreamMessage, responses) < 256 && pb_membersize(krpc_schema_StreamResponse, response) < 256 && pb_membersize(krpc_schema_Services, services) < 256 && pb_membersize(krpc_schema_Service, procedures) < 256 && pb_membersize(krpc_schema_Service, classes) < 256 && pb_membersize(krpc_schema_Service, enumerations) < 256 && pb_membersize(krpc_schema_Procedure, parameters) < 256 && pb_membersize(krpc_schema_Enumeration, values) < 256 && pb_membersize(krpc_schema_Dictionary, entries) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_krpc_schema_Request_krpc_schema_Argument_krpc_schema_Response_krpc_schema_StreamMessage_krpc_schema_StreamResponse_krpc_schema_Services_krpc_schema_Service_krpc_schema_Procedure_krpc_schema_Parameter_krpc_schema_Class_krpc_schema_Enumeration_krpc_schema_EnumerationValue_krpc_schema_List_krpc_schema_Dictionary_krpc_schema_DictionaryEntry_krpc_schema_Set_krpc_schema_Tuple_krpc_schema_Status)
#endif


/* On some platforms (such as AVR), double is really float.
 * These are not directly supported by nanopb, but see example_avr_double.
 * To get rid of this error, remove any double fields from your .proto.
 */
PB_STATIC_ASSERT(sizeof(double) == 8, DOUBLE_MUST_BE_8_BYTES)

/* @@protoc_insertion_point(eof) */
