/*
$Header: svn+ssh://unreal/var/svn/public/pipeflow/trunk/src/pipeflow.c 5 2011-09-16 10:04:51Z beli $

Pipe Flow Meter
pipeflow.c - main code file

This file is part of Pipe Flow Meter project (short 'pipeflow')

Copyright (c) 2011, Michal Belica (http://www.beli.sk/)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// for clock_gettime() from time.h
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include <config.h>

#define BUFFSIZE 512
#define LINEMARK "PipeFlow: "

int finish;
char version[] = VERSION;

float get_time() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
		// TODO return value
		// TODO check clock availability
	return (float)ts.tv_sec + ((float)ts.tv_nsec) / 1000000000;
}

void print_num( FILE *f, float n ) {
	if( n > 1024L*1024*1024*1024 ) {
		fprintf( f, "%.2f Ti", n / 1024/1024/1024/1024 );
	} else if( n > 1024*1024*1024 ) {
		fprintf( f, "%.2f Gi", n / 1024/1024/1024 );
	} else if( n > 1024*1024 ){
		fprintf( f, "%.2f Mi", n / 1024/1024 );
	} else if( n > 1024 ){
		fprintf( f, "%.2f Ki", n / 1024 );
	} else {
		fprintf( f, "%.0f ", n );
	}
}

void calculate( float period_start, float now, long count, long totalcount ) {
	float bps;
	bps = (float)count / ( now - period_start );

	//print
	print_num(stderr, totalcount);
	fputs("Bytes at ", stderr);
	print_num(stderr, bps);
	fputs("Bytes/s\n", stderr);
}

void signal_handler( int signal ) {
	if(signal == SIGINT) {
		finish = 1;
	}
}

int main( int ac, char *av[] ) {
	int readsize, writesize, interval = 3;
	long count, totalcount;
	char *buff, *str;
	float period_start, now, starttime;
	struct sigaction sa;

	finish = 0;

	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);

	buff = malloc( BUFFSIZE );
		// TODO return value
	str = malloc(64);
		// TODO return value
	starttime = get_time();
	period_start = starttime;
	totalcount = 0;
	count = 0;
	while( !finish && !feof( stdin ) && !ferror(stdin) ) {
		readsize = fread( buff, 1, BUFFSIZE, stdin );
		if( readsize ) {
			writesize = fwrite( buff, 1, readsize, stdout );

			// calculations
			count += writesize;
			now = get_time();
			if( now > period_start + interval ) {
				totalcount += count;
				fputs(LINEMARK, stderr);
				calculate(period_start, now, count, totalcount);

				// restart counts
				period_start = now;
				count = 0;
			}
			// end calculations

			if( writesize < readsize ) {
				if( ferror(stdout) ) {
					fprintf( stderr, "%serror writing to stdout: %s\n",
							LINEMARK, strerror(errno) );
					break;
				} else if( feof(stdout) ) {
					fprintf( stderr, "%seof on stdout\n", LINEMARK );
					break;
				}
			}
		}
	}
	if( ferror(stdin) ) {
		fprintf( stderr, "%serror reading stdin: %s\n",
				LINEMARK, strerror(errno) );
	}
	//print
	totalcount += count;
	now = get_time();
	fputs(LINEMARK, stderr);
	fputs("Total ", stderr);
	calculate(starttime, now, totalcount, totalcount);
	return 0;
}
