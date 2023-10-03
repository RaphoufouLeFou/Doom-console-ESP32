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
#include "LCD_startup.h"
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

int renderDistance = 2000000;

int fov = 300;


volatile int joyValXrender=0;
volatile int joyValYrender=0;

static adc_oneshot_unit_init_cfg_t *adc1_chars;

QueueHandle_t Main_queue;

#define M_PI 3.14159265358979323846   // pi
#define DEG_TO_RAD 3.14159265358979323846/180.0
#define WIDTH 320
#define HEIGHT 240

#define BROWN 0b1010101010011011

uint16_t texture_dirt[8][8] = {
    {(0x7AA7),(0x7AA7),(0x9369),(0x59E5),(0xBC2B),(0x7AA7),(0x9369),(0x9369)},
    {(0x7AA7),(0x59E5),(0x59E5),(0x59E5),(0x59E5),(0x59E5),(0x59E5),(0x7AA7)},
    {(0x59E5),(0x9369),(0x9369),(0x59E5),(0x7AA7),(0x59E5),(0x7AA7),(0xBC2B)},
    {(0x7AA7),(0xBC2B),(0x9369),(0x7AA7),(0x59E5),(0x9369),(0x7AA7),(0x9369)},
    {(0x9369),(0x7AA7),(0x59E5),(0x7AA7),(0x7AA7),(0x7AA7),(0xBC2B),(0x9369)},
    {(0x7AA7),(0x9369),(0xBC2B),(0x9369),(0x9369),(0x7AA7),(0x9369),(0x59E5)},
    {(0x9369),(0x7AA7),(0x7AA7),(0x9369),(0x59E5),(0x59E5),(0xBC2B),(0x9369)},
    {(0x7AA7),(0x9369),(0x7AA7),(0x7AA7),(0x9369),(0x7AA7),(0x9369),(0x59E5)}
};

uint8_t * IsHided = NULL;

typedef struct cube {
    int centerX;
    int centerY;
    int centerZ;
    int length;
    int width;
    int height;
    float distToPlayer;
    uint16_t color;
}cube;


const int Map[55][4] = {

    // format : {x, y, z, 16bit color}
    {1, 7, 1, BROWN},
    {1, 6, 1, BROWN},
    {1, 5, 1, BROWN},
    {1, 4, 1, BROWN},
    {1, 3, 1, BROWN},
    {1, 7, 0, BROWN},
    {1, 6, 0, BROWN},
    {1, 5, 0, BROWN},
    {1, 4, 0, BROWN},
    {1, 3, 0, BROWN},
    {1, 7, -1, BROWN},
    {1, 6, -1, BROWN},
    {1, 5, -1, BROWN},
    {1, 4, -1, BROWN},
    {1, 3, -1, BROWN},
    {5, 7, 1, BROWN},
    {5, 6, 1, BROWN},
    {5, 5, 1, BROWN},
    {5, 4, 1, BROWN},
    {5, 3, 1, BROWN},
    {5, 7, 0, BROWN},
    {5, 6, 0, BROWN},
    {5, 5, 0, BROWN},
    {5, 4, 0, BROWN},
    {5, 3, 0, BROWN},
    {5, 7, -1, BROWN},
    {5, 6, -1, BROWN},
    {5, 5, -1, BROWN},
    {5, 4, -1, BROWN},
    {5, 3, -1, BROWN},
    {4, 7, -1, BROWN},
    {4, 6, -1, BROWN},
    {4, 5, -1, BROWN},
    {4, 4, -1, BROWN},
    {4, 3, -1, BROWN},
    {3, 7, -1, BROWN},
    {3, 6, -1, BROWN},
    {3, 5, -1, BROWN},
    {3, 4, -1, BROWN},
    {3, 3, -1, BROWN},
    {2, 7, -1, BROWN},
    {2, 6, -1, BROWN},
    {2, 5, -1, BROWN},
    {2, 4, -1, BROWN},
    {2, 3, -1, BROWN},
    {2, 7, 0, BROWN},
    {3, 7, 0, BROWN},
    {4, 7, 0, BROWN},
    {2, 7, 1, BROWN},
    {3, 7, 1, BROWN},
    {4, 7, 1, BROWN},
    {2, 3, 0, BROWN},
    {4, 3, 0, BROWN},
    {2, 3, 1, BROWN},
    {4, 3, 1, BROWN}
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
    {2 , 3, 7, 6},
    {0 , 1, 5, 4},
    {1 , 3, 7, 5},
    {0 , 2, 6, 4}                               
};

