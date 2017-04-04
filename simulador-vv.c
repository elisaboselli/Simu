/* External definitions for single-server queueing system. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.h"  /* Header file for random-number generator. */

#define Q_LIMIT 100  /* Limit on queue length. */
#define BUSY      1  /* Mnemonics for server's being busy */
#define IDLE      0  /* and idle. */
#define CANTSERVERS 2
#define MAXTIME 1000000

void  initialize(void);
void  timing(void);
void  arrive(void);
void  depart(void);
void  report(void);
void  update_time_avg_stats(void);
float expon(float mean);
float gen_next_interarrive(void);
float  gen_next_depart(void);


float departs[CANTSERVERS];  
// numero de servidores, estos guardan las salidas!
int serversBusy =0;
int num_custs_delayed;
int num_delays_required=30;
int next_event_type;
int server_status;
float sim_time=0;
int num_in_q ;
float time_last_event;
float time_next_event[3];

//~ estadisticos para medir el tiempo de espera medio en cola
float total_clients_in_queue=0;
float acum_queue_waittime =0;
float queue[100];

//~ estadisticos para medir el tiempo de espera medio en cola
float last_modif_queue=0;
float acum_longPerTime =0;

// estadisticos para medir la utilizacion de los servidores

float tiempo_ultimo_evento =0;
float acum_utilizacion =0;


total_clients=0;



void init_servers(float a){
	//~ printf("se inicializan los servidores con: infinito");
	
  int i =0;
  while (i<CANTSERVERS){
    departs[i] = a;
    i++;
  }
}

void encolar(){
	printf("encolar!\n");
	if(num_in_q < Q_LIMIT){
	
	acum_longPerTime+=num_in_q*(sim_time-last_modif_queue);
	last_modif_queue=sim_time;
	
	queue[num_in_q] = sim_time;
	num_in_q++;
	
	total_clients_in_queue++;
	
	}
	
	}

void desencolar(){
	printf("desencolar!\n");
	if(num_in_q>0){		
		acum_longPerTime+=num_in_q*(sim_time-last_modif_queue);
		last_modif_queue=sim_time;
		
		acum_queue_waittime+= (sim_time-queue[0]);
		reorderqueue(0,num_in_q-1);
		num_in_q--;
		}
	
	}



int main()  /* Main function. */{
    /* Initialize the simulation. */

    initialize();
	
    /* Run the simulation while more delays are still needed. */

    while (sim_time < MAXTIME) {
		printf("Hora actual: %f \n",sim_time);
        /* Determine the next event. */
		show_next_events();
        timing();

        /* Update time-average statistical accumulators. */

        //update_time_avg_stats();

        /* Invoke the appropriate event function. */

        switch (next_event_type) {
            case 1:
				//~ printf("el prox elemento sera un arribo! \n");
                arrive();
                break;
            case 2:
                //~ printf("el prox elemento sera una salida! \n");
                depart();
                break;
        }
        num_custs_delayed++;
        printf("\n\n\n\n\n\n");
		// getchar (); 
    }

    /* Invoke the report generator and end the simulation. */

    report();


    return 0;
}


void initialize(void)  /* Initialization function. */{
    /* Initialize the simulation clock. */
    sim_time = 0.0;
    /* Initialize the state variables. */

    server_status   = 0;
    num_in_q        = 0;
    time_last_event = 0;

    // Initialize the statistical counters. num_custs_delayed, total_of_delays, area_num_in_q, area_server_status
	num_custs_delayed=0;

    /* Initialize event list.  Since no customers are present, the departure
       (service completion) event is eliminated from consideration. */
	
    time_next_event[1] = sim_time + gen_next_interarrive() ; //Planificacion del primer arribo
    
    printf("El primer arribo sera a las: %f \n",time_next_event[1]);
    init_servers( 1.0e+30); //infinito
	//~ printf("inicializacion!\n");
}


void timing(void)  /* Timing function. */
{
    int   i;
    float min_time_next_event = 1.0e+29;

    next_event_type = 0; //Al final debe quedar en 1 en caso de que el proximo evento sea un arribo, 2: si es la salida de un servicio
	

    /* Determine the event type of the next event to occur. */

	//~ printf("Estamos en TIMING \n");
    if(min_time_next_event > time_next_event[1]){
      next_event_type = 1 ;
      min_time_next_event = time_next_event[1];
    }
    
    i=0;
    while(i<serversBusy){
      if (min_time_next_event > departs[i]){
          next_event_type=2;
          min_time_next_event = departs[i];

      }
      i++;
    }
	//~ printf("el proximo evento es a las: %f \n",min_time_next_event);

    /* advance the simulation clock. */

    sim_time = min_time_next_event;
	
}


