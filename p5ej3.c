#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX 20
#define XO 77
#define MULT 8 // multiplo de 4 + 1
#define INC 17 // distinto de 0
#define MOD 90 // potencia de 2

float genNumber(int last, int mult, int inc, int mod);
float numbers [MAX];
int lastValue;

int main(){
	int i = 0;
	float lastValue = XO;
	float firstValue;
	while (i<MAX){
		lastValue=genNumber(lastValue,MULT,INC,MOD);
		if (i==0){
			firstValue=lastValue;
		}
		if (i>0 && firstValue==lastValue){
			printf("Fin del periodo \n");
		}
		printf("%f (",lastValue);
		numbers[i]=lastValue/MOD;
		printf("%f)\n",numbers[i]);
		i++;
	}
	return 0;
}

float genNumber(int x, int a, int c, int m){
	int aux = ((a*x)+c)%m;
	return aux;
}