/*
        2-------3 -  -  -  -  - 6-------7
        |       |               |       |
        |       | BAS           |       | HAUT
        0-------1-  -  -  -  -  4-------5
*/

// Fonction auxiliaire pour calculer l'aire d'un triangle form� par trois points
// Auxiliary function to calculate the area of ??a triangle formed by three points
int calculate_area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
}

// Fonction auxiliaire pour v�rifier si un point (x, y) est � l'int�rieur du carr�
// Auxiliary function to check if a point (x,y) is inside the square
int is_point_inside_square(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    // Utiliser l'algorithme du produit vectoriel pour v�rifier si le point est � l'int�rieur du triangle form� par les sommets du carr�
    // Use the cross product algorithm to check if the point is inside the triangle formed by the vertices of the square
    int area1 = calculate_area(x, y, x1, y1, x2, y2);
    int area2 = calculate_area(x, y, x2, y2, x3, y3);
    int area3 = calculate_area(x, y, x3, y3, x4, y4);
    int area4 = calculate_area(x, y, x4, y4, x1, y1);

    // Le point est � l'int�rieur du carr� si les aires des triangles sont toutes positives
    // The point is inside the square if the areas of the triangles are all positive
    return (area1 >= 0 && area2 >= 0 && area3 >= 0 && area4 >= 0);
}


float map(float x, long in_min, long in_max, long out_min, long out_max)
{
    if (in_max == in_min) return 0;
    return ((((x - in_min) * (out_max - out_min)) / (in_max - in_min)) + out_min);
}