void arrive(void)  /* Arrival event function. */
{
	acum_utilizacion+=(sim_time-tiempo_ultimo_evento)*serversBusy;
	tiempo_ultimo_evento=sim_time;
	
    float delay;
	total_clients++;
    /* Schedule next arrival. */
	//~ printf("acaba de arribar uno ! son las: %f \n",sim_time);
    time_next_event[1] = sim_time + gen_next_interarrive();
	//~ printf("EL PROXIMO ARRIBO sERA las: %f \n",time_next_event[1]);
    /* Check to see whether server is busy. */

    if (server_status == BUSY) {

        /* Server is busy, so increment number of customers in queue. */
        encolar();

        /* Check to see whether an overflow condition exists. */

        if (num_in_q > Q_LIMIT) {

            exit(2);
        }

        /* Guardar el tiempo de arribo de esta entidad para los calculos estadisticos */

    }
    else {
        /* Server libre, tener en  cuenta la entidad que pasa directamente al server para los calculos estadisticos */
        /* Schedule a departure (service completion). */
        float auxfloat =gen_next_depart();
        printf("el auxfloat es:%f \n",auxfloat);
        add_to_departs(auxfloat);
    
    }
}


void depart(void)  /* Departure event function. */
{
    acum_utilizacion+=(sim_time-tiempo_ultimo_evento)*serversBusy;
	tiempo_ultimo_evento=sim_time;
    
    
    //~ printf("es hora de que salga uno! :%f \n",sim_time);
    int i;
    float delay;
    /* Check to see whether the queue is empty. */
    //~ printf("La cola es de :%i \n",num_in_q);
    if (num_in_q == 0) {
      makedeparture ();
        /* The queue is empty so make the server idle and eliminate the
           departure (service completion) event from consideration. */

    }else {
		//~ printf("se va uno y entra otro que estaba en la cola, ahora la cola es de  :%i \n",num_in_q);
        makedeparture();
        add_to_departs(gen_next_depart());
        desencolar();
        /* The queue is nonempty, so decrement the number of customers in
           queue. */

        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */
        /* Increment the number of customers delayed, and schedule departure. */
    }
}


void report(void)  /* Report generator function. */
{
    /* Compute and write estimates of desired measures of performance. */

    // \\Average delay in queue
    printf("El numero promedio de espera en la cola es de %f \n",acum_queue_waittime/total_clients_in_queue);
    //
    // \\Average number in queue
    //
    printf("La longitud promedio de la cola es de %f \n",acum_longPerTime /sim_time);
    
    
    // \\Server utilization
	printf("La utilizacion de cada servidor es de %f \n",acum_utilizacion /(sim_time*CANTSERVERS));

}


void update_time_avg_stats(void)  /* Update area accumulators for time-average
                                     statist
                                     * ics. */
{
    float time_since_last_event;

    /* Compute time since last event, and update last-event-time marker. */

    time_since_last_event = sim_time - time_last_event;
    time_last_event       = sim_time;

    /* Update area under number-in-queue function. */



    /* Update area under server-busy indicator function. */


}




void add_to_departs(double timeofdepart){
 //~ printf("ENTRO EN ADD_TO_DEPARTS, los servidores ocupados son:%i \n",serversBusy);
 //~ printf("el depart que se agregara es a las: %f\n",timeofdepart);
 //~ printf("%f\n",sim_time+timeofdepart);
 departs[serversBusy] = sim_time+timeofdepart;
 serversBusy++;
 if (serversBusy == CANTSERVERS){
    server_status = 1 ;
 }
}
void reorderdeparts (int pos){
  while (pos<CANTSERVERS-1){
	  //~ printf("Entro al while de reorder pos=%i\n",pos);
    departs[pos] = departs[pos+1];
    pos++;
  }
}

void reorderqueue (int pos,int fin){
  while (pos<fin-1){
	  //~ printf("Entro al while de reorder pos=%i\n",pos);
    queue[pos] = queue[pos+1];
    pos++;
  }
}

void makedeparture(){
  int aux = 1;
  int i=0;
  while (i<serversBusy && aux==1){
    if (departs[i]==sim_time){
	  //~ printf("Entro al if de makedeparture! i=%i\n",i);
      
      aux = 0;
      reorderdeparts(i);

      serversBusy--;
      server_status =0;
    }
    //~ printf("while de makedeparture! i=%i\n",i);
    i++;
  }
  //~ printf("Salio del while de make departure!");
}


float gen_next_interarrive()  {
   
    float aux,rn;
    rn =lcgrand(7);
    printf("Generated number : %f\n",rn);
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
};

float gen_next_depart()  {
    //~ printf("Estamos generando el proximo depart!\n");
    float aux,rn;
    rn = lcgrand(3);
    printf("Generated number : %f\n",rn);
	if (rn >= 0.0 && rn<0.15){
			return 10;
			
	}
	
	if (rn >= 0.15 && rn<0.40){
			return 20;
			
	}
	if (rn >= 0.4 && rn<0.8){
			return 30;
			
	}
	if (rn >= 0.8 && rn<1){
			return 40;
			
	}
	return 40;
	
}

void show_next_events(){
	printf("ESTADO ACTUAL:\n");
	printf("Prox entrada: %f\n",time_next_event[1]);
	int i =0;
	while(i<serversBusy){
		printf("Prox salida(%i): %f\n",i,departs[i]);
		i++;
	}
	while(i<CANTSERVERS){
		printf("Prox salida(%i): INF\n",i);
		i++;
	}
   printf("Cola:%i\n",num_in_q);

	
	
	
}
