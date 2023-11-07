#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "hashdb.h"
#include "commands.h"
#include "readsubs.h"

static void find_cmd(char *, struct vehicle **, uint32_t, struct fine *,
    char **);
static void pay_cmd(char *, struct vehicle **, uint32_t, struct fine *,
    char **);
static void insert_cmd(char *, struct vehicle **, uint32_t, struct fine *,
    char **);
static void chain_cmd(char *, struct vehicle **, uint32_t, struct fine *,
    char **);
/*
 * simple command line interface just to test code
 * not a typical good interface
 */
void commands(struct vehicle **htable, uint32_t tabsz, struct fine *fineTab,
        char **argv, int silent){
    char *buf = NULL;       /* input buffer, getline() will allocate it */
    size_t bufsz = 0;       /* size of buffer altered by getline()*/

    /*
     * loop through the rest of the codes
     */
    if (!silent)
        printf("Input command: ");
    while (getline(&buf, &bufsz, stdin) > 0) {
        if (silent)
            printf("Command: %s\n", buf);
        switch(*buf) {
        case 'C':
        case 'c':
            chain_cmd(buf, htable, tabsz, fineTab, argv);
            break;
        case 'D':
        case 'd':
            dump_db(htable, tabsz, fineTab, argv);
            break;
        case 'F':
        case 'f':
            find_cmd(buf, htable, tabsz, fineTab, argv);
            break;
        case 'I':
        case 'i':
            insert_cmd(buf, htable, tabsz, fineTab, argv);
            break;
        case 'P':
        case 'p':
            pay_cmd(buf, htable, tabsz, fineTab, argv);
            break;
        case 'L':
        case 'l':
            largest(htable, tabsz);
            break;
        case 'V':
        case 'v':
            verify_db(htable, tabsz, fineTab);
            break;
        case 'Q':
        case 'q':
            free(buf);
            return;
        default:
            if (silent)
                break;
            printf("Unknown command %c\n", *buf);
            printf("Commands (PLATE and STATE must be in all CAPS):\n");
            printf("\tF PLATE STATE                   Find tickets for vehicle\n");
            printf("\tP PLATE STATE summons_number    Pay ticket for vehicle\n");
            printf("\tI SUMMONS PLATE STATE DATE CODE Insert a summons\n");
            printf("\tI PLATE STATE summons_number    Pay ticket for vehicle\n");
            printf("\tL                               Print largest fines and tickets\n");
            printf("\tV                               Verify total ticket and total fines\n");
            printf("\tD                               Dump (print) entire database\n");
            printf("\tC chain_index                   Chain print a single hash chain\n");
            printf("\tQ                               Quit\n");
            break;
        }
        if (!silent)
            printf("Input command: ");
    }
    free(buf);
    return;
}

/*
 * find_cmd
 *          finds and prints all tickets for specified vehicle
 * input    plate and state from stdim
 * output   vehicle summary plus all tickets unpaid
 */
void find_cmd(char *buf, struct vehicle **htable, uint32_t tabsz,
        struct fine *fineTab, char **argv)
{
    char cmd[3];
    char plate[12];         /* wired in limits for sscanf safety */
    char state[3];          /* wired in limits for sscanf safety */
    struct vehicle *found;

    if (sscanf(buf,"%1s %11s %2s", cmd, plate, state) != 3) {
        printf("Useage: F PLATE STATE\n");
        return;
    }

    found = vehicle_lookup(htable, tabsz, plate, state, argv);
    if (found == NULL) {
        printf("   Plate: %s, State: %s \n", plate, state);
        printf("      No tickets\n");
        return;
    }
    (void)print_vehicle(found, fineTab, argv);
    return;
}

/*
 * pay_cmd
 *          pays a fine for a ticket (summons) specified
 *          will cause the ticket to be removed and if
 *          no tickets remain, remove the vehicle info
 * input    plate state and summos_is from stdin
 * output   summons paid or not
 */
void pay_cmd(char *buf, struct vehicle **htable, uint32_t tabsz,
        struct fine *fineTab, char **argv)
{
    char cmd[3];
    char plate[12];         /* wired in limits for sscanf safety */
    char state[3];          /* wired in limits for sscanf safety */
    char summ[14];          /* wired in limits for sscanf safety */

    if (sscanf(buf,"%1s  %11s %2s %13s", cmd, plate, state, summ) != 4) {
        printf("Useage: P PLATE STATE SUMMONS_NUMBER\n");
        return;
    }

    if (del_ticket(htable, tabsz, fineTab, plate, state, summ, argv) == 0)
        printf("Plate: %s, State %s, Summons %s paid\n", plate, state, summ);
    else
        printf("Plate: %s, State %s, Summons %s not found\n", plate, state,
             summ);
    return;
}

/*
 * insert_cmd
 *          inserts a summons into the database
 * input    summons,plate,state,date,code from stdin
 * output   none 
 */
void insert_cmd(char *buf, struct vehicle **htable, uint32_t tabsz,
        struct fine *fineTab, char **argv){
    char cmd[3];
    int code;
    char plate[12];         /* wired in limits for sscanf safety */
    char state[3];          /* wired in limits for sscanf safety */
    char summ[14];          /* wired in limits for sscanf safety */
    char date[12];          /* wired in limits for sscanf safety */
    char codstr[4];         /* wired in limits for sscanf safety */
    char *endptr;

    if (sscanf(buf,"%1s %13s %11s %2s %11s %3s", cmd, summ, plate, state, date,
            codstr) != 6) {
        printf("Useage: I  SUMMONS PLATE STATE DATE CODE\n");
        return;
    }

    /*
     * translate the code to an int
     */
    errno = 0;
    code = (int)strtoul(codstr, &endptr, 10);
    if ((errno != 0) || (*endptr != '\0') || (code >= CODES) || (code <= 0)) {
        printf("%s: bad code value\n", codstr);
        return;
    }

    if (insert_ticket(htable, tabsz, fineTab, summ, plate, state, date, code, argv) != 0)
        printf("Plate: %s, State %s, Summons %s insert failed\n", plate, state, summ);
    else
        printf("Plate: %s, State %s, Summons %s inserted\n", plate, state, summ);

    return;
}

/*
 * chain_cmd debug command
 *          prints out all the vehicles and tickets on a hash chain
 * input    hash chain number 
 * output   dump of all data on a hash chain
 */
void chain_cmd(char *buf, struct vehicle **htable, uint32_t tabsz,
        struct fine *fineTab, char **argv)
{
    uint32_t index;
    char *endptr;
    unsigned long tickets;
    unsigned long vehicles;

    errno = 0;
    index = (uint32_t)strtoul(buf+1, &endptr, 10);
    if (((*endptr != '\n') && (*endptr != '\0')) || (errno != 0) || (index >= tabsz)) {
        printf("C command: index bad value %u %s\n", index,endptr);
        printf("Useage: C index\n");
        return;
    }

    vehicles = dump_index(index, htable, tabsz, fineTab, &tickets, argv);
    printf("index: %u vehicles: %lu tickets: %lu\n", index, vehicles, tickets);
    return;
}
