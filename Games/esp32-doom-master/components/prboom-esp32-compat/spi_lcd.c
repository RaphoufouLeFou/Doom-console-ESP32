// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/param.h>

//#include "esp_heap_alloc_caps.h"

#if 0
#define PIN_NUM_MISO 46
#define PIN_NUM_MOSI 8
#define PIN_NUM_CLK  3
#define PIN_NUM_CS   16
#define PIN_NUM_DC   18
#define PIN_NUM_RST  17
#define PIN_NUM_BCKL 5

#elif 1

#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11
#define PIN_NUM_CLK  12
#define PIN_NUM_CS   38
#define PIN_NUM_DC   10
#define PIN_NUM_RST  9
#define PIN_NUM_BCKL 5

#else 

#define PIN_NUM_MISO 8
#define PIN_NUM_MOSI 46
#define PIN_NUM_CLK  3

#define PIN_NUM_CS   18
#define PIN_NUM_DC   16
#define PIN_NUM_RST  17
#define PIN_NUM_BCKL 5
#endif

//You want this, especially at higher framerates. The 2nd buffer is allocated in iram anyway, so isn't really in the way.
#define DOUBLE_BUFFER 1



#ifndef DOUBLE_BUFFER
volatile static uint16_t *currFbPtr=NULL;
#else
//Warning: This gets squeezed into IRAM.
static uint32_t *currFbPtr=NULL;
#endif

#define NO_SIM_TRANS 6 //Amount of SPI transfers to queue in parallel
#define MEM_PER_TRANS 1024*3 //in 16-bit words

extern int16_t lcdpal[256];
void  displayTask(void *arg) {

}


void spi_lcd_wait_finish() {
#ifndef DOUBLE_BUFFER
	xSemaphoreTake(dispDoneSem, portMAX_DELAY);
#endif
}

void spi_lcd_send(uint32_t *scr) {
#ifdef DOUBLE_BUFFER

	memcpy(currFbPtr, scr, 320*240);

    /*size_t i;

    for (i=0; i < 320*240; i++)
        currFbPtr[320*240-1-i] = scr[i];
*/
	//Theoretically, also should double-buffer the lcdpal array... ahwell.
#else
	currFbPtr=scr;
#endif
	//xSemaphoreGive(dispSem);
}

void spi_lcd_init() {
/*
	printf("spi_lcd_init()\n");
    dispSem=xSemaphoreCreateBinary();
    dispDoneSem=xSemaphoreCreateBinary();
#ifdef DOUBLE_BUFFER
	//currFbPtr=pvPortMallocCaps(320*240, MALLOC_CAP_32BIT);
	currFbPtr=malloc(320*240*4);
#endif
#if CONFIG_FREERTOS_UNICORE
	xTaskCreatePinnedToCore(&displayTask, "display", 6000, NULL, 6, NULL, 0);
#else
	xTaskCreatePinnedToCore(&displayTask, "display", 6000, NULL, 6, NULL, 1);
#endif

*/
}