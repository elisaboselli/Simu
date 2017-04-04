/* External definitions for single-server queueing system. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "lcgrand.h"  /* Header file for random-number generator. */

#define Q_LIMIT 100  /* Limit on queue length. */
#define BUSY      1  /* Mnemonics for server's being busy */
#define IDLE      0  /* and idle. */
#define ENDTIME	300


void  initialize(void);
void  timing(void);
void  arrive(void);
void  depart(void);
void  report(void);
void  updateTimeAvgStats(void);
float expon(float mean);
float generateNextInterarrive(void);
float generateNextDeparture(void);

float timeSinceLastEvent;
float timeNextEvent[3];
float simulationTime;
float timeLastEvent;
float gain;
float totalGain;
float lastQueueEvent;
float clientsTime[Q_LIMIT][5]; //clientsTime[clients][times(arrive,departureOfQueue,departure,timeInQueue,totalTime)]
float averageQueueLength;

int serverStatus;
int elementsInQueue;
int totalElementsInQueue;
int elementsTimeInQueue[Q_LIMIT];
int nextEventType;
int totalClients;
int currentClient;


int main()  /* Main function. */{

    /* Initialize the simulation. */
	printf("INITIALIZE \n");
    initialize();

    /* Run the simulation while more delays are still needed. */
    //while (num_custs_delayed < num_delays_required) {
	while (simulationTime <= ENDTIME){
		/* Determine the next event. */
		printf("TIMING \n");
		timing();

		/* Update time-average statistical accumulators. */

		updateTimeAvgStats();

		/* Invoke the appropriate event function. */

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
    }
    printf("Current client %d, Total clients %d\n",currentClient,totalClients);
    while(currentClient<=totalClients){
		if(currentClient==totalClients){
			printf("Last client departure\n");
			simulationTime = timeNextEvent[2];
		}
		else{
			printf("Client in queue departure\n");
			depart();
		}
		currentClient++;
	}
    /* Invoke the report generator and end the simulation. */
	
	printf("\n\nREPORT\n\n");
    report();


    return 0;
}


void initialize(void)  /* Initialization function. */{
    /* Initialize the simulation clock. */

    simulationTime = 0.0;
    
    /* Initialize the state variables. */

    serverStatus   = IDLE;
    elementsInQueue  = 0;
    totalElementsInQueue = 0;
    timeLastEvent = 0.0;
    lastQueueEvent = 0.0;
    totalGain = 0.0;
    currentClient=0;
    totalClients=0;

    // Initialize the statistical counters. num_custs_delayed, total_of_delays, area_num_in_q, area_server_status
	averageQueueLength = 0.0;
     

    /* Initialize event list.  Since no customers are present, the departure
       (service completion) event is eliminated from consideration. */
	printf("	NEXT ARRIVE GENERATION\n");
    timeNextEvent[1] = simulationTime + generateNextInterarrive() ; //Planificacion del primer arribo
    timeNextEvent[2] = 1.0e+30; //infinito
	
	printf("	Sistem Time : %.2f \n",simulationTime);
	printf("	Time next arrive : %.2f \n",timeNextEvent[1]);
	printf("	Time next departure : %.2f \n",timeNextEvent[2]);
}


void timing(void)  /* Timing function. */{
    //int   i;
    //float min_time_next_event = 1.0e+29;

    nextEventType = 0; //Al final debe quedar en 1 en caso de que el proximo evento sea un arribo, 2: si es la salida de un servicio
	
	
    /* Determine the event type of the next event to occur. */
	
    if (timeNextEvent[1]<=timeNextEvent[2] && timeNextEvent[1]<=ENDTIME){
		nextEventType = 1;
		simulationTime = timeNextEvent[1];
		totalClients++;
	}
	else{
		nextEventType = 2;
		simulationTime = timeNextEvent[2];
	}
	printf("	Current client: %d\n",currentClient);
	printf("	Total client: %d\n",totalClients);
	printf("	Queue length: %d\n",elementsInQueue);
	printf("	Next event type %d \n",nextEventType);
	printf("	Simulation Time %.2f\n",simulationTime);

    /* advance tstdlib.hhe simulation clock. */

}


