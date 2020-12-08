/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: storage_query.proto */

#ifndef PROTOBUF_C_storage_5fquery_2eproto__INCLUDED
#define PROTOBUF_C_storage_5fquery_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _Mgmt__BioHealthReq Mgmt__BioHealthReq;
typedef struct _Mgmt__BioHealthResp Mgmt__BioHealthResp;
typedef struct _Mgmt__SmdDevReq Mgmt__SmdDevReq;
typedef struct _Mgmt__SmdDevResp Mgmt__SmdDevResp;
typedef struct _Mgmt__SmdDevResp__Device Mgmt__SmdDevResp__Device;
typedef struct _Mgmt__SmdPoolReq Mgmt__SmdPoolReq;
typedef struct _Mgmt__SmdPoolResp Mgmt__SmdPoolResp;
typedef struct _Mgmt__SmdPoolResp__Pool Mgmt__SmdPoolResp__Pool;
typedef struct _Mgmt__DevStateReq Mgmt__DevStateReq;
typedef struct _Mgmt__DevStateResp Mgmt__DevStateResp;
typedef struct _Mgmt__DevReplaceReq Mgmt__DevReplaceReq;
typedef struct _Mgmt__DevReplaceResp Mgmt__DevReplaceResp;
typedef struct _Mgmt__SmdQueryReq Mgmt__SmdQueryReq;
typedef struct _Mgmt__SmdQueryResp Mgmt__SmdQueryResp;
typedef struct _Mgmt__SmdQueryResp__Device Mgmt__SmdQueryResp__Device;
typedef struct _Mgmt__SmdQueryResp__Pool Mgmt__SmdQueryResp__Pool;
typedef struct _Mgmt__SmdQueryResp__RankResp Mgmt__SmdQueryResp__RankResp;


/* --- enums --- */


/* --- messages --- */

struct  _Mgmt__BioHealthReq
{
  ProtobufCMessage base;
  char *dev_uuid;
  char *tgt_id;
};
#define MGMT__BIO_HEALTH_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__bio_health_req__descriptor) \
    , (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string }


/*
 * BioHealthResp mirrors nvme_health_stats structure.
 */
struct  _Mgmt__BioHealthResp
{
  ProtobufCMessage base;
  char *model;
  char *serial;
  uint64_t timestamp;
  /*
   * Device health details
   */
  uint32_t warn_temp_time;
  uint32_t crit_temp_time;
  uint64_t ctrl_busy_time;
  uint64_t power_cycles;
  uint64_t power_on_hours;
  uint64_t unsafe_shutdowns;
  uint64_t media_errs;
  uint64_t err_log_entries;
  /*
   * I/O error counters
   */
  uint32_t bio_read_errs;
  uint32_t bio_write_errs;
  uint32_t bio_unmap_errs;
  uint32_t checksum_errs;
  /*
   * in Kelvin
   */
  uint32_t temperature;
  /*
   * Critical warnings
   */
  protobuf_c_boolean temp_warn;
  protobuf_c_boolean avail_spare_warn;
  protobuf_c_boolean dev_reliability_warn;
  protobuf_c_boolean read_only_warn;
  /*
   * volatile memory backup
   */
  protobuf_c_boolean volatile_mem_warn;
  /*
   * DAOS err code
   */
  int32_t status;
  /*
   * UUID of blobstore
   */
  char *dev_uuid;
  /*
   * Usage stats
   */
  /*
   * size of blobstore
   */
  uint64_t total_bytes;
  /*
   * free space in blobstore
   */
  uint64_t avail_bytes;
};
#define MGMT__BIO_HEALTH_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__bio_health_resp__descriptor) \
    , (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (char *)protobuf_c_empty_string, 0, 0 }


struct  _Mgmt__SmdDevReq
{
  ProtobufCMessage base;
};
#define MGMT__SMD_DEV_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_dev_req__descriptor) \
     }


struct  _Mgmt__SmdDevResp__Device
{
  ProtobufCMessage base;
  /*
   * UUID of blobstore
   */
  char *uuid;
  /*
   * VOS target IDs
   */
  size_t n_tgt_ids;
  int32_t *tgt_ids;
  /*
   * BIO device state
   */
  char *state;
  /*
   * Transport address of blobstore
   */
  char *traddr;
};
#define MGMT__SMD_DEV_RESP__DEVICE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_dev_resp__device__descriptor) \
    , (char *)protobuf_c_empty_string, 0,NULL, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string }


