#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "hashdb.h"

/*
 *	This will free up memory for all the tickets, vehicles, and for the hash table
 */
void free_tickets(struct vehicle **hashtab, uint32_t tabsz)
{
	unsigned long int cnt = 0UL;
	struct vehicle *ptr = *hashtab;
	struct vehicle *prevV = *hashtab;
	for(uint32_t i = 0; i < tabsz; i++){
		if(*(hashtab+i) != NULL){ // found a spot in the hashtab that has a vehicle
			ptr = *(hashtab+i);
			while(ptr != NULL){
				struct ticket *tptr = ptr->head;
				struct ticket *prev  = ptr->head;
				while(tptr != NULL){ // loop that will delete ticktes
					prev = tptr;
					tptr = tptr->next;
					free(prev);
					cnt++;
				} // this will delete the vehicle
				prevV = ptr;
				ptr = ptr->next;
				free(prevV->state);
				free(prevV->plate);
				free(prevV);
			}	
		}
	}
	free(hashtab);
	printf("Total tickets freed: %lu\n", cnt);
}