void draw_square(uint16_t color, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, uint16_t* buffer) {

    // Calculer les coordonnees du rectangle englobant
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

    int minBisX = minX;
    int minBisY = minY;
    int maxBisX = maxX;
    int maxBisY = maxY;

    if (minX >= WIDTH || minY >= HEIGHT || maxX < 0 || maxY < 0)
        return;

    if (maxX > WIDTH) 
        maxX = WIDTH;
    
    if (maxY > HEIGHT) 
        maxY = HEIGHT;
    
    if (minX < 0) 
        minX = 0;
    
    if (minY < 0) 
        minY = 0;
    
    // Draw the texture

    for (int x = minX; x < maxX; x++) {
        for (int y = minY; y < maxY; y++) {
            if (IsHided[y * WIDTH + x] == 0) {

                // Check if the pixel is inside the square and determine his boundaries

                float u = (x - minBisX) / (maxBisX - minBisX);
                float u2 = u;

                if (x1 > x2 || x4 > x3) {
                    u = y1 < y2 ? 1 - u : u;
                    u2 = y4 < y3 ? 1 - u2 : u2;
                }
                else
                {
                    u = y1 < y2 ? u : 1 - u;
                    u2 = y4 < y3 ? u2 : 1 - u2;
                }

                
                float Ytmin = (y1 < y2 ? (y2 - y1) * u + y1 : (y1 - y2) * u + y2);
                float Ytmax = (y4 < y3 ? (y3 - y4) * u2 + y4 : (y4 - y3) * u2 + y3);


                if (x1 == x4 && x3 == x2) {

                    if (y >= Ytmin && y <= Ytmax) {

                        // calculate the index of the pixel in the texture

                        int _x = ((x - minBisX)*7) / (maxBisX - minBisX);
                        int _y = ((y - Ytmin)*7) / (Ytmax - Ytmin);

                        // change the color format of the pixel from
                        // R4, R3, R2, R1, R0, G5, G4, G3, G2, G1, G0, B4, B3, B2, B1, B0
                        // to
                        // G2, G1, G0, B4, B3, B2, B1, B0, R4, R3, R2, R1, R0, G5, G4, G3
                        // beacause the screen use a different color format

                        uint16_t L = (texture_dirt[_x][_y] >> 8;
                        uint16_t H = (texture_dirt[_x][_y] << 8 ;

                        // send the pixel to the buffer

                        buffer[(y * WIDTH + x)] =  L | H;

                        // mark the pixel as drawn, so the same pixel of cube behind this one will not be calculated for optimization

                        IsHided[y * WIDTH + x] = 1;
                        
                   
                    }

                }
            }
        }
    }
}


// function to calculate the square distance between two points 

float calc_diff(int posX, int posY, int posZ, int playerX, int playerY, int playerZ) {
    float diffXY = (playerX - posX) * (playerX - posX) + (playerY - posY) * (playerY - posY);
    float diffTot = diffXY + (playerZ - posZ) * (playerZ - posZ);
    return diffTot;
}


// Function to draw a cube by projecting its vertices onto the screen

int draw_cube(cube Cube, uint16_t* buffer) {

    float halfLength = Cube.length / 2.0;
    float halfHeight = Cube.height / 2.0;
    float halfWidth = Cube.width / 2.0;

    // Calculate the cube's vertices
    float vertices[8][3] = {
        { Cube.centerX - halfLength, Cube.centerY - halfHeight, Cube.centerZ - halfWidth},
        { Cube.centerX + halfLength, Cube.centerY - halfHeight, Cube.centerZ - halfWidth},
        { Cube.centerX - halfLength, Cube.centerY + halfHeight, Cube.centerZ - halfWidth},
        { Cube.centerX + halfLength, Cube.centerY + halfHeight, Cube.centerZ - halfWidth},
        { Cube.centerX - halfLength, Cube.centerY - halfHeight, Cube.centerZ + halfWidth},
        { Cube.centerX + halfLength, Cube.centerY - halfHeight, Cube.centerZ + halfWidth},
        { Cube.centerX - halfLength, Cube.centerY + halfHeight, Cube.centerZ + halfWidth},
        { Cube.centerX + halfLength, Cube.centerY + halfHeight, Cube.centerZ + halfWidth}
    };




    float rotationXRad = ProtX * DEG_TO_RAD;
    float rotationZRad = ProtZ * DEG_TO_RAD;
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

        // Project the vertices onto the screen

        // Calculate the relative coordinates of the vertex
        float relativeX = vertices[i][0] - PposX;
        float relativeY = vertices[i][1] - PposY;
        float relativeZ = vertices[i][2] - PposZ;

        // Check if the vertex is behind the player
        if ((relativeY) < -halfLength) return -1;
        if ((relativeY) == 0) relativeY = 0.1;
        if ((relativeY) < 0) relativeY = -relativeY;
        

        // Calculate the projected coordinates
        float projectedX = (fov * relativeX) / relativeY;
        float projectedY = (fov * relativeZ) / relativeY;

        // Convert the projected coordinates to screen space
        points[i][0] = projectedX + (WIDTH / 2);
        points[i][1] = projectedY + (HEIGHT / 2);
    }

    // determine which faces of the cube are hidden by other cubes 

    int faceNotDoDraw[6] = { 0, 0, 0, 0, 0, 0 };
    
    for (int i = 0; i < 55; i++) {
        float dist = calc_diff(Cube.centerX, Cube.centerY, Cube.centerZ, Map[i][0] * 100, Map[i][1] * 100, Map[i][2] * 100);
        if (dist == 100*100) { //distance is squared

            if (Cube.centerX == Map[i][0] * 100 + 100) {
                faceNotDoDraw[5] = 1;
            }
            else if (Cube.centerX == Map[i][0] * 100 - 100) {
                faceNotDoDraw[4] = 1;
            }
            else if (Cube.centerY == Map[i][1] * 100 + 100) {
                faceNotDoDraw[3] = 1;
            }
            else if (Cube.centerY == Map[i][1] * 100 - 100) {
                faceNotDoDraw[2] = 1;
            }
            else if (Cube.centerZ == Map[i][2] * 100 + 100) {
                faceNotDoDraw[0] = 1;
            }
            else if (Cube.centerZ == Map[i][2] * 100 - 100) {
                faceNotDoDraw[1] = 1;
            }
        }
    }


   // Draw the cube's faces in order of distance from the player 
    float distances[6];

    for (int i = 0; i < 6; i++) {
        distances[i] = calc_diff(
            (vertices[FacesConTable[i][0]][0] + vertices[FacesConTable[i][2]][0])/2,
            (vertices[FacesConTable[i][0]][1] + vertices[FacesConTable[i][2]][1])/2,
            (vertices[FacesConTable[i][0]][2] + vertices[FacesConTable[i][2]][2])/2,
            PposX, PposY, PposZ
        );
    }

    // sort the faces by distance

    float minDist1 = distances[0];
    int maxFace1 = 0;
    float minDist2 = distances[1];
    int maxFace2 = 1;
    float minDist3 = distances[2];
    int maxFace3 = 2;

    for (int i = 1; i < 6; i++) {
        if (distances[i] < minDist1) {
            minDist1 = distances[i];
            maxFace1 = i;
        }
    }
    for (int i = 1; i < 6; i++) {
        if(i != maxFace1){
            if (distances[i] < minDist2) {
                minDist2 = distances[i];
                maxFace2 = i;
            }
        }
    }
    for (int i = 1; i < 6; i++) {
        if (i != maxFace1 && i != maxFace2) {
            if (distances[i] < minDist3) {
                minDist3 = distances[i];
                maxFace3 = i;
            }
        }
    }

    // draw the faces in order of distance

    if (faceNotDoDraw[maxFace1] == 0) {
        draw_square(Cube.color,
            points[FacesConTable[maxFace1][0]][0],
            points[FacesConTable[maxFace1][0]][1],
            points[FacesConTable[maxFace1][1]][0],
            points[FacesConTable[maxFace1][1]][1],
            points[FacesConTable[maxFace1][2]][0],
            points[FacesConTable[maxFace1][2]][1],
            points[FacesConTable[maxFace1][3]][0],
            points[FacesConTable[maxFace1][3]][1],
            buffer);
    }


    if (faceNotDoDraw[maxFace2] == 0) {
        draw_square(Cube.color,
            points[FacesConTable[maxFace2][0]][0],
            points[FacesConTable[maxFace2][0]][1],
            points[FacesConTable[maxFace2][1]][0],
            points[FacesConTable[maxFace2][1]][1],
            points[FacesConTable[maxFace2][2]][0],
            points[FacesConTable[maxFace2][2]][1],
            points[FacesConTable[maxFace2][3]][0],
            points[FacesConTable[maxFace2][3]][1],
            buffer);
    }
    
    if (faceNotDoDraw[maxFace3] == 0) {
        draw_square(Cube.color,
            points[FacesConTable[maxFace3][0]][0],
            points[FacesConTable[maxFace3][0]][1],
            points[FacesConTable[maxFace3][1]][0],
            points[FacesConTable[maxFace3][1]][1],
            points[FacesConTable[maxFace3][2]][0],
            points[FacesConTable[maxFace3][2]][1],
            points[FacesConTable[maxFace3][3]][0],
            points[FacesConTable[maxFace3][3]][1],
            buffer);
    }



    // enabled only for testing and optimization of other functions
    // but it draws the cube's edges

    /*
    for (int k = 0; k < 12; k++) {

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
            uint16_t col = 0b0;
            if (x1 < WIDTH && x1 >= 0 && y1 < HEIGHT && y1 >= 0) memcpy(buffer + (y1 * WIDTH + x1), &col, sizeof(uint16_t));

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
    */

    return 0;
}

// Function to compare cubes by distance

int compareCubes(const void* a, const void* b) {
    const cube* cubeA = (const cube*)a;
    const cube* cubeB = (const cube*)b;

    if (cubeA->distToPlayer < cubeB->distToPlayer) {
        return -1;
    }
    else if (cubeA->distToPlayer > cubeB->distToPlayer) {
        return 1;
    }
    else {
        return 0;
    }
}



// Function to sort and draw cubes
void sortAndDrawCubes(cube* cubes, int numCubes, uint16_t* vid_buf) {

    // Sort the cubes by distance
    qsort(cubes, numCubes, sizeof(cube), compareCubes);

    // Draw the sorted cubes
    for (int i = 0; i < numCubes; i++) {
        draw_cube(cubes[i], vid_buf);
    }
}

//draws the backgroud terrain

int Build_terrain(uint16_t* buffer){

    // color code : G2, G1, G0, B4, B3, B2, B1, B0, R4, R3, R2, R1, R0, G5, G4, G3
    int16_t bite = 0b1100000001110111; //(sry)     // green grass
    
    for (int i = HEIGHT / 2 * WIDTH; i < HEIGHT * WIDTH; i++) {
        buffer[i] = bite;
    }

    bite = 0b0111111110000110;      //blue sky

    for (int i = 0; i < HEIGHT / 2 * WIDTH; i++) {
        buffer[i] = bite;
    }
    return 0;
}

//draws the house with the Map[] array of cubes

int DrawHouse(uint16_t * vid_buf) {

    cube cubes[55];

    
    for (int i = 0; i < 55; i++) {

        cubes[i].centerX = Map[i][0] * 100;
        cubes[i].centerY = Map[i][1] * 100;
        cubes[i].centerZ = Map[i][2] * 100;
        cubes[i].height = cubes[i].length = cubes[i].width = 100;
        cubes[i].color = Map[i][3];
        cubes[i].distToPlayer = calc_diff(cubes[i].centerX, cubes[i].centerY, cubes[i].centerZ, PposX, PposY, PposZ);
       
    }
    
    memset(IsHided, 0, WIDTH*HEIGHT*sizeof(uint8_t));
    sortAndDrawCubes(cubes, 55, vid_buf);
    return 0;
}

TaskHandle_t xHandle = NULL;


// main function to display the game

void displaytasks(){

    // initialize the buffers

    uint16_t * vid_buf = NULL;
    vid_buf = (uint16_t *)malloc(WIDTH*HEIGHT*sizeof(uint16_t));
    if(!vid_buf){
        printf("Failed to allocate memory");
        return;
    }

    IsHided = (uint8_t *)calloc(WIDTH*HEIGHT, sizeof(uint8_t));
    if(!IsHided){
        printf("Failed to allocate memory");
        return;
    }


    // initialize the times variables
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

    // start the loop
    while(1){

        //get time since last frame
        lastMillisGlobal = currentMillisGlobal;
        currentMillisGlobal = esp_timer_get_time();
        elapsedMillisGlobal = currentMillisGlobal-lastMillisGlobal;
        fps = 1000000.0f/elapsedMillisGlobal;

        currentMillis = esp_timer_get_time();

        //read the joystick values
        joyValXrender=adc1_get_raw(ADC1_CHANNEL_5);
        joyValYrender=adc1_get_raw(ADC1_CHANNEL_4);

        // range of the joystick values
        int rangeAnalogInput = maxAnalogInput - minAnalogInput;
        int rangeMovement = maxMovement - minMovement;

        // calculate the movement of the player based of the joystick values

        if(joyValYrender>2100||joyValYrender<1900||joyValXrender>2100||joyValXrender<1900){

            int movementX = ((joyValYrender - minAnalogInput) * rangeMovement) / rangeAnalogInput + minMovement;
            int movementY = ((joyValXrender - minAnalogInput) * rangeMovement) / rangeAnalogInput + minMovement;

            // Mettre � jour la position du joueur en fonction du mouvement
            PposX += movementX * sin(ProtZ * M_PI / 180.0) / (fps/20);
            PposY += movementX * cos(ProtZ * M_PI / 180.0) / (fps/20);
            ProtZ -= movementY/5 / (fps/20);

        }

        // read the buttons values and calculate the translation of the player based of the buttons values

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

        // draw the background and the house
        Build_terrain(vid_buf);

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisMemset = currentMillis-lastMillis;

        DrawHouse(vid_buf);

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisCalculs1 = currentMillis-lastMillis;
/*         
        cube Cube;
        Cube.centerX = 100;
        Cube.centerY = 100;
        Cube.centerZ = 000;
        Cube.height = Cube.length = Cube.width = 100;
        Cube.color = BROWN;
        Cube.distToPlayer = calc_diff(Cube.centerX, Cube.centerY, Cube.centerZ, PposX, PposY, PposZ);
       draw_cube(Cube, vid_buf);

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisCalculs2 = currentMillis-lastMillis;
*/
        
        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisCalculs3 = currentMillis-lastMillis;

        // send the buffer to the display task on the second core

        xQueueOverwrite(Main_queue, vid_buf);

        lastMillis = currentMillis;
        currentMillis = esp_timer_get_time();
        elapsedMillisDraw = currentMillis-lastMillis;


        // print some debug informations
        printf("fps : %f, Input time : %f, Memset Time : %f, Calcul Time 1 : %f, Calcul Time 2 : %f, Calcul Time 3 : %f, Display Time : %f, Global Time : %f\n", fps, (elapsedMillisInput/1000.0f), (elapsedMillisMemset/1000.0f), (elapsedMillisCalculs1/1000.0f), (elapsedMillisCalculs2/1000.0f), (elapsedMillisCalculs3/1000.0f), (elapsedMillisDraw/1000.0f), (elapsedMillisGlobal/1000.0f));


        
    }
}


void display_ext(){

    //get the buffer from the firt core and display it

    uint16_t * vid_buf = NULL;
    vid_buf = (uint16_t *)malloc(320*240*sizeof(uint16_t));
    if(!vid_buf){
        printf("Failed to allocate memory");
        return;
    }
    memset(vid_buf, 0, 320*240*sizeof(uint16_t));
    int err = 0;
    while(true){

        // chech for errors
        err = xQueuePeek(Main_queue, vid_buf, 0);
        if(err != pdTRUE){
            printf("error : %d\n", err);
        }
        // send the buffer to the display
        display_buff_ext(vid_buf);

    }
    free(vid_buf);

}


void Start_Render_test(void)
{


    // initialize the joystick and buttons pins
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

    // create the tasks
    xTaskCreatePinnedToCore(&display_ext, "display_ext", 10000, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(&displaytasks, "display_task", 22480, NULL, 5, NULL, 0);
    
    
}