struct  _Mgmt__SmdDevResp
{
  ProtobufCMessage base;
  int32_t status;
  size_t n_devices;
  Mgmt__SmdDevResp__Device **devices;
};
#define MGMT__SMD_DEV_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_dev_resp__descriptor) \
    , 0, 0,NULL }


struct  _Mgmt__SmdPoolReq
{
  ProtobufCMessage base;
};
#define MGMT__SMD_POOL_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_pool_req__descriptor) \
     }


struct  _Mgmt__SmdPoolResp__Pool
{
  ProtobufCMessage base;
  /*
   * UUID of VOS pool
   */
  char *uuid;
  /*
   * VOS target IDs
   */
  size_t n_tgt_ids;
  int32_t *tgt_ids;
  /*
   * SPDK blobs
   */
  size_t n_blobs;
  uint64_t *blobs;
};
#define MGMT__SMD_POOL_RESP__POOL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_pool_resp__pool__descriptor) \
    , (char *)protobuf_c_empty_string, 0,NULL, 0,NULL }


struct  _Mgmt__SmdPoolResp
{
  ProtobufCMessage base;
  int32_t status;
  size_t n_pools;
  Mgmt__SmdPoolResp__Pool **pools;
};
#define MGMT__SMD_POOL_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_pool_resp__descriptor) \
    , 0, 0,NULL }


struct  _Mgmt__DevStateReq
{
  ProtobufCMessage base;
  /*
   * UUID of blobstore
   */
  char *dev_uuid;
};
#define MGMT__DEV_STATE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__dev_state_req__descriptor) \
    , (char *)protobuf_c_empty_string }


struct  _Mgmt__DevStateResp
{
  ProtobufCMessage base;
  /*
   * DAOS error code
   */
  int32_t status;
  /*
   * UUID of blobstore
   */
  char *dev_uuid;
  /*
   * Transport address of blobstore
   */
  char *dev_state;
};
#define MGMT__DEV_STATE_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__dev_state_resp__descriptor) \
    , 0, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string }


struct  _Mgmt__DevReplaceReq
{
  ProtobufCMessage base;
  /*
   * UUID of old (hot-removed) blobstore/device
   */
  char *old_dev_uuid;
  /*
   * UUID of new (hot-plugged) blobstore/device
   */
  char *new_dev_uuid;
  /*
   * Skip device reintegration if set
   */
  protobuf_c_boolean noreint;
};
#define MGMT__DEV_REPLACE_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__dev_replace_req__descriptor) \
    , (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, 0 }


struct  _Mgmt__DevReplaceResp
{
  ProtobufCMessage base;
  /*
   * DAOS error code
   */
  int32_t status;
  /*
   * UUID of new (hot-plugged) blobstore/device
   */
  char *new_dev_uuid;
  /*
   * BIO device state
   */
  char *dev_state;
};
#define MGMT__DEV_REPLACE_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__dev_replace_resp__descriptor) \
    , 0, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string }


struct  _Mgmt__SmdQueryReq
{
  ProtobufCMessage base;
  /*
   * query should omit devices
   */
  protobuf_c_boolean omitdevices;
  /*
   * query should omit pools
   */
  protobuf_c_boolean omitpools;
  /*
   * query should include BIO health for devices
   */
  protobuf_c_boolean includebiohealth;
  /*
   * set the specified device to FAULTY
   */
  protobuf_c_boolean setfaulty;
  /*
   * constrain query to this UUID (pool or device)
   */
  char *uuid;
  /*
   * response should only include information about this rank
   */
  uint32_t rank;
  /*
   * response should only include information about this VOS target
   */
  char *target;
  /*
   * UUID of new device to replace storage with
   */
  char *replaceuuid;
  /*
   * specify if device reint is needed (used for replace cmd)
   */
  protobuf_c_boolean noreint;
};
#define MGMT__SMD_QUERY_REQ__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_query_req__descriptor) \
    , 0, 0, 0, 0, (char *)protobuf_c_empty_string, 0, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, 0 }


struct  _Mgmt__SmdQueryResp__Device
{
  ProtobufCMessage base;
  /*
   * UUID of blobstore
   */
  char *uuid;
  /*
   * VOS target IDs
   */
  size_t n_tgt_ids;
  int32_t *tgt_ids;
  /*
   * BIO device state
   */
  char *state;
  /*
   * Transport address of blobstore
   */
  char *traddr;
  /*
   * optional BIO health
   */
  Mgmt__BioHealthResp *health;
};
#define MGMT__SMD_QUERY_RESP__DEVICE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_query_resp__device__descriptor) \
    , (char *)protobuf_c_empty_string, 0,NULL, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, NULL }


