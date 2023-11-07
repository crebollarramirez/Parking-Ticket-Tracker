#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "hashdb.h"

//uncomment the next line when you want to use your routine
#define MYCODE
#ifndef MYCODE
TODO(USING THE SOLUTION FUNCTION NOT MY CODE)
#else

/*
 * vehicle_lookup
 *          look for vehicle in the database
 *          vehicle must match both plate and state strings
 * args
 *  hashtab pointer to hashtable (pointer to an array of pointers)
 *  tabsz   number of elements in the hash table
 *  plate   plate id string to be found
 *  state   state id string to be found
 *  argv    for error printing
 *
 * returns  pointer to vehicle if found NULL otherwise
 */


/*
 * 	This will return a vehicle if found, will return NULL if vehicle not found.
 */
struct vehicle *
vehicle_lookup(struct vehicle **hashtab, uint32_t tabsz, char *plate,
    char *state, char **argv)
{ 
	uint32_t index = hash(plate, argv) % tabsz; // getting the index for hashtable
	struct vehicle *ptr = *(hashtab+index); // vehicle pointer

	while(ptr != NULL){ // loop to look for vechicle, if not found, we will return NULL
		if ((strcmp(ptr->plate, plate) == 0) && (strcmp(ptr->state, state) == 0)){
			return  ptr; // we found the vehicle so we return the vehicle
		}
		ptr = ptr->next;
	}
	return NULL;
}
#endif
