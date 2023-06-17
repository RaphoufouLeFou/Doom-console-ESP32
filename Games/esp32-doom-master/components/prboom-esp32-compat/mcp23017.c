// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include <mcp23x17.h>
#include <driver/gpio.h>
#include <string.h>


#include "driver/gpio.h"
#include "soc/gpio_struct.h"

#include "sdkconfig.h"


#define DELAY() asm("nop; nop; nop; nop;nop; nop; nop; nop;nop; nop; nop; nop;nop; nop; nop; nop;")

// #define INTA_GPIO 19  // INTA pin
#define SDA_GPIO 22
#define SCL_GPIO 21

 #define CONFIG_HW_MCPKEYB_ENA 1

#if CONFIG_HW_MCPKEYB_ENA

    mcp23x17_t dev;


    uint16_t val;
    uint32_t lev;


int keybReadInput() {

        
        printf("read val = %x\n", val);
//        mcp23x17_get_level(&dev, 6, &lev);
//        printf("level val = %x\n", lev);


	return val;
}


void keybControllerInit() {

}


#else

int keybReadInput() {
	return 0xFFFF;
}


void keybControllerInit() {
	printf("MCP controller disabled in menuconfig; no input enabled.\n");
}

#endif

