#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "misc.h"
#include "hashdb.h"

/*
 * split_input
 * 
 * usage:   input buffer csv format, builds table of pointers to each column
 * assumes: input buffer has at least one char in it (not counting the '\0')
 * returns:  0 if all the columns were found,  -1 otherwise
 */
int split_input(char *buf, char delim, int cnt, char **table, unsigned long lineno,
            char **argv){

	int colCount = 0, badField = 0, ignoreQ = 0;
	for( int i = 0; i < cnt; i++){
		ignoreQ = 0;
		*(table + i) = buf;
		if(*buf == '"'){
			ignoreQ = 1;
			while(*buf != '\0'){
				if(((*buf == delim) || *buf == '\n') && !ignoreQ){
					colCount++;
					*buf++ = '\0';
					break;
				}
				buf++;
				if(*buf == '"'){
					ignoreQ = 0;
				}
			}
		}else{
			while(*buf != '\0'){
				colCount++;
				*buf++ = '\0';
				break;
			}
			buf++;
		}

	}
	if(ignoreQ){
		badField = 1;
	}

	if(colCount < cnt){
		dropmsg("too few columns", lineno, argv);
		return -1;
	}else if(colCount > cnt){
		dropmsg("too many columns", lineno, argv);
	}else if(badField){
		return -1;
	}
	
	// no errors 
	return 0;
}
