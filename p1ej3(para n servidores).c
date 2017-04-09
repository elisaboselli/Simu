/* External definitions for single-server queueing system. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lcgrand.h"  /* Header file for random-number generator. */

#define SERVERS 2	 /*Number of servers.*/
#define Q_LIMIT 100  /* Limit on queue length. */
#define ENDTIME	300	 /*Limit time (if there are clients in queue, they will be attended)*/


void  initialize(void);
void  timing(void);
void  arrive(void);
void  depart(void);
void  report(void);
void  updateTimeAvgStats(void);
float expon(float mean);
float getMinDeparture(void);
void departureIn(float);
void departureOut(void);
float generateNextInterarrive(void);
float generateNextDeparture(void);
void debug(void);

float timeSinceLastEvent;
float timeNextEvent[3];
float nextDepartures[SERVERS];
float simulationTime;
float timeLastEvent;
float gain;
float totalGain;
float lastQueueEvent;
float clientsTime[Q_LIMIT][5]; //clientsTime[clients][times(arrive,departureOfQueue,departure,timeInQueue,totalTime)]
float averageQueueLength;
float averageServerUse;

int serverStatus;
int elementsInQueue;
int totalElementsInQueue;
int elementsTimeInQueue[Q_LIMIT];
int nextEventType;
int totalClients;
int currentClient;
int usedServers;


int main()  /* Main function. */{

    /* Initialize the simulation. */
	printf("INITIALIZE \n");
    initialize();

	while (simulationTime <= ENDTIME){
		printf("TIMING \n");
		timing();

		switch (nextEventType) {
			case 1:
				printf("NEXT EVENT (ARRIVE) \n");
				arrive();
				break;
			case 2:
				printf("NEXT EVENT (DEPARTURE) \n");
				depart();
				break;
		}
		//printf("\n	DEBUG\n");
		//debug();
    }
    
    while(currentClient<=totalClients){
		
		if(currentClient==totalClients){
			printf("Last client departure\n");
		}
		
		else{
			printf("Client in queue departure\n");
			depart();
		}
		
		currentClient++;
	}
	
	printf("\n\nREPORT\n\n");
    report();

    return 0;
}


void initialize(void){

    simulationTime = 0.0;

    usedServers=0;
    elementsInQueue  = 0;
    totalElementsInQueue = 0;
    timeLastEvent = 0.0;
    lastQueueEvent = 0.0;
    totalGain = 0.0;
    currentClient=0;
    totalClients=0;

	averageQueueLength = 0.0;
	averageServerUse = 0.0;
     
    timeNextEvent[1] = simulationTime + generateNextInterarrive();

    int i=0;
	while(i<SERVERS){
		nextDepartures[i]=1.0e+30;
		i++;
	}
	
	debug();
}


void timing(void){

    nextEventType = 0;
    timeLastEvent = simulationTime;
    
    if (timeNextEvent[1]<=nextDepartures[0] && timeNextEvent[1]<=ENDTIME){
		nextEventType = 1;
		simulationTime = timeNextEvent[1];
		totalClients++;
	}
    else{
		nextEventType = 2;
		simulationTime = nextDepartures[0];
	}
	
	debug();
	printf("	Current client: %d\n",currentClient);
	printf("	Total client: %d\n",totalClients);
	printf("	Queue length: %d\n",elementsInQueue);
	printf("	Free servers: %d\n",SERVERS-usedServers);
	printf("	Next event type %d \n",nextEventType);

}


void arrive(void){
    
    clientsTime[totalClients][0]= timeNextEvent[1];
    
    printf("	client (%d) arrive %.2f\n",totalClients,clientsTime[totalClients][0]);
	printf("	NEXT ARRIVE GENERATION\n");
	
    timeNextEvent[1] = simulationTime + generateNextInterarrive() ;
    
    printf("	time next arrive: %.2f\n",timeNextEvent[1]);
    
    if (usedServers==SERVERS) {
		
		averageQueueLength += elementsInQueue*(simulationTime-lastQueueEvent);
		
		lastQueueEvent=simulationTime;
		elementsInQueue++;
		totalElementsInQueue++;
		printf("	Sent to queue. Queue length %d\n",elementsInQueue);
		
		printf("	Time next arrive : %.2f (%.2f)\n",timeNextEvent[1],timeNextEvent[1]-simulationTime);
		printf("	Time next departure : %.2f (%.2f)\n",nextDepartures[0], nextDepartures[0]-simulationTime);
		
        if (elementsInQueue > Q_LIMIT) {
            exit(2);
        }
        
		totalGain = totalGain + gain;
    }

    else {
		averageServerUse += usedServers*(simulationTime - timeLastEvent);
		
		float auxDeparture;
		currentClient++;
		clientsTime[totalClients][1]=simulationTime;

		printf("	NEXT DEPARTURE GENERATION\n");
        auxDeparture=simulationTime + generateNextDeparture();
		//printf(" 	DEPARTURE IN: \n");
		departureIn(auxDeparture);
		
        clientsTime[totalClients][2]=auxDeparture;

        //printf("	Departure genetated: %f\n",auxDeparture);
		usedServers++;

        //printf("	client (%d) departure %.2f\n",currentClient,clientsTime[currentClient][2]);
        //debug();
		
		totalGain = totalGain + gain;		
    }
}


