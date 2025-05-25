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
#include <sys/unistd.h>
#include <sys/stat.h>

#include "Front.h"

#undef false
#undef true

#include "i_system.h"
#include "i_network.h"
#include "spi_lcd.h"
#include "LCD_startup.h"

#define EXAMPLE_MAX_CHAR_SIZE    64

extern void display_buff_ext(uint16_t *buffer);
extern void jsInit();
extern void M_DrawString();

//static spi_flash_host_inst_t spi;

void doomEngineTask(void *pvParameters)
{
    char *argv[]={"doom","-cout","ICWEFDA", "-net", "-server", NULL};
	argv[((int)pvParameters) + 1] = NULL;
    doom_main((int)pvParameters, argv);

}

void serverTask(void *pvParameters)
{
	Server_Main(0, NULL);
}

void StartDoom(int net){
	//spi_stop();
/*

	spi_lcd_init();
	
	jsInit();

	const esp_partition_t* part;
	part=esp_partition_find_first(66, 6, NULL);


	if (part==0) printf("Couldn't find wad part!\n");


	if (net == 5)
		xTaskCreatePinnedToCore(&serverTask, "server", 16000, NULL, 4, NULL, 1);
	xTaskCreatePinnedToCore(&doomEngineTask, "doomEngine", 22480, net, 5, NULL, 0);
	//xTaskCreatePinnedToCore(&ServerTask, "Server", 22480, 0, 5, NULL, 1);
*/
}

void UpdateFrameBuffer(uint16_t* FrameBuffer, uint32_t offset, uint16_t* data){
	//memcpy(FrameBuffer + offset, data, sizeof(uint16_t));
}

void DisplayText(char * text, int x, int y, uint16_t *FrameBuffer, int size){
	
	char * textCpy = text;
	for (int i=0; i<size; i++){
		//printf("size : %d\n", size);
		uint16_t* letter;
		switch (*textCpy++)
		{
			case 'A' : letter = A; break;
			case 'B' : letter = B; break;
			case 'C' : letter = C; break;
			case 'D' : letter = D; break;
			case 'E' : letter = E; break;
			case 'F' : letter = F; break;
			case 'G' : letter = G; break;
			case 'H' : letter = H; break;
			case 'I' : letter = I; break;
			case 'J' : letter = J; break;
			case 'K' : letter = K; break;
			case 'L' : letter = L; break;
			case 'M' : letter = M; break;
			case 'N' : letter = N; break;
			case 'O' : letter = O; break;
			case 'P' : letter = P; break;
			case 'Q' : letter = Q; break;
			case 'R' : letter = R; break;
			case 'S' : letter = S; break;
			case 'T' : letter = T; break;
			case 'U' : letter = U; break;
			case 'V' : letter = V; break;
			case 'W' : letter = W; break;
			case 'X' : letter = X; break;
			case 'Y' : letter = Y; break;
			case 'Z' : letter = Z; break;
			case ':' : letter = PPT; break;
			default: letter = 0; break;
		}
		
		for(int j=0; j<14; j++){
			for(int k=0; k<10; k++){
				UpdateFrameBuffer(FrameBuffer, (y+j)*320+(x+k+i*12), letter[j*10+k]);
			}
		}
	}
}

int ListenInput(){
	//gpio_set_direction(GPIO_NUM_8, GPIO_MODE_INPUT);
	//gpio_set_direction(GPIO_NUM_18, GPIO_MODE_INPUT);
	//gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);
	//gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);

	#define ever ;;
	for(ever){
		//if (gpio_get_level(GPIO_NUM_18))
			return 4;
		//if (gpio_get_level(GPIO_NUM_8))
			return 2;
		//if (gpio_get_level(GPIO_NUM_15))
			return 3;
		//if (gpio_get_level(GPIO_NUM_5))
			return 1;
		

	}
	return 0;

}

int main()
{

	
	spi_main();
	//StartDoom();


    uint16_t * vid_buf = NULL;
    vid_buf = (uint16_t *)malloc(320*240*sizeof(uint16_t));
    if(!vid_buf){
        printf("Failed to allocate memory");
        return -1;
    }
	
    memset(vid_buf, 0xFF, 320*240*sizeof(uint16_t));
	
	
	DisplayText("DOOM :", 10, 10, vid_buf, 7);
	DisplayText("SOLO MODE:     DOWN ", 10, 40, vid_buf, 21);
	DisplayText("CLIENT MODE:   UP", 10, 60, vid_buf, 18);
	DisplayText("SERVER MODE:   RIGHT", 10, 80, vid_buf, 21);
	DisplayText("TEST:", 10, 110, vid_buf, 6);
	DisplayText("RENDER TEST:   LEFT ", 10, 140, vid_buf, 21);
	display_buff_ext(vid_buf);

	int input; 
	input = ListenInput();
	// while (1) ;

	switch (input)
	{
	case 1:
		/*Render test*/
		printf("Render test\n");
		Start_Render_test();
		break;
	case 2:
		printf("Doom client\n");
		StartDoom(4);
		break;
	case 3:
		printf("Doom solo\n");
		StartDoom(3);
		break;
	case 4:
		printf("Doom Server");
		StartDoom(5);
		break;
	default:
		printf("nothing\n");
		break;
	}
	free(vid_buf);
	return 0;
}

