//**************************************************************************//
//                                                                          //
//                            /!\ Warnig /!\                                //
//                                                                          //
//      Some comments may no longer correspond to the real code !           //
//                                                                          //
//**************************************************************************//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spi_master_example_main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <math.h>
#include <rom/ets_sys.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include <esp_timer.h>

float PposX = 10;//player position X
float PposY = 0; //player position Y
float PposZ = 0; //player position Z

float ProtX = 0; //player rotation X
float ProtZ = 0; //player rotation Z

int translationSpeed = 10;

int minAnalogInput = 0;
int maxAnalogInput = 4095;
int minMovement = -10;
int maxMovement = 10;


int fov = 300;
volatile int joyValX=0;
volatile int joyValY=0;

static adc_oneshot_unit_init_cfg_t *adc1_chars;

QueueHandle_t Main_queue;

#define M_PI 3.14159265358979323846   // pi
#define WIDTH 320
#define HEIGHT 240

const int Map[55][4] = {

    // format : {x, y, z, 16bit color}
    {1, 7, 1, 0b1010101010011011},
    {1, 6, 1, 0b1010101010011011},
    {1, 5, 1, 0b1010101010011011},
    {1, 4, 1, 0b1010101010011011},
    {1, 3, 1, 0b1010101010011011},
    {1, 7, 0, 0b1010101010011011},
    {1, 6, 0, 0b1010101010011011},
    {1, 5, 0, 0b1010101010011011},
    {1, 4, 0, 0b1010101010011011},
    {1, 3, 0, 0b1010101010011011},
    {1, 7, -1, 0b1010101010011011},
    {1, 6, -1, 0b1010101010011011},
    {1, 5, -1, 0b1010101010011011},
    {1, 4, -1, 0b1010101010011011},
    {1, 3, -1, 0b1010101010011011},
    {5, 7, 1, 0b1010101010011011},
    {5, 6, 1, 0b1010101010011011},
    {5, 5, 1, 0b1010101010011011},
    {5, 4, 1, 0b1010101010011011},
    {5, 3, 1, 0b1010101010011011},
    {5, 7, 0, 0b1010101010011011},
    {5, 6, 0, 0b1010101010011011},
    {5, 5, 0, 0b1010101010011011},
    {5, 4, 0, 0b1010101010011011},
    {5, 3, 0, 0b1010101010011011},
    {5, 7, -1, 0b1010101010011011},
    {5, 6, -1, 0b1010101010011011},
    {5, 5, -1, 0b1010101010011011},
    {5, 4, -1, 0b1010101010011011},
    {5, 3, -1, 0b1010101010011011},
    {4, 7, -1, 0b1010101010011011},
    {4, 6, -1, 0b1010101010011011},
    {4, 5, -1, 0b1010101010011011},
    {4, 4, -1, 0b1010101010011011},
    {4, 3, -1, 0b1010101010011011},
    {3, 7, -1, 0b1010101010011011},
    {3, 6, -1, 0b1010101010011011},
    {3, 5, -1, 0b1010101010011011},
    {3, 4, -1, 0b1010101010011011},
    {3, 3, -1, 0b1010101010011011},
    {2, 7, -1, 0b1010101010011011},
    {2, 6, -1, 0b1010101010011011},
    {2, 5, -1, 0b1010101010011011},
    {2, 4, -1, 0b1010101010011011},
    {2, 3, -1, 0b1010101010011011},
    {2, 7, 0, 0b1010101010011011},
    {3, 7, 0, 0b1010101010011011},
    {4, 7, 0, 0b1010101010011011},
    {2, 7, 1, 0b1010101010011011},
    {3, 7, 1, 0b1010101010011011},
    {4, 7, 1, 0b1010101010011011},
    {2, 3, 0, 0b1010101010011011},
    {4, 3, 0, 0b1010101010011011},
    {2, 3, 1, 0b1010101010011011},
    {4, 3, 1, 0b1010101010011011}
};

int verticesConTable[12][2] = {

    {0 , 1},
    {1 , 3},
    {2 , 3},
    {2 , 0},
    {4 , 5},
    {5 , 7},
    {6 , 7},
    {6 , 4},
    {0 , 4},
    {1 , 5},
    {2 , 6},
    {3 , 7}
};

int FacesConTable[6][4] = {
    {0 , 1, 3, 2},
    {4 , 5, 7, 6},
    {2 , 3, 6, 7},
    {0 , 1, 4, 5},
    {0 , 3, 6, 4},
    {1 , 2, 7, 6}
};

