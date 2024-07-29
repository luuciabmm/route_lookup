#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define IP_ADDRESS_LENGTH 32
#define MAX_LENGTH_TABLE 16777216

/********************************************************************
 * Generate a netmask of length prefixLength
 ********************************************************************/
void getNetmask (int prefixLength, int *netmask);


struct param{
    short* tbl24;    //Memory 1
    short* tbllong;	//Memory 2
    unsigned short net_long;    
    long ip_content;                   
};

void getNetmask (int prefixLength, int *netmask);

void param_init(struct param* parametros);

void paramFree(struct param* parametros);

void routeLookup(struct param* parametros, unsigned short *interface, unsigned int *address_search, short int *n_accesses );

void newRoute(struct param* parametros,  int* out_interface, int* prefixLength, unsigned int* address);

#endif

//RL Lab 2020 Switching UC3M