void depart(void){
    if (elementsInQueue == 0) {
		averageServerUse += usedServers * (simulationTime-timeLastEvent);

        //printf(" 	DEPARTURE OUT: \n");
		departureOut();
		usedServers--;
		printf("	Empty queue\n");
    }
    else {
		averageQueueLength += elementsInQueue*(simulationTime-lastQueueEvent);
		
		lastQueueEvent=simulationTime;
        elementsInQueue--;
        clientsTime[currentClient][2]=simulationTime;
        
        printf("	client (%d) departure %.2f\n",currentClient,clientsTime[currentClient][2]);
        currentClient++;
        clientsTime[currentClient][1]=simulationTime;
        
        printf("	client (%d) outOfQueue %.2f\n",currentClient,clientsTime[currentClient][1]);
		printf("	Out of queue. Queue length: %d\n",elementsInQueue);

        //printf(" 	DEPARTURE OUT: \n");
        departureOut();
        
		printf("	NEXT DEPARTURE GENERATION\n");
		float auxDeparture = simulationTime+generateNextDeparture();
		
		//printf(" 	DEPARTURE IN: \n");
        departureIn(auxDeparture);
        
        clientsTime[currentClient][2]=auxDeparture;
        
        //printf("	client (%d) departure %.2f\n",currentClient,clientsTime[currentClient][2]);
        //debug();
    }
}

void report(void){
	
	float avTimeInTrade=0.0;
	float avTimeInQueue=0.0;
	float serverUtilizationTime = 0.0;
	
	printf("Simulation total time : %.2f min.\n",simulationTime);
	printf("Total clients : %d\n",totalClients);
	printf("Gain: $ %.2f\n",totalGain);
	
	//printf("Clients information\n");
	int i=1;
	while (i<=totalClients){
		clientsTime[i][3]= clientsTime[i][1] - clientsTime[i][0];
		clientsTime[i][4]= clientsTime[i][2] - clientsTime[i][0];
		//printf("Client %d: Arrive: %.2f, outOfQueue: %.2f, departure: %.2f,\n timeInQueue: %.2f, totalTime: %.2f\n",i,clientsTime[i][0],clientsTime[i][1],clientsTime[i][2],clientsTime[i][3],clientsTime[i][4]);
		avTimeInQueue = avTimeInQueue + clientsTime[i][3];
		avTimeInTrade = avTimeInTrade + clientsTime[i][4];
		serverUtilizationTime = serverUtilizationTime + (clientsTime[i][2] - clientsTime[i][1]);
		i++;
	}
	
	//Average delay in trade
	avTimeInTrade = avTimeInTrade/totalClients;
	printf("Average delay in trade: %.2f min.\n",avTimeInTrade);
	
    //Average delay in queue
    if (totalElementsInQueue>0){
		avTimeInQueue = avTimeInQueue/totalElementsInQueue;
	}
	printf("Average delay in queue: %.2f min.\n",avTimeInQueue);

    //Average number in queue
	averageQueueLength = averageQueueLength/simulationTime;
	printf("Average queue legth: %.2f\n", averageQueueLength);
	
    //Server utilization
	printf("Server utilization: %.2f percent\n",averageServerUse/(simulationTime*SERVERS)*100);

}

void departureIn(float dep){
	int i = 0;
	int j = SERVERS-1;
	while (nextDepartures[i]<dep){
		i++;
	}
	while (i<j){
		nextDepartures[j]=nextDepartures[j-1];
		j--;
	}
	nextDepartures[i]=dep;
	i = 0;
	while (i<SERVERS){
		//printf("			%.2f\n ",nextDepartures[i]);
		i++;
	}
}

void departureOut(){
	int i=0;
	//printf("DEPARTURE\n");
	while(i+1<SERVERS){
		nextDepartures[i]=nextDepartures[i+1];
		i++;
	}
	nextDepartures[i] = 1.0e+30;
}
		
float generateNextInterarrive()  {
    float rn = lcgrand(7);
	if (rn >= 0.0 && rn<0.05){
			return 5;
	}
	if (rn >= 0.05 && rn<0.1){
			return 10;
	}
	if (rn >= 0.1 && rn<0.2){
			return 15;
	}
	if (rn >= 0.2 && rn<0.3){
			return 20;
	}
	if (rn >= 0.3 && rn<0.6){
			return 25;
	}
	if (rn >= 0.6 && rn<0.8){
			return 30;
	}
	if (rn >= 0.8 && rn<0.95){
			return 35;
	}
	return 40;
}

float  generateNextDeparture(){
    float rn = lcgrand(3);
	if (rn >= 0.0 && rn<0.15){
			gain = 5000;
			return 10;
	}
	if (rn >= 0.15 && rn<0.4){
			gain = 7000;
			return 20;
	}
	if (rn >= 0.4 && rn<0.8){
			gain = 9000;
			return 30;
	}
	gain=11000;
	return 40;
}

void debug(){
	printf("	Sistem Time : %.2f \n",simulationTime);
	printf("	Time next arrive : %.2f \n",timeNextEvent[1]);
	printf("	Time next departures : ");
	int i=0;
	while(i<SERVERS-1){
		if(nextDepartures[i]==1000000015047466219876688855040.00){
			printf("[-] - ");
		}
		else{
			printf("%.2f - ",nextDepartures[i]);
		}
		i++;
	}
	if(nextDepartures[i]==1000000015047466219876688855040.00){
		printf("[-]\n");
	}
	else{
		printf("%.2f\n",nextDepartures[i]);
	}
}