struct  _Mgmt__SmdQueryResp__Pool
{
  ProtobufCMessage base;
  /*
   * UUID of VOS pool
   */
  char *uuid;
  /*
   * VOS target IDs
   */
  size_t n_tgt_ids;
  int32_t *tgt_ids;
  /*
   * SPDK blobs
   */
  size_t n_blobs;
  uint64_t *blobs;
};
#define MGMT__SMD_QUERY_RESP__POOL__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_query_resp__pool__descriptor) \
    , (char *)protobuf_c_empty_string, 0,NULL, 0,NULL }


struct  _Mgmt__SmdQueryResp__RankResp
{
  ProtobufCMessage base;
  /*
   * rank to which this response corresponds
   */
  uint32_t rank;
  /*
   * List of devices on the rank
   */
  size_t n_devices;
  Mgmt__SmdQueryResp__Device **devices;
  /*
   * List of pools on the rank
   */
  size_t n_pools;
  Mgmt__SmdQueryResp__Pool **pools;
};
#define MGMT__SMD_QUERY_RESP__RANK_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_query_resp__rank_resp__descriptor) \
    , 0, 0,NULL, 0,NULL }


struct  _Mgmt__SmdQueryResp
{
  ProtobufCMessage base;
  /*
   * DAOS error code
   */
  int32_t status;
  /*
   * List of per-rank responses
   */
  size_t n_ranks;
  Mgmt__SmdQueryResp__RankResp **ranks;
};
#define MGMT__SMD_QUERY_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&mgmt__smd_query_resp__descriptor) \
    , 0, 0,NULL }


/* Mgmt__BioHealthReq methods */
void   mgmt__bio_health_req__init
                     (Mgmt__BioHealthReq         *message);
size_t mgmt__bio_health_req__get_packed_size
                     (const Mgmt__BioHealthReq   *message);
size_t mgmt__bio_health_req__pack
                     (const Mgmt__BioHealthReq   *message,
                      uint8_t             *out);
