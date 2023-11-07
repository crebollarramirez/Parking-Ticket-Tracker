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
 *  Inputs:
 *  hashtab pointer to hashtable (pointer to an array of pointers)
 *  tabsz   number of elements in the hash table
 *  fineTab table mapping fine code to description and fine   
 *  plate   plate id string to be found
 *  state   state id string to be found
 *  summ    summon id string to be found
 *  argv    for error printing
 *

 * This will delete a ticket specified. If the vehicle
 * does not have any more tickets it will be deleted. 
 * This will return a -1 if ticket was not found or there was an error. 
 * Returns 0 if there was no errors.
 */
int del_ticket(struct vehicle **hashtab, uint32_t tabsz, struct fine *fineTab,
    char *plate, char *state, char *summ, char **argv)
{
	
	
	struct vehicle *ptr = vehicle_lookup(hashtab, tabsz, plate, state, argv);
	if(ptr == NULL){
		return -1; // if the vehicle does not exist, return -1
	}

	// we found the vehicle now we want to delete ticket
	
	// we will convert summ from string to long long int using strtosumid, if the 
	// conversion doesn't work we return -1
	unsigned long long summid;
	if(strtosumid(summ, &summid, argv) != 0){
		return -1;
	}
	// now we are searching the ticket chain
	struct ticket *tptr = ptr->head;
	struct ticket *prev = ptr->head;
	while(tptr != NULL){
		if(tptr->summons == summid){ // we found the ticket we want to remove
			break;
		}
		prev = tptr;
		tptr = tptr->next;
	}
	
	if(tptr == NULL){
		return -1;
	}
	

	ptr->tot_fine -= (fineTab + tptr->code)->fine;
	
	if(tptr == ptr->head){
		ptr->head = tptr->next;
	}else{
		prev->next = tptr->next;
	}
	ptr->cnt_ticket--;
	free(tptr);

	
	// removing the vehicle if there is no more tickets
	if(ptr->head == NULL){
		uint32_t index = hash(plate, argv) % tabsz;
		struct vehicle *vpt = *(hashtab+index);
		struct vehicle *prev = *(hashtab+index);
		
		while(vpt != ptr){	
			prev = vpt;
			vpt = vpt->next;
		}
		if(ptr == *(hashtab+index)){
			*(hashtab+index) = ptr->next;	
		}else{
			prev->next = ptr->next;

		}
		free(ptr->state);
		free(ptr->plate);
		free(ptr);
	}
	return 0;
}
