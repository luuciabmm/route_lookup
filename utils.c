#include "utils.h"

#define IP_ADDRESS_LENGTH 32
#define MAX_LENGTH_TABLE 16777216 //2^24

/********************************************************************
 * Two Level Multibit Trie
 ********************************************************************/
void getNetmask(int prefixLength, int *netmask){
	
	*netmask = (0xFFFFFFFF << (IP_ADDRESS_LENGTH - prefixLength));
}

// Initialize tbl24 memory 
void param_init(struct param* param){
    param-> tbl24 = (short *)calloc(MAX_LENGTH_TABLE, sizeof(short));
    if(param-> tbl24 == NULL){
        perror("ERROR: Failure allocating memory\n");
        return;
    }
    param-> tbllong = NULL;         
    param-> ip_content = 0;
    param-> net_long = 0;

}

void routeLookup(struct param* parametros, unsigned short *interface, unsigned int *ipaddress_search, short int *n_accesses) {
    *interface = parametros->tbl24[*ipaddress_search>>8];    //Take the header bit
    if(*interface>>15 == 0)	{
        *n_accesses = 1;
        return;
    } else {              //If it is a 1 we need 2 memory accesses and check 2nd table
        *n_accesses = 2;
        *interface = parametros->tbllong[(*ipaddress_search & 0x000000FF) + (*interface & 0x7FFF)*256];      
        return;
    }
}

void newRoute(struct param* parametros,  int* OutIfc, int* prefixLength, unsigned int* IPaddress){

    //short diff_mask_long = 32 - *prefixLength;
    //short diff_mask = 24 - *prefixLength;
    long entries = 1;                               
    unsigned int i;

   
    if (*prefixLength <= 24){
        // 2^entries
        for (i = 0; i < 24-*prefixLength; ++i){
            entries *= 2;              
        }

        for (parametros->ip_content = 0; parametros->ip_content < entries; parametros->ip_content++){
            parametros->tbl24[(*IPaddress >>8) + parametros->ip_content] = *OutIfc;
        }
    } else {         
        for (i = 0; i < 32-*prefixLength; ++i){  
            entries *= 2;
        }
        
	// If the header bit is 0, there is no interface so we create one.
        if(parametros->tbl24[*IPaddress>>8]>>15 == 0){
            parametros->tbllong = realloc(parametros->tbllong, 256 * 2 * (parametros->net_long + 1)); //Reserve 256 new positions
	    
            for (parametros->ip_content = 0; parametros->ip_content <= 255; parametros->ip_content++) {
                parametros->tbllong[256 * parametros->net_long + parametros->ip_content] = parametros->tbl24[ *IPaddress>>8 ];
            }
            
            // We change header to 1 to signal there is an interface now and create interface
            parametros->tbl24[*IPaddress>>8] = parametros->net_long | 0b1000000000000000; 
            for(parametros->ip_content = (*IPaddress& 0x00FF); parametros->ip_content < entries + (*IPaddress & 0x00FF); parametros->ip_content++){
                parametros->tbl24[(parametros->net_long * 256) + parametros->ip_content] = *OutIfc;
            }

            parametros->net_long++;
        } else {
            // Save directly the interface in tbllong
            for(parametros->ip_content = (*IPaddress & 0x00FF); parametros->ip_content < entries + (*IPaddress & 0x00FF); parametros->ip_content++){
                parametros->tbllong[(parametros->tbl24[*IPaddress>>8] & 0x7FFF)*256 + parametros->ip_content] = *OutIfc;
            }
        }
    }
}

 
// Free memory of both tables
void paramFree(struct param* parametros) {
    if (parametros != NULL) {
        free(parametros->tbl24);
        free(parametros->tbllong);
    }
}

//RL Lab 2020 Switching UC3M