// Fonction auxiliaire pour calculer l'aire d'un triangle formé par trois points
// Auxiliary function to calculate the area of ??a triangle formed by three points
int calculate_area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
}

// Fonction auxiliaire pour vérifier si un point (x, y) est à l'intérieur du carré
// Auxiliary function to check if a point (x,y) is inside the square
int is_point_inside_square(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    // Utiliser l'algorithme du produit vectoriel pour vérifier si le point est à l'intérieur du triangle formé par les sommets du carré
    // Use the cross product algorithm to check if the point is inside the triangle formed by the vertices of the square
    int area1 = calculate_area(x, y, x1, y1, x2, y2);
    int area2 = calculate_area(x, y, x2, y2, x3, y3);
    int area3 = calculate_area(x, y, x3, y3, x4, y4);
    int area4 = calculate_area(x, y, x4, y4, x1, y1);

    // Le point est à l'intérieur du carré si les aires des triangles sont toutes positives
    // The point is inside the square if the areas of the triangles are all positive
    return (area1 >= 0 && area2 >= 0 && area3 >= 0 && area4 >= 0);
}



void draw_square(uint16_t color, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint16_t *buffer) {
    // Calculer les coordonnées du rectangle englobant
    // Calculate the coordinates of the bounding rectangle
    
    int minX = x1;
    int maxX = x1;
    int minY = y1;
    int maxY = y1;

    if (x2 < minX) minX = x2;
    if (x2 > maxX) maxX = x2;
    if (y2 < minY) minY = y2;
    if (y2 > maxY) maxY = y2;

    if (x3 < minX) minX = x3;
    if (x3 > maxX) maxX = x3;
    if (y3 < minY) minY = y3;
    if (y3 > maxY) maxY = y3;

    if (x4 < minX) minX = x4;
    if (x4 > maxX) maxX = x4;
    if (y4 < minY) minY = y4;
    if (y4 > maxY) maxY = y4;

    if (minX >= WIDTH || minY >= HEIGHT || maxX < 0 || maxY < 0)
        return;
    
    // Dessiner le carré
    // Draw the square
    
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            if(x<WIDTH&&x>=0&&y<HEIGHT&&y>=0){
                if(buffer+(y*WIDTH+x) != color){
                    memcpy(buffer+(y*WIDTH+x), &color, sizeof(uint16_t));
                    //if (is_point_inside_square(x, y, x1, y1, x2, y2, x3, y3, x4, y4)) {}
                }
            }
        }
    }
}




