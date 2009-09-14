#ifndef _SFF_H_
#define _SFF_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This is the overall sff header file */

typedef struct {
    uint32_t  magic;
    uint32_t  version;
    uint64_t  index_offset;
    uint32_t  index_len;
    uint32_t  nreads;
} sff_common_header;

#define SFF_MAGIC   0x2e736666 /* ".sff" */
#define SFF_VERSION "\0\0\0\1"

/*
 * There is a read_header per "reading" in the SFF archive.
 * It too is padded to an 8-byte boundary.
 */
typedef struct {
    uint16_t  header_len;
    uint16_t  name_len;
    uint32_t  nbases;
    uint16_t  clip_qual_left;
    uint16_t  clip_qual_right;
    uint16_t  clip_adapter_left;
    uint16_t  clip_adapter_right;
    char     *name;
} sff_read_header;

/*
 * The read_data section per reading, following the read_header.
 * It is padded to an 8-byte boundary.
 */
typedef struct {
    uint16_t *flowgram;   /* x 100.0 */
    uint8_t  *flow_index; /* relative to last */
    char     *bases;
    uint8_t  *quality;
} sff_read_data;

/* function to read the sff file */
sff_common_header * read_sff_common_header(const char *sff_file);


#ifdef __cplusplus
}
#endif

#endif /* _SFF_H */
