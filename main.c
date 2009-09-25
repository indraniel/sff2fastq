/* I N C L U D E S ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    char *name;
    char *bases;
    register int i, j;
    uint8_t quality_char;
    int numreads = (int) h.nreads;
    for (i = 0; i < numreads; i++) {
        read_sff_read_header(sff_fp, &rh);
        read_sff_read_data(sff_fp, &rd, h.flow_len, rh.nbases);
//        construct_fastq_entry(fastq_fp, &rh, &rd, &h);

        /* create bases string */
        int bases_length = (int) rh.nbases + 1; // account for NULL termination
        bases = (char *) malloc( bases_length * sizeof(char) );
        if (!bases) {
            printf("Out of memory! For bases string!\n");
            exit(1);
        }
        memset(bases, '\0', (size_t) bases_length);
        strncpy(bases, rd.bases, (size_t) rh.nbases);

        /* create read name string */
        int name_length = (int) rh.name_len + 1; // account for NULL termination
        name = (char *) malloc( bases_length * sizeof(char) );
        if (!name) {
            printf("Out of memory! For name string!\n");
            exit(1);
        }
        memset(name, '\0', (size_t) name_length);
        strncpy(name, rh.name, (size_t) rh.name_len);

        printf("[%d | %d] %s\n%s\n", (i+1), numreads, name, bases);

        /* print out quality values (as integer) */
        for (j = 0; j < rh.nbases; j++) {
            printf("%d ", (int) rd.quality[j] );
        }
        printf("\n");

        /* print out quality values (as characters) 
         * formula taken from http://maq.sourceforge.net/fastq.shtml */
        for (j = 0; j < rh.nbases; j++) {
            quality_char = (rd.quality[j] <= 93 ? rd.quality[j] : 93) + 33;
            printf("%c ", (char) quality_char );
        }
        printf("\n");


        free(name);
        free(bases);
        free_sff_read_header(&rh);
        free_sff_read_data(&rd);
    }

    free_sff_common_header(&h);
    fclose(fastq_fp);
    fclose(sff_fp);
}
