/* L I C E N S E *************************************************************/

/*
    Copyright (C) 2009, 2010 Indraniel Das <indraniel@gmail.com>
                             and Washington University in St. Louis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, see <http://www.gnu.org/licenses/>
*/

/* I N C L U D E S ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sff.h"

/* D E F I N E S *************************************************************/
#define VERSION "0.8.0"
#define PRG_NAME "sff2fastq"
#define FASTQ_FILENAME_MAX_LENGTH 1024
#define SFF_FILENAME_MAX_LENGTH 1024

/* P R O T O T Y P E S *******************************************************/
void help_message(void);
void version_info(void);
void process_options(int argc, char *argv[]);
void process_sff_to_fastq(char *sff_file, char *fastq_file, int trim_flag);

void construct_fastq_entry(FILE *fp,
                           char *name,
                           char *bases,
                           uint8_t *quality,
                           int nbases);

/* G L O B A L S *************************************************************/
char fastq_file[FASTQ_FILENAME_MAX_LENGTH] = { '\0' };
char sff_file[SFF_FILENAME_MAX_LENGTH] = { '\0' };

// trimming is enabled by default -- like 454 tools
int  opt_trim  = 1;

/* M A I N *******************************************************************/
int main(int argc, char *argv[]) {

    process_options(argc, argv);
    process_sff_to_fastq(sff_file, fastq_file, opt_trim);

    return 0;
}

/* F U N C T I O N S *********************************************************/
void
help_message() {
    fprintf(stdout, "Usage: %s %s %s\n", PRG_NAME, "[options]", "[sff_file]");
    fprintf(stdout, "\t%-20s%-20s\n", "-h", "This help message");
    fprintf(stdout, "\t%-20s%-20s\n", "-v", "Program and version information");
    fprintf(stdout, "\t%-20s%-20s\n", "-n", "Output the untrimmed sequence and quality scores");
    fprintf(stdout, "\t%-20s%-20s %s\n",
                    "-o <fastq_file>",
                    "Desired fastq output file.",
                    "If not specified, defaults to stdout");
}

void
version_info() {
    fprintf(stdout, "%s -- version: %s\n", PRG_NAME, VERSION);
}

void
process_options(int argc, char *argv[]) {
    int c;
    int index;
    char *opt_o_value = NULL;

    while( (c = getopt(argc, argv, "hvno:")) != -1 ) {
        switch(c) {
            case 'h':
                help_message();
                exit(0);
                break;
            case 'v':
                version_info();
                exit(0);
                break;
            case 'o':
                opt_o_value = optarg;
                break;
            case 'n':
                opt_trim = 0;    /* disable trimming -- like 454 tools */
                break;
            case '?':
                exit(1);
             default:
                abort();
        }
    }

    if ( opt_o_value != NULL ) {
        strncpy(fastq_file, opt_o_value, FASTQ_FILENAME_MAX_LENGTH);
    }

    /* process the remaining command line arguments */
    for (index = optind; index < argc; index++) {
        strncpy(sff_file, argv[index], SFF_FILENAME_MAX_LENGTH);
    }

//    /* just take the first passed in non-getopt argument as the sff file */
//    strncpy(sff_file, argv[optind], SFF_FILENAME_MAX_LENGTH);
}

void
process_sff_to_fastq(char *sff_file, char *fastq_file, int trim_flag) {
    sff_common_header h;
    sff_read_header rh;
    sff_read_data rd;
    FILE *sff_fp, *fastq_fp;

    if ( !strlen(sff_file) ) {
        sff_fp = stdin;
    }
    else if ( (sff_fp = fopen(sff_file, "r")) == NULL ) {
        fprintf(stderr,
                "[err] Could not open file '%s' for reading.\n", sff_file);
        exit(1);
    }

    read_sff_common_header(sff_fp, &h);
    verify_sff_common_header(PRG_NAME, VERSION, &h);

//    printf("size of header: %d \n", sizeof(sff_common_header));
//    printf("\tmagic        : 0x%x\n" , h.magic);
//    printf("\tindex_offset : 0x%llx\n", h.index_offset);
//    printf("\tindex_len    : 0x%x\n" , h.index_len);
//    printf("\tnumreads     : 0x%x\n" , h.nreads);
//    printf("\theader_len   : 0x%x\n" , h.header_len);
//    printf("\tkey_len      : 0x%x\n" , h.key_len);
//    printf("\tflow_len     : 0x%x\n" , h.flow_len);
//    printf("\tflowgram_fmt : 0x%x\n" , h.flowgram_format);
//    printf("\tflow         : %s\n  " , h.flow);
//    printf("\tkey          : %s\n  " , h.key);
//    printf("\n\n");

    if ( !strlen(fastq_file) ) {
        fastq_fp = stdout;
    }
    else {
        if ( (fastq_fp = fopen(fastq_file, "w")) == NULL ) {
            fprintf(stderr,
                    "[err] Could not open file '%s' for writing.\n",
                    fastq_file);
            exit(1);
        }
    }

    int left_clip = 0, right_clip = 0, nbases = 0;
    char *name;
    char *bases;
    uint8_t *quality;
    register int i;
    int numreads = (int) h.nreads;
    for (i = 0; i < numreads; i++) {
        read_sff_read_header(sff_fp, &rh);
        read_sff_read_data(sff_fp, &rd, h.flow_len, rh.nbases);

        /* get clipping points */
        get_clip_values(rh, trim_flag, &left_clip, &right_clip);
        nbases = right_clip - left_clip;

        /* create bases string */
        bases = get_read_bases(rd, left_clip, right_clip);

        /* create quality array */
        quality = get_read_quality_values(rd, left_clip, right_clip);

        /* create read name string */
        int name_length = (int) rh.name_len + 1; // account for NULL termination
        name = (char *) malloc( name_length * sizeof(char) );
        if (!name) {
            fprintf(stderr, "Out of memory! For read name string!\n");
            exit(1);
        }
        memset(name, '\0', (size_t) name_length);
        strncpy(name, rh.name, (size_t) rh.name_len);

        construct_fastq_entry(fastq_fp, name, bases, quality, nbases);

        free(name);
        free(bases);
        free(quality);
        free_sff_read_header(&rh);
        free_sff_read_data(&rd);
    }

    free_sff_common_header(&h);
    fclose(fastq_fp);
    fclose(sff_fp);
}

void
construct_fastq_entry(FILE *fp,
                           char *name,
                           char *bases,
                           uint8_t *quality,
                           int nbases) {
    register int j;
    uint8_t quality_char;

    /* print out the name/sequence blocks */
    fprintf(fp, "@%s\n%s\n+%s\n", name, bases, name);

//    /* print out quality values (as integer) */
//    for (j = 0; j < nbases; j++) {
//        printf("%d ", (int) rd.quality[j] );
//    }
//    printf("\n");

    /* print out quality values (as characters)
     * formula taken from http://maq.sourceforge.net/fastq.shtml
     */
    for (j = 0; j < nbases; j++) {
        quality_char = (quality[j] <= 93 ? quality[j] : 93) + 33;
        fprintf(fp, "%c", (char) quality_char );
    }
    fprintf(fp, "\n");
}
