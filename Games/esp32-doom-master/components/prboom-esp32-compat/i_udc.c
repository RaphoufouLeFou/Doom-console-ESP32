
#include <sys/param.h>














#include "protocol.h"

#define CONSOLE_SERV
//#define CONSOLE_CLIENT

int CreateServer(char *ssid, char* password, int port)
{

}


size_t I_GetPacket(packet_header_t* buffer, size_t buflen)
{
  printf("GetPacket\n");
  return 0;
}

void I_SendPacket(packet_header_t* packet, size_t len)
{
  printf("I_SendPacket\n");
}
void I_InitNetwork(void)
{
    #ifdef CONSOLE_SERV
    //printf("init network\n");
    #endif
    #ifdef CONSOLE_CLIENT

    #endif
}

void I_WaitForPacket(int ms)
{
  //printf("WaitForPacket for : %d ms\n", ms);
}