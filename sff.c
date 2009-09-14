#include <stdio.h>
#include <stdlib.h>
#include <endian.h>
#include "sff.h"

sff_common_header * read_sff_common_header(const char *sff_file) {
    sff_common_header *h;
    FILE *fp;

    if ( (fp = fopen(sff_file, "r")) == NULL ) {
        fprintf(stderr, "Could not open file '%s' \n", sff_file);
        exit(1);
    }

//    fread(h, sizeof(sff_read_header), 1, fp);

    fread(&(h->magic),        sizeof(uint32_t), 1, fp);
    fread(&(h->version),      sizeof(char),     4, fp);
    fread(&(h->index_offset), sizeof(uint64_t), 1, fp);
    fread(&(h->index_len),    sizeof(uint32_t), 1, fp);
    fread(&(h->nreads),       sizeof(uint32_t), 1, fp);

    /* sff files are in big endian notation so adjust appropriately */
    h->magic        = htobe32(h->magic);
    h->index_offset = htobe64(h->index_offset);
    h->index_len    = htobe32(h->index_len);
    h->nreads       = htobe32(h->nreads);

    return h;
}
