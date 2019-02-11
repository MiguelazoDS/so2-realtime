#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "string.h"
#include <stdlib.h>

/*Se incluye FreeRTOS.h para poder crear tareas, ejecutar el Scheduler 
 * y configurar el entorno de tiempo real.*/
#include "FreeRTOS.h"
#include "task.h"

/*Necesario para la comunicación entre dos tareas mediante la cola.*/
#include "queue.h"

/*Una estructura que contiene un campo es_num, para indicar que se recibe 
 * un valor numérico y hacer el "casting" correspondiente" 
 * El puntero void permite independizarse del tipo, ya que puede guardar
 * direcciones de cualquier tipo y usar "casting" de cualquier tipo.*/
typedef struct A_Message
{
 int es_num;
 void *pointer;
} AMessage;

/*Definición de constantes.*/
#define QUEUE_LENGTH 10
#define QUEUE_ITEM_SIZE sizeof(AMessage)

/*Definimos una función que imprime "Hello World".*/
void vPrint(void *pvParameter){
    for(;;){
    	printf("Hello World\n");
        vTaskDelay(500);
    }
}

/*Tarea vProductor que escribe "Hola Mundo!" y "Chau Mundo!" en una cola.*/
void vProductor(void *pvParameter){
	/*Declara una variable local donde se guarda la cola que recibe desde main.*/
    QueueHandle_t xQueue;
    xQueue = (QueueHandle_t) pvParameter;

    /*Estructura donde se guarda el mensaje.*/
    AMessage xMessage;
    char msg[20];

    for(;;){
    	strcpy(msg, "Hola Mundo!");
		if( xQueueSendToBack( xQueue, &xMessage, 2000/portTICK_RATE_MS ) != pdPASS )
		{
			printf("No se pudo escribir en la cola\n");
		}
		printf("Guardo en la cola: %s\n", msg);
		vTaskDelay(2000/portTICK_RATE_MS);

		strcpy(msg,"Chau Mundo!");
		if( xQueueSendToBack( xQueue, &xMessage, 2000/portTICK_RATE_MS ) != pdPASS )
		{
			printf("No se pudo escribir en la cola\n");
		}
		printf("Guardo en la cola: %s\n", msg);
		vTaskDelay(2000/portTICK_RATE_MS);
    }
}

/*Función que genera cadenas aleatorias de tamaño también aleatorio*/
void vTeclado (void *pvParameter){
	/*Declara una variable local donde se guarda la cola que recibe desde main.*/
	QueueHandle_t xQueue;
	xQueue = (QueueHandle_t) pvParameter;

    /*Variable donde se guarda el mensaje.*/
	AMessage xMessage;

	/*Variable donde se guarda el valor aleatorio de delay.*/
	TickType_t pxDelay;

	int i;
	int longitud;

	/*Arreglo donde se guarda el mensaje*/
	char msg[20];
	
	/*Definimos que no es un número lo que irá en la estructura*/
	xMessage.es_num=0;
	
	for(;;){
		/*Limpiamos el arreglo*/
		for(i=0;i<20;i++){
			msg[i]='\0';
		}
        /*Genera longitudes que van desde 1 a 19*/
		longitud = rand()%19+1;

		/*Genera valores desde 500 hasta 3499*/
		pxDelay = rand()%3000+500;

		for(i=0;i<longitud;i++){
			/*Genera caracteres desde 97 hasta 121*/
			msg[i]=rand()%25+97;
		}
		/*Guarda la dirección del arreglo en el puntero de la estructura*/
		xMessage.pointer=&msg;

		printf("Escribo el siguiente texto: %s\n", msg);

		/*Envía e imprime mensaje de error en caso de falla*/
		if( xQueueSendToBack( xQueue, &xMessage, 3500/portTICK_RATE_MS ) != pdPASS ){
			printf("No se pudo escribir en la cola\n");
		}

		/*Limpia el arreglo*/
		for(i=0;i<20;i++){
			msg[i]='\0';
		}

		/*portTICK_RATE_MS definido en FreeRTOSConfig.h con un valor default de 1000
		 * Nos da valores de 500 ms a 3,5 segundos*/
		vTaskDelay(pxDelay/portTICK_RATE_MS);
	}
}

void vSensor (void *pvParameter){
	/*Declara una variable local donde se guarda la cola que recibe desde main.*/
	QueueHandle_t xQueue;
	xQueue = (QueueHandle_t) pvParameter;

    /*Variable donde se guarda el mensaje.*/
	AMessage xMessage;

	/*Define un entero de 8bits*/
	uint8_t temperatura;

	/*Setea en la estructura que lo que se envía es un número*/
	xMessage.es_num=1;
	for(;;){
 		/*Crea un valor entre 10 y 54*/
		temperatura=rand()%45+10;

		/*Guarda en el puntero la dirección del entero*/
		xMessage.pointer=&temperatura;
		printf("Lectura del sensor: %d\n", temperatura);
		
		/*Envía e imprime mensaje de error en caso de falla*/
		if( xQueueSendToBack( xQueue, &xMessage, 2000/portTICK_RATE_MS ) != pdPASS ){
			printf("No se pudo escribir en la cola\n");
		}

 		/*2 segundos.*/
		vTaskDelay(2000/portTICK_RATE_MS);
	}
}

void vConsumidor(void *pvParameter){
	/*Declara una variable local donde se guarda la cola que recibe desde main.*/
    QueueHandle_t xQueue;
    xQueue = (QueueHandle_t) pvParameter;

    /*Variable donde se guarda el mensaje.*/
    AMessage xMessage;

	/*Variable de 8bits entera para almacenar el valor recibido*/
    uint8_t num;

    for(;;){
		/*Lee e imprime error en caso de falla*/
		if( xQueueReceive( xQueue, &xMessage, portMAX_DELAY ) != pdPASS ){
			printf("No se leyó nada de la cola\n");
		}

		/*Si el mensaje es un número hace un casting y luego lo imprime
		 * Muestra también el tamaño del entero, de lo contrario muestra la cadena.*/
		if(xMessage.es_num == 1){
			num=*((uint8_t*) xMessage.pointer);
			printf("Leí de la cola el mensaje: %d que tiene un tamaño de: %d byte\n",num, sizeof(num));//.ucData);
		}
		else
			printf("Leí de la cola el mensaje: %s\n",(char*)xMessage.pointer);
    }
}

int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    /*No se puede correr el programa si no está esta línea.
     El programa finaliza en xTaskCreate() o en la creación de la cola.
     Debe ser la primer línea y es necesaria para comenzar a grabar.*/
    vTraceEnable(TRC_START);

    /*Creamos una cola*/
    QueueHandle_t xQueue;
    xQueue = xQueueCreate( QUEUE_LENGTH, QUEUE_ITEM_SIZE );

    /*Agregamos las siguientes tareas*/
    /*xTaskCreate(vPrint, "vPrint", 240, NULL, 1, NULL);*/
    /*xTaskCreate(vProductor, "Productor", 240, (void *)xQueue, 1, NULL);*/
    xTaskCreate(vTeclado, "Teclado", 240, (void *)xQueue, 1, NULL);
    xTaskCreate(vSensor, "Sensor", 240, (void*)xQueue, 1, NULL);
    xTaskCreate(vConsumidor, "Consumidor", 240, (void *)xQueue, 2, NULL);

    /*El planificador se encarga de gestionar las tareas agregadas.*/
    vTaskStartScheduler();

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
    }
    return 0 ;
}
