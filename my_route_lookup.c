#include "utils.h"
#include "io.h"
#include <time.h>

int main(int argc, char *argv[])
{
    struct param parametros; 

    //routing table parameters
    unsigned int *IPaddress;
    int *OutIfc;
    int *prefixLength;
	int FIBline = 0;
	int packetline = 0;
    int error = 0; 

    //var for the final and initial time and mean time of each packet
    //var for total num of proccessed packets, and the mean access  
    struct timespec initial_time, final_time;
    int *packets_processed; 
    double *totalTableAccesses;
    double *totalPacketProcessingTime;

    int clock_getres(clockid_t clk_id, struct timespec *res);

    int clock_gettime(clockid_t clk_id, struct timespec *tp);

    int clock_settime(clockid_t clk_id, const struct timespec *tp);

    //space in memory for data and packets 
    param_init(&parametros);
    packets_processed = (int*)calloc(1, sizeof(int));
    

    //space for accesses and mean time of packets 
    totalTableAccesses = (double*)calloc(1, sizeof(double));

    totalPacketProcessingTime = (double*)calloc(1, sizeof(double));
  
    //enter the files and possible errors 
    if(argc == 3){
        error = initializeIO(argv[1], argv[2]); //init file descriptors
        if(error != 0){
            perror("\nerror: "); 
            printIOExplanationError(error);
			free(packets_processed); 
            free(totalTableAccesses); 
            free(totalPacketProcessingTime);  
            return 0;   
        }
        //memory for IP direction 
        IPaddress = (unsigned int*)calloc(1, sizeof(int));
        
        //memory for prefix
        prefixLength = (int*)calloc(1, sizeof(int));
     
        //memory for interface
        OutIfc =(int*)calloc(1,sizeof(int));
       
        //read routing files 
        FIBline = readFIBLine (IPaddress, prefixLength, OutIfc );
        while(FIBline == 0){
            //with newRoute we introduce the new directions that come from routing files 
            newRoute(&parametros, OutIfc, prefixLength, IPaddress );
            FIBline = readFIBLine(IPaddress, prefixLength, OutIfc);
        }
        //memory for lookup of ip
        unsigned int *ipaddress_search = (unsigned int*)calloc (1, sizeof(unsigned int));
        
        //memory for interface 
        unsigned short *interface = (unsigned short*)calloc(1, sizeof(unsigned short));
        
        //memory for time 
        double *time = (double*)calloc(1, sizeof(double));
      
        //memory for tables
        short int *tables = (short int*) calloc (1, sizeof(short int));
       

        //read what we have 
        packetline = readInputPacketFileLine(ipaddress_search);
        while (packetline == 0){
            //take initTime
            clock_gettime(CLOCK_MONOTONIC_RAW, &initial_time); 
            //we search in the tables, and take the output interface
            routeLookup(&parametros, interface, ipaddress_search, tables); 
             //take final time and calculate the total time
            clock_gettime(CLOCK_MONOTONIC_RAW, &final_time);
             //+1 to the processed directions
            *packets_processed = *packets_processed + 1;
             //we sum the numer of accesses to the table 
            *totalTableAccesses = *totalTableAccesses + *tables;
            //we sum the computed time to the processing time
            *totalPacketProcessingTime = *totalPacketProcessingTime + *time; 

            //Terminal solution
            printOutputLine(*ipaddress_search, *interface, &initial_time, &final_time, time, *tables); 
            packetline = readInputPacketFileLine(ipaddress_search);
        }

        //memory for mean accesses to the tables
        double *averageTableAccesses = (double*)calloc(1, sizeof(double));
       //computation of the time
        *averageTableAccesses = *totalTableAccesses / *packets_processed;

        //memory for mean packets processing time
        double *averagePacketProcessingTime = (double* ) calloc(1, sizeof(double));
        //computation of the time
        *averagePacketProcessingTime = *totalPacketProcessingTime/ *packets_processed;
            
        printSummary(*packets_processed, *averageTableAccesses, *averagePacketProcessingTime);

            //free all the memory used
            freeIO();
            paramFree(&parametros);
            free(ipaddress_search);
			free(tables);
			free(time);
            free(OutIfc);
            free(totalPacketProcessingTime);
            free(totalTableAccesses);
            free(IPaddress);
			free(prefixLength);
            free(interface);
            free(averagePacketProcessingTime);
            free(averageTableAccesses);
            free(packets_processed);
     

         //possible error of not right arguments
        }  else { 
        printf("\nerror, please, try the following: \n\t./my_route_lookup FIB InputPacketFile\n");
        paramFree(&parametros); 
        free(totalTableAccesses); 
        free(packets_processed); 
        free(totalPacketProcessingTime);
        return 0;    
    }
    return 1;  

}