int draw_cube(int centerX, int centerY, int centerZ, int width, int height, int length, uint16_t texture, uint16_t *buffer){

    float halfLength = length / 2.0;
    float halfHeight = height / 2.0;
    float halfWidth = width / 2.0;

    // Calculate the cube's vertices
    float vertices[8][3] = {
        { centerX - halfLength, centerY - halfHeight, centerZ - halfWidth},
        { centerX + halfLength, centerY - halfHeight, centerZ - halfWidth},
        { centerX - halfLength, centerY + halfHeight, centerZ - halfWidth},
        { centerX + halfLength, centerY + halfHeight, centerZ - halfWidth},
        { centerX - halfLength, centerY - halfHeight, centerZ + halfWidth},
        { centerX + halfLength, centerY - halfHeight, centerZ + halfWidth},
        { centerX - halfLength, centerY + halfHeight, centerZ + halfWidth},
        { centerX + halfLength, centerY + halfHeight, centerZ + halfWidth}
    };



 
    float rotationXRad = ProtX * M_PI / 180.0;
    float rotationZRad = ProtZ * M_PI / 180.0;
    for (int i = 0; i < 8; i++) {
        // Translate vertices relative to PposX, PposY, PposZ
        float translatedX = vertices[i][0] - PposX;
        float translatedY = vertices[i][1] - PposY;
        float translatedZ = vertices[i][2] - PposZ;

        // Rotate around X axis
        float rotatedY = cos(rotationXRad) * translatedY - sin(rotationXRad) * translatedZ;
        float rotatedZ = sin(rotationXRad) * translatedY + cos(rotationXRad) * translatedZ;

        // Rotate around Z axis
        float rotatedX = cos(rotationZRad) * translatedX - sin(rotationZRad) * rotatedY;
        rotatedY = sin(rotationZRad) * translatedX + cos(rotationZRad) * rotatedY;

        // Translate vertices back to their original position
        vertices[i][0] = rotatedX + PposX;
        vertices[i][1] = rotatedY + PposY;
        vertices[i][2] = rotatedZ + PposZ;
    }

    int points[8][2];
    for (int i = 0; i < 8; i++) {
        float relativeX = vertices[i][0] - PposX;
        float relativeY = vertices[i][1] - PposY;
        float relativeZ = vertices[i][2] - PposZ;

        if((relativeY)==0) return -1;
        if((relativeY)<0) return -1;

        float projectedX = (fov * relativeX) / relativeY;
        float projectedY = (fov * relativeZ) / relativeY;

        // Convert the projected coordinates to screen space
        points[i][0] = projectedX + WIDTH / 2;
        points[i][1] = projectedY + HEIGHT / 2;
    }
    

    // disabled for further testing and optimization of other functions

    /*for(int i = 0; i < 6; i++){
        draw_square(texture, 
                    points[FacesConTable[i][0]][0], 
                    points[FacesConTable[i][0]][1],
                    points[FacesConTable[i][1]][0], 
                    points[FacesConTable[i][1]][1], 
                    points[FacesConTable[i][2]][0], 
                    points[FacesConTable[i][2]][1], 
                    points[FacesConTable[i][3]][0], 
                    points[3][1], 
                    buffer);
    }*/
    //draw_square(texture, points[4][0], points[4][1], points[5][0], points[5][1], points[2][0], points[2][1], points[3][0], points[3][1], buffer);

    for(int k = 0; k<12; k++) {
        
        int x1 = points[verticesConTable[k][0]][0];
        int y1 = points[verticesConTable[k][0]][1];
        int x2 = points[verticesConTable[k][1]][0];
        int y2 = points[verticesConTable[k][1]][1];
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = x1 < x2 ? 1 : -1;
        int sy = y1 < y2 ? 1 : -1;
        int err = dx - dy;
        //printf("x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", x1, y1, x2, y2);
        while (x1 != x2 || y1 != y2)
        {
            if(x1<WIDTH&&x1>=0&&y1<HEIGHT&&y1>=0) memcpy(buffer+(y1 * WIDTH + x1) , &texture, sizeof(uint16_t));

            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y1 += sy;
            }
        }
    }
    return 0;
}





int Build_terrain(uint16_t* buffer){

    // color code : G2, G1, G0, B4, B3, B2, B1, B0, R4, R3, R2, R1, R0, G5, G4, G3
    int16_t bite = 0b1100000001110111; //(sry)
    
    for (int i = 0; i <  120*320 ; i++) {
        memcpy(buffer+120*320 + i, &bite, sizeof(uint16_t));
    }
    bite = 0b0111111110000110;
    for (int i = 0; i <  120*320 ; i++) {
        memcpy(buffer + i, &bite, sizeof(uint16_t));
    }
    return 0;
}

int DrawHouse(uint16_t * vid_buf){

    for(int i = 0; i < 55; i++){
        draw_cube(Map[i][0]*100, Map[i][1]*100, Map[i][2]*100, 100, 100, 100, Map[i][3], vid_buf);
    }
    return 0;
}

TaskHandle_t xHandle = NULL;



