SYNOPSIS
========

The program `sff2fastq` extracts read information from a SFF file,
produced by the 454 genome sequencer, and outputs the sequences and
quality scores in a FASTQ format.

USAGE
=====

Given an SFF file, `file.sff` you can simply run:

    sff2fastq file.sff

`sff2fastq` will also read from standard input if a SFF file is not specified on the command line.  For example, you can do the following:

    cat file.sff | sff2fastq > file.fastq

This is useful if you have a compressed sff file and you'd like to avoid
creating temporary files like so:

    zcat file.sff.gz | sff2fastq > file.fastq

_(This feature has been kindly added by [Björn Winckler](https://github.com/b4winckler) )_

Options
-------

Below is the help message (via `sff2fastq -h`) describing its usage & options:

    Usage: sff2fastq [options] [sff_file]
            -h                  This help message   
            -v                  Program and version information
            -n                  Output the untrimmed sequence
            -o <fastq_file>     Desired fastq output file. If not specified, 
                                defaults to stdout


INSTALLATION
============

The installation process currently consists of a very simple Makefile.

Just do the following:

    git clone git://github.com/indraniel/sff2fastq.git;
    cd sff2fastq;
    make; # try 'make genome' if at the Genome Center at Washington University
          # or on a Linux distribution from 2008 or earlier

The `sff2fastq` executable should be in the working directory.
Afterwards, you can move the executable to wherever you wish.

NOTES
=====

This has been successfully compiled on Linux/Ubuntu 8.04 & 9.10
workstations (both 32-bit and 64-bit machines), and on Mac OS X (version
10.5).  Compiling on other types of operating systems and architectures
has not been experimented upon.

The FASTQ output produced is of the Sanger FASTQ format as described
here (http://maq.sourceforge.net/fastq.shtml).

Without any given options the default approach is to output trimmed
sequence and quality values.  This is similar in nature to the sff tools
produced by 454 Life Sciences/Roche.

AUTHORS
=======

Indraniel Das (indraniel@gmail.com or idas@wustl.edu)
The Genome Institute at Washington University

Contributors
------------

* [Björn Winckler](https://github.com/b4winckler)
* [James Casbon](https://github.com/jamescasbon)

ACKNOWLEDGEMENTS
================

This software was developed at The Genome Institute at Washington
University, St. Louis, MO.

DISCLAIMER
==========

This software is provided "as is" without warranty of any kind.


March 23, 2010
