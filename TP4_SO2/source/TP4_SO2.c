#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "string.h"
#include <stdlib.h>

/*Se incluye FreeRTOS.h para poder crear tareas y ejecutar el Scheduler*/
#include "FreeRTOS.h"
#include "task.h"

/*Necesario para la comunicación entre dos tareas mediante la cola.*/
#include "queue.h"

/*Una estructura que guarda los mensajes que se escriben y guardan en la cola.*/
typedef struct A_Message
{
 char ucMessageID;
 char ucData[ 20 ];
} AMessage;

/*Definición de constantes.*/
#define QUEUE_LENGTH 10
#define QUEUE_ITEM_SIZE sizeof(AMessage)

/*Definimos una función usando la convención que imprime "Hello World".*/
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
    /*Variable donde se guarda el mensaje.*/
    AMessage xMessage;

    for(;;){
		strcpy(xMessage.ucData, "Hola Mundo!");
		if( xQueueSendToBack( xQueue, &xMessage, 2000/portTICK_RATE_MS ) != pdPASS )
		{
			printf("No se pudo escribir en la cola\n");
		}
		printf("Guardo en la cola: %s\n", xMessage.ucData);
		vTaskDelay(2000/portTICK_RATE_MS);

		strcpy(xMessage.ucData, "Chau Mundo!");
		if( xQueueSendToBack( xQueue, &xMessage, 2000/portTICK_RATE_MS ) != pdPASS )
		{
			printf("No se pudo escribir en la cola\n");
		}
		printf("Guardo en la cola: %s\n", xMessage.ucData);
		vTaskDelay(2000/portTICK_RATE_MS);
    }
}

/*Función que genera cadenas aleatorias de tamaño también aleatorio y
 * las guarda en una cola*/
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

	for(;;){
		for(i=0;i<20;i++){
			xMessage.ucData[i]='\0';
		}
		longitud = rand()%19+1;
		pxDelay = rand()%3000+500;
		for(i=0;i<longitud;i++){
			xMessage.ucData[i]=rand()%25+97;
		}
		printf("Escribo el siguiente texto: %s\n", xMessage.ucData);
		if( xQueueSendToBack( xQueue, &xMessage, 3500/portTICK_RATE_MS ) != pdPASS ){
			printf("No se pudo escribir en la cola\n");
		}
		for(i=0;i<20;i++){
			xMessage.ucData[i]='\0';
		}
		vTaskDelay(pxDelay/portTICK_RATE_MS);
	}
}

void vSensor (void *pvParameter){
	/*Declara una variable local donde se guarda la cola que recibe desde main.*/
	QueueHandle_t xQueue;
	xQueue = (QueueHandle_t) pvParameter;
    /*Variable donde se guarda el mensaje.*/
	AMessage xMessage;

	int temperatura;

	for(;;){
		temperatura=rand()%45+10;
		itoa(temperatura,xMessage.ucData,10);
		printf("Lectura del sensor: %d\n", temperatura);
		if( xQueueSendToBack( xQueue, &xMessage, 2000/portTICK_RATE_MS ) != pdPASS ){
			printf("No se pudo escribir en la cola\n");
		}
		vTaskDelay(2000/portTICK_RATE_MS);
	}
}

void vConsumidor(void *pvParameter){
	/*Declara una variable local donde se guarda la cola que recibe desde main.*/
    QueueHandle_t xQueue;
    xQueue = (QueueHandle_t) pvParameter;

    /*Variable donde se guarda el mensaje.*/
    AMessage xMessage;

    for(;;){
		if( xQueueReceive( xQueue, &xMessage, portMAX_DELAY ) != pdPASS ){
			printf("No se leyó nada de la cola\n");
		}
		printf("Leí de la cola el mensaje: %s\n",xMessage.ucData);
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
