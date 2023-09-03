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

#include "esp_attr.h"

//#include "D:\DOCUMENTS\Raphael\console\TFT_eSPI-master\TFT_eSPI.h" // Graphics and font library for ILI9341 driver chip
//#include <SPI.h>

#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"
#include "rom/crc.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdlib.h>
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_partition.h"
#include <esp_spi_flash.h>
#include <string.h>

#include "Render.h"
#include "Front.h"


#undef false
#undef true
#include "i_system.h"

#include "spi_lcd.h"
#include "LCD_startup.h"

extern void display_buff_ext(uint16_t *buffer);
extern void jsInit();
extern void M_DrawString();


void doomEngineTask(void *pvParameters)
{
    char const *argv[]={"doom","-cout","ICWEFDA", "-net", NULL};
    doom_main((int)pvParameters, argv);
}

void StartDoom(int net){
	//spi_stop();


	spi_lcd_init();
	
	jsInit();

	
	const esp_partition_t* part;
	part=esp_partition_find_first(66, 6, NULL);
	if (part==0) printf("Couldn't find wad part!\n");

	xTaskCreatePinnedToCore(&doomEngineTask, "doomEngine", 22480, net, 5, NULL, 0);
}

void DisplayText(char * text, int x, int y, uint16_t *FrameBuffer, int size){
	
	for (int i=0; i<size; i++){
		printf("size : %d\n", size);
		for(int j=0; j<14; j++){
			for(int k=0; k<10; k++){
				if(text[i] == 'A'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &A[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'B'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &B[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'C'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &C[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'D'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &D[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'E'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &E[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'F'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &F[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'G'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &G[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'H'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &H[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'I'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &I[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'J'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &J[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'K'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &K[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'L'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &L[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'M'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &M[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'N'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &N[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'O'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &O[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'P'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &P[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'Q'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &Q[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'R'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &R[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'S'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &S[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'T'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &T[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'U'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &U[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'V'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &V[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'W'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &W[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'X'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &X[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'Y'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &Y[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == 'Z'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &Z[j*10+k], sizeof(uint16_t));
				}
				if(text[i] == ':'){
					memcpy(FrameBuffer+((y+j)*320+(x+k+i*12)), &PPT[j*10+k], sizeof(uint16_t));
				}
			}
		}
	}
}

int ListenInput(){
	gpio_set_direction(GPIO_NUM_8, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_18, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);
	#define ever ;;
	for(ever){
		if (gpio_get_level(GPIO_NUM_18)==1) {
			return 1;
		}
		if (gpio_get_level(GPIO_NUM_8)==1) {
			return 2;
		}
		if (gpio_get_level(GPIO_NUM_15)==1) {
			return 3;
		}
	}
	return 0;

}

void app_main()
{
	spi_main();
	//StartDoom();
	
	/*int16_t lcdpal[256];
	int pal = 0;
	int i, r, g, b, v;

	const byte * palette = 0x3c8d000c;
	palette+=pal*(3*256);
	for (i=0; i<255 ; i++) {
		v=((palette[0]>>3)<<11)+((palette[1]>>2)<<5)+(palette[2]>>3);
		lcdpal[i]=(v>>8)+(v<<8);
	//		lcdpal[i]=v;
		palette += 3;
	}
*/



    uint16_t * vid_buf = NULL;
    vid_buf = (uint16_t *)malloc(320*240*sizeof(uint16_t));
    if(!vid_buf){
        printf("Failed to allocate memory");
        return;
    }
	
    memset(vid_buf, 0xFF, 320*240*sizeof(uint16_t));
    
	
	DisplayText(&"DOOM :", 10, 10, vid_buf, 7);
	DisplayText(&"ONLINE MODE:   UP", 10, 40, vid_buf, 18);
	DisplayText(&"SOLO MODE:     DOWN ", 10, 60, vid_buf, 21);
	DisplayText(&"RENDER TEST:   LEFT ", 10, 90, vid_buf, 21);
	display_buff_ext(vid_buf);

	int input; 
	input = ListenInput();
	switch (input)
	{
	case 1:
		/*Render test*/
		printf("Render test\n");
		Start_Render_test();
		break;
	case 2:
		printf("Doom online\n");
		StartDoom(4);
		break;
	case 3:
		printf("Doom solo\n");
		StartDoom(3);
		break;
	default:
		printf("nothing\n");
		break;
	}
	free(vid_buf);

}

