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


#include <stdlib.h>

#include "doomdef.h"
#include "doomtype.h"
#include "m_argv.h"
#include "d_event.h"
#include "g_game.h"
#include "d_main.h"
#include "gamepad.h"
#include "lprintf.h"


//The gamepad uses keyboard emulation, but for compilation, these variables need to be placed
//somewhere. THis is as good a place as any.
int usejoystick=0;
int joyleft, joyright, joyup, joydown;
// static esp_adc_cal_characteristics_t *adc1_chars;

//atomic, for communication between joy thread and main game thread
volatile int joyValnumY=0;
volatile int joyValnumX=0;
volatile int GpioVal0=0;
volatile int GpioVal1=0;
volatile int GpioVal2=0;
volatile int GpioVal3=0;
volatile int GpioVal4=0;
volatile int GpioVal5=0;
volatile int GpioVal6=0;
volatile int GpioVal7=0;
volatile int lastjoyValX=0;
volatile int lastjoyValY=0;
volatile int joyValX=0;
volatile int joyValY=0;

typedef struct {
	int *GPIO;
	int *key;
} JsKeyMap;

#ifdef OLD_JOYSTICK
//Mappings from PS2 buttons to keys
static const JsKeyMap keymap[]={
	{0x10, &key_up},
	{0x40, &key_down},
	{0x80, &key_left},
	{0x20, &key_right},
	
	{0x4000, &key_use},				//cross
	{0x2000, &key_fire},			//circle
	{0x2000, &key_menu_enter},		//circle
	{0x8000, &key_pause},			//square
	{0x1000, &key_weapontoggle},	//triangle

	{0x8, &key_escape},				//start
	{0x1, &key_map},				//select
	
	{0x400, &key_strafeleft},		//L1
	{0x100, &key_speed},			//L2
	{0x800, &key_straferight},		//R1
	{0x200, &key_strafe},			//R2

	{0, NULL},
};
#endif







