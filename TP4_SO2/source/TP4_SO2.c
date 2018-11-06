/*
 * Copyright 2016-2018 NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    TP4_SO2.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "string.h"
/* TODO: insert other include files here. */
/*Se incluye FreeRTOS.h para poder crear tareas y ejecutar el Scheduler*/
#include "FreeRTOS.h"
#include "task.h"
/*Necesario para la comunicación entre dos tareas.*/
#include "queue.h"
/* TODO: insert other definitions and declarations here. */
typedef struct A_Message
{
 char ucMessageID;
 char ucData[ 20 ];
} AMessage;

#define QUEUE_LENGTH 10
#define QUEUE_ITEM_SIZE sizeof(AMessage)

/*Definimos una función usando la convención*/
void vPrint(void *pvParameter){
    for(;;){
    	printf("Hello World\n");
        vTaskDelay(500);
    }
}

void vProductor(void *pvParameter){
    QueueHandle_t xQueue;
    xQueue = (QueueHandle_t) pvParameter;
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

void vTeclado (void *pvParameter){
	QueueHandle_t xQueue;
	xQueue = (QueueHandle_t) pvParameter;
	AMessage xMessage;
	TickType_t pxDelay;
	int letra;
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
		printf("Soy una cadena: %s\n",xMessage.ucData);
		for(i=0;i<20;i++){
			xMessage.ucData[i]='\0';
		}
		vTaskDelay(pxDelay/portTICK_RATE_MS);
	}
}

void vSensor (void *pvParameter){
	int temperatura;
	QueueHandle_t xQueue;
	xQueue = (QueueHandle_t) pvParameter;
	AMessage xMessage;

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
    QueueHandle_t xQueue;
    xQueue = (QueueHandle_t) pvParameter;
    AMessage xMessage;
    for(;;){
		if( xQueueReceive( xQueue, &xMessage, portMAX_DELAY ) != pdPASS ){
			printf("No se leyó nada de la cola\n");
		}
		printf("Leí de la cola el mensaje: %s\n",xMessage.ucData);
    }
}

/*
 * @brief   Application entry point.
 */
int main(void) {
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    /*No se puede correr el programa si no está esta línea.
     El programa finaliza en xTaskCreate() o en la creación de la cola.
     Debe ser la primer línea.*/
    vTraceEnable(TRC_START);

    /*Creamos una cola*/
    QueueHandle_t xQueue;
    xQueue = xQueueCreate( QUEUE_LENGTH, QUEUE_ITEM_SIZE );

    /*No agreamos esta tarea*/
    /*xTaskCreate(vPrint, "vPrint", 240, NULL, 1, NULL);*/
    /*xTaskCreate(vProductor, "Productor", 240, (void *)xQueue, 1, NULL);*/
    xTaskCreate(vTeclado, "Teclado", 240, (void *)xQueue, 1, NULL);
    xTaskCreate(vSensor, "Sensor", 240, (void*)xQueue, 1, NULL);
    xTaskCreate(vConsumidor, "Consumidor", 240, (void *)xQueue, 2, NULL);

    vTaskStartScheduler();
    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
    }
    return 0 ;
}