void arrive(void)  /* Arrival event function. */{
    //float delay;

    /* Schedule next arrival. */
    clientsTime[totalClients][0]= timeNextEvent[1];
    printf("	client (%d) arrive %.2f\n",totalClients,clientsTime[totalClients][0]);
	printf("	NEXT ARRIVE GENERATION\n");
    timeNextEvent[1] = simulationTime + generateNextInterarrive() ;
    printf("	time next event: %.2f\n",timeNextEvent[1]);
    /* Check to see whether server is busy. */
    if (serverStatus == BUSY) {

        /* Server is busy, so increment number of customers in queue. */
		averageQueueLength += elementsInQueue*(simulationTime-lastQueueEvent);
		printf("	av queue length with %d elem: %d*(%.2f-%.2f)\n",elementsInQueue,elementsInQueue,simulationTime,lastQueueEvent);
		printf("	acumulatedResult: %.2f\n",averageQueueLength);
		lastQueueEvent=simulationTime;
		elementsInQueue++;
		totalElementsInQueue++;
		printf("	Sent to queue. Queue length %d\n",elementsInQueue);
		printf("	Time next arrive : %.2f (%.2f)\n",timeNextEvent[1],timeNextEvent[1]-simulationTime);
		printf("	Time next departure : %.2f (%.2f)\n",timeNextEvent[2],timeNextEvent[2]-simulationTime);
        /* Check to see whether an overflow condition exists. */
		
        if (elementsInQueue > Q_LIMIT) {

            exit(2);
        }

        /* Guardar el tiempo de arribo de esta entidad para los calculos estadisticos */
		totalGain = totalGain + gain;

    }

    else {

        /* Server libre, tener en  cuenta la entidad que pasa directamente al server para los calculos estadisticos */
		serverStatus = BUSY;
		currentClient++;
		clientsTime[totalClients][1]=simulationTime;
			


        /* Schedule a departure (service completion). */
		printf("	NEXT DEPARTURE GENERATION\n");
        timeNextEvent[2] = simulationTime + generateNextDeparture() ;
        clientsTime[totalClients][2]=timeNextEvent[2];
        printf("	client (%d) departure %.2f\n",currentClient,clientsTime[currentClient][2]);
        printf("	Time next arrive : %.2f (%.2f)\n",timeNextEvent[1],timeNextEvent[1]-simulationTime);
		printf("	Next departure : %.2f (%.2f)\n", timeNextEvent[2],timeNextEvent[2]-simulationTime);
		printf("	Simulation Time : %.2f\n",simulationTime);
		totalGain = totalGain + gain;		
    }

}


void depart(void)  /* Departure event function. */{
    //int   i;
    //float delay;

    /* Check to see whether the queue is empty. */
    if (elementsInQueue == 0) {
        /* The queue is empty so make the server idle and eliminate the
           departure (service completion) event from consideration. */
		timeNextEvent[2] = 1.0e+30;
		serverStatus = IDLE; 
		printf("	Empty queue\n");
        
    }

    else {

        /* The queue is nonempty, so decrement the number of customers in
           queue. */
		averageQueueLength += elementsInQueue*(simulationTime-lastQueueEvent);
		printf("	av queue length with %d elem: %d*(%.2f-%.2f)\n",elementsInQueue,elementsInQueue,simulationTime,lastQueueEvent);
		printf("	acumulatedResult: %.2f\n",averageQueueLength);
		lastQueueEvent=simulationTime;
        elementsInQueue--;
        clientsTime[currentClient][2]=simulationTime;
        printf("	client (%d) departure %.2f\n",currentClient,clientsTime[currentClient][2]);
        currentClient++;
        clientsTime[currentClient][1]=simulationTime;
        printf("	client (%d) outOFQueue %.2f\n",currentClient,clientsTime[currentClient][1]);
		printf("	Out of queue. Queue length: %d\n",elementsInQueue);
        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */


        /* Increment the number of customers delayed, and schedule departure. */
		printf("	NEXT DEPARTURE GENERATION\n");
        timeNextEvent[2] = simulationTime + generateNextDeparture();
        clientsTime[currentClient][2]=timeNextEvent[2];
        printf("	client (%d) departure %.2f\n",currentClient,clientsTime[currentClient][2]);
        printf("	Time next arrive : %.2f (%.2f)\n",timeNextEvent[1],timeNextEvent[1]-simulationTime);
		printf("	Time next departure : %.2f (%.2f)\n",timeNextEvent[2],timeNextEvent[2]-simulationTime);
		printf("	Simulation Time : %.2f\n",simulationTime);
    }
}


void report(void)  /* Report generator function. */{
    /* Compute and write estimates of desired measures of performance. */
	float avTimeInTrade=0.0;
	float avTimeInQueue=0.0;
	float serverUtilizationTime = 0.0;

	printf("Simulation total time : %.2f min.\n",simulationTime);
	//printf("Actual client: %d\n",currentClient);
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
    avTimeInQueue = avTimeInQueue/totalElementsInQueue;
	printf("Average delay in queue: %.2f min.\n",avTimeInQueue);

    //Average number in queue
	averageQueueLength = averageQueueLength/simulationTime;
	printf("Average queue legth: %.2f\n", averageQueueLength);
	
    //Server utilization
	printf("Server utilization: %.2f percent\n",serverUtilizationTime/simulationTime*100);

}


void updateTimeAvgStats(void)  /* Update area accumulators for time-average
                                     statistics. */{
    //float timeSinceLastEvent;

    /* Compute time since last event, and update last-event-time marker. */

    timeSinceLastEvent = simulationTime - timeLastEvent;
    timeLastEvent       = simulationTime;

    /* Update area under number-in-queue function. */



    /* Update area under server-busy indicator function. */


}


float generateNextInterarrive()  {
    float rn = lcgrand(7);
    printf("		Generated number : %.2f\n",rn);
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
    printf("		Generated number : %.2f\n",rn);
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

