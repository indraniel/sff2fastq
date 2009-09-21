/* I N C L U D E S ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sff.h"

/* P R O T O T Y P E S *******************************************************/
void process_sff_to_fastq(char *sff_file, char *fastq_file);
void construct_fastq_entry(FILE *fp, sff_read_header *rh, sff_read_data *rd);

/* G L O B A L S *************************************************************/
char prg_version[]  = "0.01";
char prg_name[] = "sff2fastq";

/* M A I N *******************************************************************/
int main(int argc, char *argv[]) {
    sff_common_header *h;
    char fastq_file[] = "test.fastq";

    printf("sff file : %s\n", argv[1]);
    process_sff_to_fastq(argv[1], fastq_file);

    return 0;
}

/* F U N C T I O N S *********************************************************/
void 
process_sff_to_fastq(char *sff_file, char *fastq_file) {
    sff_common_header h;
    sff_read_header rh;
    sff_read_data rd;
    FILE *sff_fp, *fastq_fp;

    if ( (sff_fp = fopen(sff_file, "r")) == NULL ) {
        fprintf(stderr, "Could not open file '%s' \n", sff_file);
        exit(1);
    }

    read_sff_common_header(sff_fp, &h);
    verify_sff_common_header(prg_name, prg_version, &h);

    printf("size of header: %d \n", sizeof(sff_common_header));
    printf("\tmagic        : 0x%x\n" , h.magic);
    printf("\tindex_offset : 0x%llx\n", h.index_offset);
    printf("\tindex_len    : 0x%x\n" , h.index_len);
    printf("\tnumreads     : 0x%x\n" , h.nreads);
    printf("\theader_len   : 0x%x\n" , h.header_len);
    printf("\tkey_len      : 0x%x\n" , h.key_len);
    printf("\tflow_len     : 0x%x\n" , h.flow_len);
    printf("\tflowgram_fmt : 0x%x\n" , h.flowgram_format);
    printf("\tflow         : %s\n  " , h.flow);
    printf("\tkey          : %s\n  " , h.key);
    printf("\n\n");

    if ( (fastq_fp = fopen(fastq_file, "w")) == NULL ) {
        fprintf(stderr, "Could not open file '%s' \n", fastq_file);
        exit(1);
    }

    printf("Found %d reads to process\n", h.nreads);

    register int i;
    int numreads = (int) h.nreads;
    for (i = 0; i < numreads; i++) {
        read_sff_read_header(sff_fp, &rh);
        read_sff_read_data(sff_fp, &rd, h.flow_len, rh.nbases);
//        construct_fastq_entry(fastq_fp, &rh, &rd, &h);
        free_sff_read_header(&rh);
        free_sff_read_data(&rd);
    }

    free_sff_common_header(&h);
    fclose(fastq_fp);
    fclose(sff_fp);
}
