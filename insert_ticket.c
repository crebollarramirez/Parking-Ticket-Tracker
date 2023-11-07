#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "hashdb.h"
#include "readsubs.h"

/*args:
 *  hashtab pointer to hashtable (pointer to an array of pointers)
 *  tabsz   number of elements in the hash table
 *          example hashval = hash(plate, argv) % tabsz;
 *  fineTab pointer fine table. maps code number to text description & fine cost 
 *          used to get the amount of the fine when calculating
 *          the tot_fine field in struct vehicle.
 *          example:
 *              struct vehicle *vhpt;
 *              vhpt->tot_fine += fineTab[code].fine;
 *  summ    summons id string to be added
 *          this string needs to be converted to a number to be stored in the
 *          database. This saves space and is faster to compare than a string.         
 *  plate   plate id string to be added
 *  state   state id string to be added
 *  date    date of summons string
 *          this string needs to be converted to a number to be stored in the
 *          database. This saves space and is faster to compare than a string
 *          The encoding into a number uses Linux time format.
 *  code    summons code integer value used an an index into the fines table
 *  argv    for error printing
 *
 * returns 0 if ok -1 for all errors
 */


/*
 *	This will add a ticket to a vehicle that exists, or it will create a new vehicle and add the ticket
 *	there if the vehicle doesn't exist. This will return a 0 if everything goes well, if there is any
 *	errors, a -1 will be returned
 */
int insert_ticket(struct vehicle **hashtab, uint32_t tabsz, struct fine *fineTab, char *summ,
            char *plate, char *state, char *date, int code, char **argv)
{
	long long unsigned int summid;
	time_t dateval;
	if(strtosumid(summ, &summid, argv) != 0){
		return -1;
	
	}
	if(strtoDate(date, &dateval, argv) != 0){
		return -1;
	}


	// look if vehicle exsist, if not them make a new vechicle nodes
	uint32_t index = hash(plate, argv) % tabsz;
	struct vehicle *v = vehicle_lookup(hashtab, tabsz, plate, state, argv);
	struct ticket *tptr = malloc(sizeof(*tptr));
	if(tptr == NULL){return -1;}
	if(v == NULL){ // if the vehicle doesn't exists, we will create a new vehicle
		struct vehicle *vptr = malloc(sizeof(*vptr));
		if(vptr == NULL){
			free(tptr);
			return -1;
		}
		
		// adding vehicle information and setting the vehicle pointers 
		vptr->state = strdup(state);
		vptr->plate = strdup(plate);
		vptr->tot_fine = fineTab[code].fine;
		vptr->cnt_ticket = 1;
		vptr->next = *(hashtab+index);
		*(hashtab+index) = vptr;
	
		// adding information to the ticket and adding it to the vehicle
		tptr->summons = summid;
		tptr->code = code;
		tptr->date = dateval;
		tptr->next = NULL;	
		vptr->head = tptr;

	}else{ // if the vehicle does exist
		struct ticket *travel = v->head;
		struct ticket *prev = v->head;
		
		tptr->summons = summid;
		v->tot_fine += (fineTab+code)->fine;
		tptr->code = code;
		tptr->date = dateval;

		while(travel != NULL){
			prev = travel;
			travel = travel->next;
		}
		
		if(prev == NULL){
			v->head = tptr;
		}else{
			prev->next = tptr;
			tptr->next =  NULL;
			}
		v->cnt_ticket++;
		}	
	return 0;
}
