#include <stdio.h>
#include "sff.h"

int main(int argc, char *argv[]) {
    sff_common_header *h;
    char version[4];

    printf("sff file : %s\n", argv[1]);
    h = read_sff_common_header(argv[1]);
    printf("size of header: %d \n", sizeof(sff_common_header));
    printf("\tmagic        : 0x%x\n", h->magic);
    printf("\tindex_offset : 0x%llx\n", h->index_offset);
    printf("\tindex_len    : 0x%x\n", h->index_len);
    printf("\tnumreads     : 0x%x\n", h->nreads);

    return 0;
}