size_t mgmt__bio_health_req__pack_to_buffer
                     (const Mgmt__BioHealthReq   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__BioHealthReq *
       mgmt__bio_health_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__bio_health_req__free_unpacked
                     (Mgmt__BioHealthReq *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__BioHealthResp methods */
void   mgmt__bio_health_resp__init
                     (Mgmt__BioHealthResp         *message);
size_t mgmt__bio_health_resp__get_packed_size
                     (const Mgmt__BioHealthResp   *message);
size_t mgmt__bio_health_resp__pack
                     (const Mgmt__BioHealthResp   *message,
                      uint8_t             *out);
size_t mgmt__bio_health_resp__pack_to_buffer
                     (const Mgmt__BioHealthResp   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__BioHealthResp *
       mgmt__bio_health_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__bio_health_resp__free_unpacked
                     (Mgmt__BioHealthResp *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__SmdDevReq methods */
void   mgmt__smd_dev_req__init
                     (Mgmt__SmdDevReq         *message);
size_t mgmt__smd_dev_req__get_packed_size
                     (const Mgmt__SmdDevReq   *message);
size_t mgmt__smd_dev_req__pack
                     (const Mgmt__SmdDevReq   *message,
                      uint8_t             *out);
size_t mgmt__smd_dev_req__pack_to_buffer
                     (const Mgmt__SmdDevReq   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__SmdDevReq *
       mgmt__smd_dev_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__smd_dev_req__free_unpacked
                     (Mgmt__SmdDevReq *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__SmdDevResp__Device methods */
void   mgmt__smd_dev_resp__device__init
                     (Mgmt__SmdDevResp__Device         *message);
/* Mgmt__SmdDevResp methods */
void   mgmt__smd_dev_resp__init
                     (Mgmt__SmdDevResp         *message);
size_t mgmt__smd_dev_resp__get_packed_size
                     (const Mgmt__SmdDevResp   *message);
size_t mgmt__smd_dev_resp__pack
                     (const Mgmt__SmdDevResp   *message,
                      uint8_t             *out);
size_t mgmt__smd_dev_resp__pack_to_buffer
                     (const Mgmt__SmdDevResp   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__SmdDevResp *
       mgmt__smd_dev_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__smd_dev_resp__free_unpacked
                     (Mgmt__SmdDevResp *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__SmdPoolReq methods */
void   mgmt__smd_pool_req__init
                     (Mgmt__SmdPoolReq         *message);
size_t mgmt__smd_pool_req__get_packed_size
                     (const Mgmt__SmdPoolReq   *message);
size_t mgmt__smd_pool_req__pack
                     (const Mgmt__SmdPoolReq   *message,
                      uint8_t             *out);
size_t mgmt__smd_pool_req__pack_to_buffer
                     (const Mgmt__SmdPoolReq   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__SmdPoolReq *
       mgmt__smd_pool_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__smd_pool_req__free_unpacked
                     (Mgmt__SmdPoolReq *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__SmdPoolResp__Pool methods */
void   mgmt__smd_pool_resp__pool__init
                     (Mgmt__SmdPoolResp__Pool         *message);
/* Mgmt__SmdPoolResp methods */
void   mgmt__smd_pool_resp__init
                     (Mgmt__SmdPoolResp         *message);
size_t mgmt__smd_pool_resp__get_packed_size
                     (const Mgmt__SmdPoolResp   *message);
size_t mgmt__smd_pool_resp__pack
                     (const Mgmt__SmdPoolResp   *message,
                      uint8_t             *out);
size_t mgmt__smd_pool_resp__pack_to_buffer
                     (const Mgmt__SmdPoolResp   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__SmdPoolResp *
       mgmt__smd_pool_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__smd_pool_resp__free_unpacked
                     (Mgmt__SmdPoolResp *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__DevStateReq methods */
void   mgmt__dev_state_req__init
                     (Mgmt__DevStateReq         *message);
size_t mgmt__dev_state_req__get_packed_size
                     (const Mgmt__DevStateReq   *message);
size_t mgmt__dev_state_req__pack
                     (const Mgmt__DevStateReq   *message,
                      uint8_t             *out);
size_t mgmt__dev_state_req__pack_to_buffer
                     (const Mgmt__DevStateReq   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__DevStateReq *
       mgmt__dev_state_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__dev_state_req__free_unpacked
                     (Mgmt__DevStateReq *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__DevStateResp methods */
void   mgmt__dev_state_resp__init
                     (Mgmt__DevStateResp         *message);
size_t mgmt__dev_state_resp__get_packed_size
                     (const Mgmt__DevStateResp   *message);
size_t mgmt__dev_state_resp__pack
                     (const Mgmt__DevStateResp   *message,
                      uint8_t             *out);
size_t mgmt__dev_state_resp__pack_to_buffer
                     (const Mgmt__DevStateResp   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__DevStateResp *
       mgmt__dev_state_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__dev_state_resp__free_unpacked
                     (Mgmt__DevStateResp *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__DevReplaceReq methods */
void   mgmt__dev_replace_req__init
                     (Mgmt__DevReplaceReq         *message);
size_t mgmt__dev_replace_req__get_packed_size
                     (const Mgmt__DevReplaceReq   *message);
size_t mgmt__dev_replace_req__pack
                     (const Mgmt__DevReplaceReq   *message,
                      uint8_t             *out);
size_t mgmt__dev_replace_req__pack_to_buffer
                     (const Mgmt__DevReplaceReq   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__DevReplaceReq *
       mgmt__dev_replace_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__dev_replace_req__free_unpacked
                     (Mgmt__DevReplaceReq *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__DevReplaceResp methods */
void   mgmt__dev_replace_resp__init
                     (Mgmt__DevReplaceResp         *message);
size_t mgmt__dev_replace_resp__get_packed_size
                     (const Mgmt__DevReplaceResp   *message);
size_t mgmt__dev_replace_resp__pack
                     (const Mgmt__DevReplaceResp   *message,
                      uint8_t             *out);
size_t mgmt__dev_replace_resp__pack_to_buffer
                     (const Mgmt__DevReplaceResp   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__DevReplaceResp *
       mgmt__dev_replace_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__dev_replace_resp__free_unpacked
                     (Mgmt__DevReplaceResp *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__SmdQueryReq methods */
void   mgmt__smd_query_req__init
                     (Mgmt__SmdQueryReq         *message);
size_t mgmt__smd_query_req__get_packed_size
                     (const Mgmt__SmdQueryReq   *message);
size_t mgmt__smd_query_req__pack
                     (const Mgmt__SmdQueryReq   *message,
                      uint8_t             *out);
size_t mgmt__smd_query_req__pack_to_buffer
                     (const Mgmt__SmdQueryReq   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__SmdQueryReq *
       mgmt__smd_query_req__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__smd_query_req__free_unpacked
                     (Mgmt__SmdQueryReq *message,
                      ProtobufCAllocator *allocator);
/* Mgmt__SmdQueryResp__Device methods */
void   mgmt__smd_query_resp__device__init
                     (Mgmt__SmdQueryResp__Device         *message);
/* Mgmt__SmdQueryResp__Pool methods */
void   mgmt__smd_query_resp__pool__init
                     (Mgmt__SmdQueryResp__Pool         *message);
/* Mgmt__SmdQueryResp__RankResp methods */
void   mgmt__smd_query_resp__rank_resp__init
                     (Mgmt__SmdQueryResp__RankResp         *message);
/* Mgmt__SmdQueryResp methods */
void   mgmt__smd_query_resp__init
                     (Mgmt__SmdQueryResp         *message);
size_t mgmt__smd_query_resp__get_packed_size
                     (const Mgmt__SmdQueryResp   *message);
size_t mgmt__smd_query_resp__pack
                     (const Mgmt__SmdQueryResp   *message,
                      uint8_t             *out);
size_t mgmt__smd_query_resp__pack_to_buffer
                     (const Mgmt__SmdQueryResp   *message,
                      ProtobufCBuffer     *buffer);
Mgmt__SmdQueryResp *
       mgmt__smd_query_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   mgmt__smd_query_resp__free_unpacked
                     (Mgmt__SmdQueryResp *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Mgmt__BioHealthReq_Closure)
                 (const Mgmt__BioHealthReq *message,
                  void *closure_data);
typedef void (*Mgmt__BioHealthResp_Closure)
                 (const Mgmt__BioHealthResp *message,
                  void *closure_data);
typedef void (*Mgmt__SmdDevReq_Closure)
                 (const Mgmt__SmdDevReq *message,
                  void *closure_data);
typedef void (*Mgmt__SmdDevResp__Device_Closure)
                 (const Mgmt__SmdDevResp__Device *message,
                  void *closure_data);
typedef void (*Mgmt__SmdDevResp_Closure)
                 (const Mgmt__SmdDevResp *message,
                  void *closure_data);
typedef void (*Mgmt__SmdPoolReq_Closure)
                 (const Mgmt__SmdPoolReq *message,
                  void *closure_data);
typedef void (*Mgmt__SmdPoolResp__Pool_Closure)
                 (const Mgmt__SmdPoolResp__Pool *message,
                  void *closure_data);
typedef void (*Mgmt__SmdPoolResp_Closure)
                 (const Mgmt__SmdPoolResp *message,
                  void *closure_data);
typedef void (*Mgmt__DevStateReq_Closure)
                 (const Mgmt__DevStateReq *message,
                  void *closure_data);
typedef void (*Mgmt__DevStateResp_Closure)
                 (const Mgmt__DevStateResp *message,
                  void *closure_data);
typedef void (*Mgmt__DevReplaceReq_Closure)
                 (const Mgmt__DevReplaceReq *message,
                  void *closure_data);
typedef void (*Mgmt__DevReplaceResp_Closure)
                 (const Mgmt__DevReplaceResp *message,
                  void *closure_data);
typedef void (*Mgmt__SmdQueryReq_Closure)
                 (const Mgmt__SmdQueryReq *message,
                  void *closure_data);
typedef void (*Mgmt__SmdQueryResp__Device_Closure)
                 (const Mgmt__SmdQueryResp__Device *message,
                  void *closure_data);
typedef void (*Mgmt__SmdQueryResp__Pool_Closure)
                 (const Mgmt__SmdQueryResp__Pool *message,
                  void *closure_data);
typedef void (*Mgmt__SmdQueryResp__RankResp_Closure)
                 (const Mgmt__SmdQueryResp__RankResp *message,
                  void *closure_data);
typedef void (*Mgmt__SmdQueryResp_Closure)
                 (const Mgmt__SmdQueryResp *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor mgmt__bio_health_req__descriptor;
extern const ProtobufCMessageDescriptor mgmt__bio_health_resp__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_dev_req__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_dev_resp__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_dev_resp__device__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_pool_req__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_pool_resp__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_pool_resp__pool__descriptor;
extern const ProtobufCMessageDescriptor mgmt__dev_state_req__descriptor;
extern const ProtobufCMessageDescriptor mgmt__dev_state_resp__descriptor;
extern const ProtobufCMessageDescriptor mgmt__dev_replace_req__descriptor;
extern const ProtobufCMessageDescriptor mgmt__dev_replace_resp__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_query_req__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_query_resp__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_query_resp__device__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_query_resp__pool__descriptor;
extern const ProtobufCMessageDescriptor mgmt__smd_query_resp__rank_resp__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_storage_5fquery_2eproto__INCLUDED */