void gamepadPoll(void)
{
/*
	
	event_t ev;
	joyValX = adc1_get_raw(ADC1_CHANNEL_5);
	joyValY = adc1_get_raw(ADC1_CHANNEL_4);
	
	//printf("test1-----------------------------------------------------%d, %d\n", joyValY, joyValX);

	//joyValY = (joyValY-125)*65535/4095.0f;
	//joyValX = (joyValX-125)*65535/4095.0f;
	if(joyValX<=500){
		joyValnumX=2;
		ev.type=ev_joystick;
		//ev.data1=*keymap[3].key;
		//printf("gauche\n");
		ev.data2=joyValnumX;

	}else if(joyValX>=4000){
		joyValnumX=-2;
		ev.type=ev_joystick;
		//ev.data1=*keymap[2].key;
		//printf("droite\n");
		ev.data2=joyValnumX;

	}else if(joyValX>500 && joyValX<1500){
		joyValnumX=1;
		ev.type=ev_joystick;
		//ev.data1=*keymap[3].key;
		//printf("gauche\n");
		ev.data2=joyValnumX;
		lastjoyValX = 0;
	}else if(joyValX<4000 && joyValX>3500){
		joyValnumX=-1;
		ev.type=ev_joystick;
		//ev.data1=*keymap[2].key;
		//printf("droite\n");
		ev.data2=joyValnumX;
		lastjoyValX = 0;
	}else if ((1500<=joyValX) && (joyValX<=3500)){
		joyValnumX=0;
		ev.type=ev_joystick;
		//printf("reset X\n");
		ev.data2=joyValnumX;
		lastjoyValX = 0;
	}
	if(joyValY<=500){
		joyValnumY=2;
		ev.type=ev_joystick;
		//ev.data1=*keymap[1].key;
		//printf("Avance\n");
		ev.data3=joyValnumY;
		lastjoyValY = 0;
	}else if(joyValY>=4000){
		joyValnumY=-2;
		ev.type=ev_joystick;
		//ev.data1=*keymap[0].key;
		//printf("recule\n");
		ev.data3=joyValnumY;
	}else if(joyValY>500 && joyValY<1500){
		joyValnumY=1;
		ev.type=ev_joystick;
		//ev.data1=*keymap[1].key;
		//printf("Avance\n");
		ev.data3=joyValnumY;
		lastjoyValY = 0;
	}else if(joyValY<4000 && joyValY>3500){
		joyValnumY=-1;
		ev.type=ev_joystick;
		//ev.data1=*keymap[0].key;
		//printf("recule\n");
		ev.data3=joyValnumY;
	}else if ((1500<=joyValY) && (joyValY<=3500)){
		joyValnumY=0;
		ev.type=ev_joystick;
		ev.data3=joyValnumY;
		//printf("reset Y\n");
	}
	//printf("Print\n");

	D_PostEvent(&ev);
	
	ev.data3=0;
	ev.data2=0;
	//printf("[APP] Free memory: %f Mb\n", (esp_get_free_heap_size())/1000000.0f);
	event_t evButton;
	if (gpio_get_level(GPIO_NUM_18)!=GpioVal0) {
		evButton.type=!gpio_get_level(GPIO_NUM_18)?ev_keyup:ev_keydown;
		evButton.data1=key_fire;
		GpioVal0=gpio_get_level(GPIO_NUM_18);
		//printf("18 is now %d\n", GpioVal0);
		D_PostEvent(&evButton);
	}else if(!gpio_get_level(GPIO_NUM_18)) GpioVal0=0;

	if (gpio_get_level(GPIO_NUM_8)!=GpioVal1) {
		evButton.type=!gpio_get_level(GPIO_NUM_8)?ev_keyup:ev_keydown;
		evButton.data1=key_weapontoggle;
		
		GpioVal1=gpio_get_level(GPIO_NUM_8);
		//printf("8 is now %d\n", GpioVal1);
		D_PostEvent(&evButton);
	}else if(!gpio_get_level(GPIO_NUM_8)) GpioVal1=0;

	if (gpio_get_level(GPIO_NUM_7)!=GpioVal2) {
		evButton.type=!gpio_get_level(GPIO_NUM_7)?ev_keyup:ev_keydown;
		evButton.data1=key_left;
		GpioVal2=gpio_get_level(GPIO_NUM_7);
		//printf("9 is now %d\n", GpioVal2);
		D_PostEvent(&evButton);
	}else if(!gpio_get_level(GPIO_NUM_7)) GpioVal2=0;
		
	if (gpio_get_level(GPIO_NUM_1)!=GpioVal3) {
		evButton.type=!gpio_get_level(GPIO_NUM_1)?ev_keyup:ev_keydown;
		evButton.data1=key_right;
		GpioVal3=gpio_get_level(GPIO_NUM_1);
		//printf("1 is now %d\n", GpioVal3);
		D_PostEvent(&evButton);
	}else if(!gpio_get_level(GPIO_NUM_1)) GpioVal3=0;
			

	if (gpio_get_level(GPIO_NUM_4)!= GpioVal4) {
		evButton.type=gpio_get_level(GPIO_NUM_4)?ev_keyup:ev_keydown;
		evButton.data1=key_autorun;
		GpioVal4=gpio_get_level(GPIO_NUM_4);
		//printf("4 is now %d\n", GpioVal4);
		D_PostEvent(&evButton);
	}else if(gpio_get_level(GPIO_NUM_4)) GpioVal4=1;
		
	if (gpio_get_level(GPIO_NUM_16)!=GpioVal5) {
		evButton.type=!gpio_get_level(GPIO_NUM_16)?ev_keyup:ev_keydown;
		evButton.data1=key_escape;
		GpioVal5=gpio_get_level(GPIO_NUM_16);
		//printf("16 is now %d\n", GpioVal5);
		D_PostEvent(&evButton);
	}else if(!gpio_get_level(GPIO_NUM_16)){
		GpioVal5=0;
	} 
		
	if (gpio_get_level(GPIO_NUM_15)!=GpioVal6) {
		evButton.type=!gpio_get_level(GPIO_NUM_15)?ev_keyup:ev_keydown;
		evButton.data1=key_use;
		GpioVal6=gpio_get_level(GPIO_NUM_15);
		//printf("15 is now %d\n", GpioVal6);
		D_PostEvent(&evButton);
	}else if(!gpio_get_level(GPIO_NUM_15)) GpioVal6=0;
		
	if (gpio_get_level(GPIO_NUM_18)!=GpioVal7) {
		evButton.type=!gpio_get_level(GPIO_NUM_18)?ev_keyup:ev_keydown;
		evButton.data1=key_menu_enter;
		GpioVal7=gpio_get_level(GPIO_NUM_18);
		D_PostEvent(&evButton);
	}else if(!gpio_get_level(GPIO_NUM_18)) GpioVal7=0;

		if (gpio_get_level(GPIO_NUM_15)!=GpioVal6) {
		evButton.type=!gpio_get_level(GPIO_NUM_15)?ev_keyup:ev_keydown;
		evButton.data1='y';
		GpioVal6=gpio_get_level(GPIO_NUM_15);
		//printf("15 is now %d\n", GpioVal6);
		D_PostEvent(&evButton);
	}else if(!gpio_get_level(GPIO_NUM_15)) GpioVal6=0;
	*/
}



void gamepadInit(void)
{
	/*
	adc1_config_channel_atten( ADC1_CHANNEL_5, ADC_ATTEN_DB_11 );
	adc1_config_channel_atten( ADC1_CHANNEL_4, ADC_ATTEN_DB_11 );
	adc1_config_width(ADC_WIDTH_BIT_12);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11));
	adc1_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));

	gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_16, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_6, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_7, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_8, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_18, GPIO_MODE_INPUT);

	lprintf(LO_INFO, "gamepadInit: Initializing game pad.\n");
	*/
}

void jsInit() {
	//Starts the js task
	gamepadInit();
	psxcontrollerInit();
	//xTaskCreatePinnedToCore(&jsTask, "js", 5000, NULL, 7, NULL, 1);
}

