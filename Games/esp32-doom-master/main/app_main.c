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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdlib.h>
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_partition.h"
#include <esp_spi_flash.h>

#undef false
#undef true
#include "i_system.h"

#include "spi_lcd.h"


extern void jsInit();
extern void M_DrawString();


void doomEngineTask(void *pvParameters)
{
    char const *argv[]={"doom","-cout","ICWEFDA", NULL};
    doom_main(3, argv);
}

void StartDoom(){
	const esp_partition_t* part;
	part=esp_partition_find_first(66, 6, NULL);
	if (part==0) printf("Couldn't find wad part!\n");

	xTaskCreatePinnedToCore(&doomEngineTask, "doomEngine", 22480, NULL, 5, NULL, 0);
}

void app_main()
{

	spi_lcd_init();
	jsInit();
	
	StartDoom();
	
	/*
	unsigned char *screenbuf;
	
	screenbuf=malloc(320*240);
	assert(screenbuf);
	//screenbuf = 0xfffffffffffffffff;
	memcpy(screenbuf,0x00ffffffffffffffff, 320*240);
	uint16_t *scr=(uint16_t*)screenbuf;
	spi_lcd_send(scr);*/

	//V_FillRect(0, 0, 0, 320, 240, 4);
	//M_DrawString(10, 10, 0, "t");
	//while (1){
	//	asm("nop");
	//}
}