void displaytasks(){

    uint16_t * vid_buf = NULL;
    vid_buf = (uint16_t *)malloc(320*240*sizeof(uint16_t));
    if(!vid_buf){
        printf("Failed to allocate memory");
        return;
    }
    int currentMillis=1;
    int currentMillisGlobal=1;
    
    int lastMillis=1;
    int lastMillisGlobal=1;

    int elapsedMillisGlobal=1;
    int elapsedMillisInput=1;
    int elapsedMillisMemset=1;
    int elapsedMillisCalculs1=1;
    int elapsedMillisCalculs2=1;
    int elapsedMillisCalculs3=1;
    int elapsedMillisDraw=1;
    float fps=1;
    while(1){

        lastMillisGlobal = currentMillisGlobal;
        currentMillisGlobal = esp_timer_get_time();
        elapsedMillisGlobal = currentMillisGlobal-lastMillisGlobal;
        fps = 1000000.0f/elapsedMillisGlobal;

        currentMillis = esp_timer_get_time();

        joyValX=adc1_get_raw(ADC1_CHANNEL_5);
        joyValY=adc1_get_raw(ADC1_CHANNEL_4);
        //printf("calibration : %d, %d\n", joyValX, joyValY);
        // Convertir les valeurs analogiques en une plage de mouvement appropriée
        // Convert analog values ??to proper motion range
        int rangeAnalogInput = maxAnalogInput - minAnalogInput;
        int rangeMovement = maxMovement - minMovement;

        // Définir la plage de mouvement du joueur
        // Set player movement range

        if(joyValY>2100||joyValY<1900||joyValX>2100||joyValX<1900){

            int movementX = ((joyValY - minAnalogInput) * rangeMovement) / rangeAnalogInput + minMovement;
            int movementY = ((joyValX - minAnalogInput) * rangeMovement) / rangeAnalogInput + minMovement;

            // Mettre à jour la position du joueur en fonction du mouvement
            PposX += movementX * sin(ProtZ * M_PI / 180.0) / (fps/20);
            PposY += movementX * cos(ProtZ * M_PI / 180.0) / (fps/20);
            ProtZ -= movementY/5 / (fps/20);

        }
        if (gpio_get_level(GPIO_NUM_7) == 1) {
            PposX -= translationSpeed * cos(ProtZ * M_PI / 180.0) / (fps/20);
            PposY += translationSpeed * sin(ProtZ * M_PI / 180.0) / (fps/20);
        }

        if (gpio_get_level(GPIO_NUM_1) == 1) {
            PposX += translationSpeed * cos(ProtZ * M_PI / 180.0) / (fps/20);
            PposY -= translationSpeed * sin(ProtZ * M_PI / 180.0) / (fps/20);
        }

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisInput = currentMillis-lastMillis;

        //memset(vid_buf, 0, 320*240*sizeof(uint16_t));
        Build_terrain(vid_buf);
        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisMemset = currentMillis-lastMillis;

        DrawHouse(vid_buf);

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisCalculs1 = currentMillis-lastMillis;
/*         
        draw_cube(-200, 300, -50, 100, 100, 100, 0x44, vid_buf);

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisCalculs2 = currentMillis-lastMillis;

        draw_cube(-50, 300, -50, 100, 100, 100, 0xffff, vid_buf);
*/
        
    

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisCalculs3 = currentMillis-lastMillis;

        xQueueOverwrite(Main_queue, vid_buf);

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisDraw = currentMillis-lastMillis;



        printf("fps : %f, Input time : %f, Memset Time : %f, Calcul Time 1 : %f, Calcul Time 2 : %f, Calcul Time 3 : %f, Display Time : %f, Global Time : %f\n", fps, (elapsedMillisInput/1000.0f), (elapsedMillisMemset/1000.0f), (elapsedMillisCalculs1/1000.0f), (elapsedMillisCalculs2/1000.0f), (elapsedMillisCalculs3/1000.0f), (elapsedMillisDraw/1000.0f), (elapsedMillisGlobal/1000.0f));
        //ets_delay_us(10000);

        
    }
}


void display_ext(){
    uint16_t * vid_buf = NULL;
    //printf("Je suis laaaaa\n");
    vid_buf = (uint16_t *)malloc(320*240*sizeof(uint16_t));
    if(!vid_buf){
        printf("Failed to allocate memory");
        return;
    }
    memset(vid_buf, 0, 320*240*sizeof(uint16_t));

    while(true){
        if(xQueuePeek(Main_queue, vid_buf, 0)!= pdTRUE){
            printf("erreur\n");
        }
        //printf("Je suis la\n");
        display_buff_ext(vid_buf);

    }
    free(vid_buf);

}


void app_main(void)
{
    spi_main();



    gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_16, GPIO_MODE_INPUT);
	gpio_set_direction(GPIO_NUM_5, GPIO_MODE_INPUT); 
	gpio_set_direction(GPIO_NUM_6, GPIO_MODE_INPUT); 
	gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT); 
	gpio_set_direction(GPIO_NUM_7, GPIO_MODE_INPUT); 
	gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT); 
	gpio_set_direction(GPIO_NUM_8, GPIO_MODE_INPUT); 
	gpio_set_direction(GPIO_NUM_18, GPIO_MODE_INPUT);


	adc1_config_channel_atten( ADC1_CHANNEL_5, ADC_ATTEN_DB_11 );
	adc1_config_channel_atten( ADC1_CHANNEL_4, ADC_ATTEN_DB_11 );
	adc1_config_width(ADC_WIDTH_BIT_12);
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11));
	adc1_chars = (esp_adc_cal_characteristics_t *)malloc(sizeof(esp_adc_cal_characteristics_t));

    Main_queue = xQueueCreate(1,320*240*sizeof(uint16_t));

    
    xTaskCreatePinnedToCore(&display_ext, "display_ext", 10000, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(&displaytasks, "display_task", 22480, NULL, 5, NULL, 0);
    
    
}