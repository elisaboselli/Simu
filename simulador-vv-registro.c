/* External definitions for single-server queueing system. */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcgrand.h"  /* Header file for random-number generator. */
#include <string.h>


//VALORES PARA MODIFICAR LA SIMULACION

#define Q_LIMIT 100000  /* LIMITE DE LA COLA */
#define CANTSERVERS 2
#define MAXTIME 3000
#define SHOWS_ENABLED 2





#define BUSY      1  /* Mnemonics for server's being busy */
#define IDLE      0  /* and idle. */



void  initialize(void);
void  timing(void);
void  arrive(void);
void  depart(void);
void  report(void);
void  update_time_avg_stats(void);
float expon(float mean);
float gen_next_interarrive(void);
float  gen_next_depart(void);



typedef struct TipoCliente
{
	
   float tiempoarribo;
   float tiemposalida;
   char id[20];
} TCliente;





TCliente departs[CANTSERVERS];
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
double acum_queue_waittime =0;
TCliente queue[100];

//~ estadisticos para medir longitud media en cola
float last_modif_queue=0;
float acum_longPerTime =0;

// estadisticos para medir la utilizacion de los servidores

float tiempo_ultimo_evento =0;
float acum_utilizacion =0;


//estadisticos para medir tiempo medio en el sistema
float acum_timeinsystem =0;

int total_clients=0;

TCliente clientinf(){
	TCliente aux;
	strcpy(aux.id, "[]");
	return aux;
}



void init_servers(float a){
	//~ printf("se inicializan los servidores con: infinito");
	
  int i =0;
  while (i<CANTSERVERS){
    departs[i] = clientinf();
    i++;
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
    if (departs[i].tiemposalida==sim_time){
	  //~ printf("Entro al if de makedeparture! i=%i\n",i);
      
      acum_timeinsystem += (sim_time-departs[i].tiempoarribo);
      total_clients++;
      
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



void encolar(){
	//printf("encolar!\n");
	if(num_in_q < Q_LIMIT){
	
	acum_longPerTime+=num_in_q*(sim_time-last_modif_queue);
	last_modif_queue=sim_time;
	
	
	TCliente aux; strcpy(aux.id, "Cliente!"); aux.tiempoarribo = sim_time;
	queue[num_in_q] =aux;
	num_in_q++;
	
	total_clients_in_queue++;
	
	}
	
	}

TCliente desencolar(){
	//printf("desencolar!\n");
	TCliente auxresultado=clientinf();
	if(num_in_q>0){		
		acum_longPerTime+=num_in_q*(sim_time-last_modif_queue);
		last_modif_queue=sim_time;
		
		acum_queue_waittime+= (sim_time-queue[0].tiempoarribo);
		
		auxresultado=queue[0];
		
		
		reorderqueue(0,num_in_q-1);
		num_in_q--;
		}
	return auxresultado;
	}





void initialize(void)  /* Initialization function. */{
    printf ("TIEMPO MAXIMO: %i ", MAXTIME );
    
    
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
      if (min_time_next_event > departs[i].tiemposalida){
          next_event_type=2;
          min_time_next_event = departs[i].tiemposalida;

      }
      i++;
    }
	//~ printf("el proximo evento es a las: %f \n",min_time_next_event);

    /* advance the simulation clock. */

    sim_time = min_time_next_event;

	if (SHOWS_ENABLED == 2 && (total_clients % 100)==0){
		report();
		}
	
}
void add_to_departs(TCliente clienteEntrando){
 //~ printf("ENTRO EN ADD_TO_DEPARTS, los servidores ocupados son:%i \n",serversBusy);
 //~ printf("el depart que se agregara es a las: %f\n",timeofdepart);
 //~ printf("%f\n",sim_time+timeofdepart);
 
 
 
 departs[serversBusy] = clienteEntrando;
 serversBusy++;
 if (serversBusy == CANTSERVERS){
    server_status = 1 ;
 }
}

void arrive(void){  /* Arrival event function. */

	acum_utilizacion+=(sim_time-tiempo_ultimo_evento)*serversBusy;
	tiempo_ultimo_evento=sim_time;
	
    
	
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
        //printf("el auxfloat es:%f \n",auxfloat);
        TCliente nuevocliente;strcpy(nuevocliente.id, "Cliente!"); nuevocliente.tiempoarribo=sim_time;nuevocliente.tiemposalida=(sim_time+auxfloat);
        add_to_departs(nuevocliente);
    
    }
}


void depart(void)  /* Departure event function. */
{
    acum_utilizacion+=(sim_time-tiempo_ultimo_evento)*serversBusy;
	tiempo_ultimo_evento=sim_time;
    
    
    total_clients++;
    
    
    
    //~ printf("es hora de que salga uno! :%f \n",sim_time);

    /* Check to see whether the queue is empty. */
    //~ printf("La cola es de :%i \n",num_in_q);
    makedeparture ();
    if (num_in_q != 0) {
        TCliente aux = desencolar();
        aux.tiemposalida = gen_next_depart()+sim_time;
        departs[serversBusy] = aux;
		serversBusy++;
		if (serversBusy == CANTSERVERS){
			server_status = 1 ;
		 }
        
        /* The queue is nonempty, so decrement the number of customers in
           queue. */

        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */
        /* Increment the number of customers delayed, and schedule departure. */
    }
}


void report(void)  /* Report generator function. */
{
    printf ("--------------------------------------------------------------------------- \n");
    /* Compute and write estimates of desired measures of performance. */
	printf("El tiempo promedio en el sistema %f \n",acum_timeinsystem/total_clients);
    // \\Average delay in queue
		
    printf("El tiempo promedio de espera en la cola es de %f \n",acum_queue_waittime/total_clients_in_queue);
    //
    // \\Average number in queue
    //

    printf("La longitud promedio de la cola es de %f \n",acum_longPerTime /sim_time);
    
    
    // \\Server utilization
	printf("La utilizacion de cada servidor es de %.2f % \n",(acum_utilizacion /(sim_time*CANTSERVERS))*100);

	// \\numero promedio de clientes en el sistema
	printf("El numero promedio de clientes en el sistema es de %f  \n",(acum_utilizacion /(sim_time)));
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








float gen_next_interarrive()  {
   
    float rn;
    rn =lcgrand(7);
    //printf("Generated number : %f\n",rn);
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
    float rn;
    rn = lcgrand(3);
    //printf("Generated number : %f\n",rn);
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
	if (SHOWS_ENABLED == 1){
		printf("\n\n\n\n\n\n");
		printf("ESTADO ACTUAL:\n");
		printf("Prox entrada: %f\n",time_next_event[1]);
		int i =0;
		while(i<serversBusy){
			printf("Prox salida(%i): %f\n",i,departs[i].tiemposalida);
			i++;
		}
		while(i<CANTSERVERS){
			printf("Prox salida(%i): INF\n",i);
			i++;
		}
	   printf("Cola:%i\n",num_in_q);
	}
	

	
	
	
}


int main()  /* Main function. */{
    /* Initialize the simulation. */

    initialize();
	
    /* Run the simulation while more delays are still needed. */

    while (sim_time < MAXTIME) {
		
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
        
		// getchar (); 
    }

    /* Invoke the report generator and end the simulation. */

    report();


    return 0;
}
