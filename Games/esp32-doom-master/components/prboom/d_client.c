/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *    Network client. Passes information to/from server, staying
 *    synchronised.
 *    Contains the main wait loop, waiting for network input or
 *    time before doing the next tic.
 *    Rewritten for LxDoom, but based around bits of the old code.
 *
 *-----------------------------------------------------------------------------
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef USE_SDL_NET
 #include "SDL.h"
#endif

#include "doomtype.h"
#include "doomstat.h"
#include "d_net.h"
#include "z_zone.h"

#include "d_main.h"
#include "g_game.h"
#include "m_menu.h"
#include "p_checksum.h"

#include "protocol.h"
#include "i_network.h"
#include "i_system.h"
#include "i_main.h"
#include "i_video.h"
#include "m_argv.h"
#include "r_fps.h"
#include "lprintf.h"

#include <string.h>
#include <sys/param.h>
#include <stdbool.h>






#include <stdio.h>










static boolean   server;
static bool isServer;
static int       remotetic; // Tic expected from the remote
static int       remotesend; // Tic expected by the remote
ticcmd_t         netcmds[MAXPLAYERS][BACKUPTICS];
static ticcmd_t* localcmds;
static unsigned          numqueuedpackets;
static packet_header_t** queuedpacket;
int maketic;
int ticdup = 1;
static int xtratics = 0;
int              wanted_player_number;

bool reicevedPacket = false;
byte reicevedPacketBuffer[10000];
int reicevedPacketLen;

typedef struct
{
  bool hasData;
  int dataLen;
  byte *data;
} QueueData;

//QueueHandle_t Client_queue;
//QueueHandle_t Server_queue;


//ToDo: What is this? - JD
int ms_to_next_tick = 1000 / TICRATE;

static boolean isExtraDDisplay = false;

static void D_QuitNetGame (void);

//QueueHandle_t *Server_Send_Queue;
size_t Server_Send_Queue_len;
//QueueHandle_t *Server_Get_Queue;
size_t Server_Get_Queue_len;

#define HAVE_NET 1

#ifndef HAVE_NET
doomcom_t*      doomcom;
#endif

#ifdef HAVE_NET

int retry_num=0;


static byte ChecksumPacket(const packet_header_t* buffer, size_t len)
{
  const byte* p = (const byte*)buffer;
  byte sum = 0;

  if (len==0)
    return 0;

  while (p++, --len)
    sum += *p;

  return sum;
}

void PrintHexData( const uint8_t *incomingData, int len){
  for(int i=0; i<len; i++){
    printf("%02x ", incomingData[i]);
  }
  printf("\n");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  //printf("Data received from: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  //printf("Data length: %d\n", len);
  if(reicevedPacket) return; // ignore if we already have a packet
  printf("Data received : ");
  PrintHexData(incomingData, len);
  reicevedPacket = true;
  reicevedPacketLen = len;
  memcpy(reicevedPacketBuffer, incomingData, len);
}

void ServerStart(void){
  //address new : 34:85:18:40:26:00
  //address old : 34:85:18:40:25:FC

  //#define NEW
/*
  nvs_flash_init();
  esp_netif_init();
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&config);
  esp_wifi_set_mode(WIFI_MODE_STA);
  //esp_wifi_set_mac(WIFI_IF_STA, (uint8_t[6]){0x34,0x85,0x18,0x40,0x26,0x00});
  esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_wifi_start();
  printf("Init result = %d\n", esp_now_init());

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
  */
}

size_t I_GetServerPacket(packet_header_t* buffer, size_t buflen)
{
  /*
  //printf("GetServerPacket\n");
  QueueData *DataBuffer;
  //printf("Server queue FETCH size = %d\n", uxQueueMessagesWaiting(Server_queue));
  if(!Server_queue)
    return 0;
  int err = xQueueReceive(Server_queue, &DataBuffer, 0);
  if(err != pdTRUE){
    //printf("no server packets\n");
    return 0;
  }
  if(!DataBuffer->hasData){
    return 0;
  }
  
  size_t len = DataBuffer->dataLen;
  memcpy(buffer, DataBuffer->data, len);
  free(DataBuffer->data);
  //free(DataBuffer);
  byte checksum = buffer->checksum;
  if (buflen<len)
    len=buflen;
  //printf("Got packet starting with %d\n", *(int *)(buffer+1));
  //printf("checkSum : %d, calculated checkSum : %d\n", checksum, ChecksumPacket(buffer, len));
  if (checksum != ChecksumPacket(buffer, len))
    return 0;

  //printf("packetLen from client (on server) = %d\n", len);
  return len;
  */
}

size_t I_GetPacketHost(packet_header_t* buffer, size_t buflen)
{
  /*
  //printf("GetHostPacket\n");
  QueueData *DataBuffer;
  if(!Client_queue)
    return 0;
  int err = xQueueReceive(Client_queue, &DataBuffer, 0);
  if(err != pdTRUE){
    //printf("no clients packets\n");
    return 0;
  }

  if(!DataBuffer->hasData){
    return 0;
  }

  size_t len = DataBuffer->dataLen;
  memcpy(buffer, DataBuffer->data, len);
  free(DataBuffer->data);
  free(DataBuffer);

  byte checksum = buffer->checksum;
  if (buflen<len)
    len=buflen;
  //printf("packetLen from server NO CHECKSUM (on host client) = %d\n", len);
  //printf("checkSum : %d, calculated checkSum : %d\n", checksum, ChecksumPacket(buffer, len));
  if (checksum != ChecksumPacket(buffer, len))
    return 0;

  //printf("packetLen from server (on host client) = %d\n", len);
  return len;
  */
}

size_t I_GetPacket(packet_header_t* buffer, size_t buflen)
{
  /*
  //printf("GetPacket\n");
  if(isServer)
    return I_GetPacketHost(buffer, buflen);
  if(!reicevedPacket){
    return 0;
  }
  
  size_t len = reicevedPacketLen;
  if (buflen<len)
    len=buflen;
  memcpy(buffer, reicevedPacketBuffer, len);
  byte checksum = buffer->checksum;
  if (checksum != ChecksumPacket(buffer, len))
    return 0;

  reicevedPacket = false;
  //printf("packetLen from server (on client) = %d\n", len);
  return len;
  */
}

void I_SendPacket(packet_header_t* packet, size_t len)
{
  //printf("I_SendPacket\n");

size_t I_GetPacket(packet_header_t* buffer, size_t buflen){
   return I_GetPacketHost(buffer, buflen);
  //else return I_GetPacket_Net(buffer, buflen);
}

  uint8_t macAdress[6] = {0x34,0x85,0x18,0x40,0x26,0x00}; 
  uint8_t mac[6] = {0};
  //esp_efuse_mac_get_default(mac);
  if(memcmp(macAdress, mac, 6) == 0)
  {
    const uint8_t macAdress2[6] = {0x34,0x85,0x18,0x40,0x25,0xFC};
    memcpy(macAdress, macAdress2, 6);
  }
  //if(!isServer)
    //esp_now_send(macAdress, (uint8_t*)packet, len);

  {
    //printf("sending packet to server of size %d\n", len);
    QueueData *QD = malloc(sizeof(QD));
    QD->hasData = true;
    QD->dataLen = len;
    QD->data = malloc(len);
    memcpy(QD->data, packet, len);
    //if(xQueueGenericSend(Server_queue, &QD, 0, queueSEND_TO_BACK) != pdTRUE)
    {
      free(QD->data);
      free(QD);
    }
  }

}

void I_SendPacketTo(packet_header_t* packet, size_t len, uint8_t *to)
{

  byte checksum = ChecksumPacket(packet, len);
  packet->checksum = checksum;

  uint8_t mac[6] = {0};
  //esp_efuse_mac_get_default(mac);
  if(memcmp(to, mac, 6) != 0)
  //if(!isServer)
    //esp_now_send(to, (uint8_t*)packet, len);

  {
    //printf("sending packet to client of size %d\n", len);
    QueueData *QD = malloc(sizeof(QD));
    QD->hasData = true;
    QD->dataLen = len;
    QD->data = malloc(len);
    memcpy(QD->data, packet, len);
    //if(xQueueGenericSend(Client_queue, &QD, 0, queueSEND_TO_BACK) != pdTRUE)
    {
      free(QD->data);
      free(QD);
    }
  }
}

void I_InitNetwork()
{
  printf("init network\n");
  ServerStart();
}

void I_WaitForPacket(int ms)
{
 // if(!Client_queue)
  {
    //vTaskDelay(pdMS_TO_TICK(ms));
    return;
  }
  packet_header_t *packet;
  //printf("WaitForPacket for : %d ms\n", ms);
  //xQueuePeek(Client_queue, &packet, pdMS_TO_TICK(ms));
  
  //vTaskDelay(pdMS_TO_TICK(10));

}

void I_WaitForPacketServer(int ms)
{
  
  //if(!Server_queue)
  {
    //vTaskDelay(pdMS_TO_TICK(ms));
    return;
  }
  packet_header_t *packet;
  //printf("WaitForPacket SERVER for : %d ms\n", ms);
  //xQueuePeek(Server_queue, &packet, pdMS_TO_TICK(ms));
  
  //vTaskDelay(pdMS_TO_TICK(10));
}

void PrintMac(uint8_t *mac)
{
  printf("mac adress : ");
  for(int i = 0; i < 6; i++)
  {
    printf("0x%x", mac[i]);
    if(i != 5)
      printf(", ");
  }
  printf("\n");
}

void D_InitNetGame (void)
{
  int i;
  int numplayers = 1;
  i = M_CheckParm("-net");
  isServer = !!M_CheckParm("-server");
  //Client_queue = xQueueCreate(5,sizeof(QueueData *));
  //if(isServer)
    //Server_queue = xQueueCreate(5,sizeof(QueueData *));

  printf("i = %d\n", i);
  if (i && i < myargc-1) i++;
  
  if (!(netgame = server = !!i)) {
    playeringame[consoleplayer = 0] = true;
    
    // e6y
    // for play, recording or playback using "single-player coop" mode.
    // Equivalent to using prboom_server with -N 1
    netgame = M_CheckParm("-solo-net");

    printf("YES %d\n", netgame);
  } else {

    printf("NO\n");
    // Get game info from server
    packet_header_t *packet = Z_Malloc(1000, PU_STATIC, NULL);
    struct setup_packet_s *sinfo = (void*)(packet+1);
  struct { packet_header_t head; short pn; uint8_t Addr[6];} PACKEDATTR initpacket;

    I_InitNetwork();
    //dp_socket = I_Socket(0);
    //_ConnectToServer(myargv[i]);

    do
    {
      do { 
        // Send init packet
        initpacket.pn = 0;
        uint8_t mac[6] = {0};
        esp_efuse_mac_get_default(mac);
        memcpy(initpacket.Addr, mac, 6);
        packet_set(&initpacket.head, PKT_INIT, 0);
        I_SendPacket(&initpacket.head, sizeof(initpacket));
        I_WaitForPacket(1000);
      } while (!I_GetPacket(packet, 1000));
      if (packet->type == PKT_DOWN) I_Error("Server aborted the game");
    } while (packet->type != PKT_SETUP);
    printf("Setup game\n");

    // Once we have been accepted by the server, we should tell it when we leave
    atexit(D_QuitNetGame);
    sinfo->numwads = 0;
    sinfo->players = 1;
    sinfo->ticdup = 1;
    // Get info from the setup packet
    consoleplayer = sinfo->yourplayer;
    compatibility_level = sinfo->complevel;
    G_Compatibility();
    startskill = sinfo->skill;
    deathmatch = sinfo->deathmatch;
    startmap = sinfo->level;
    startepisode = sinfo->episode;
    ticdup = sinfo->ticdup;
    xtratics = sinfo->extratic;
    G_ReadOptions(sinfo->game_options);


    lprintf(LO_INFO, "\tjoined game as player %d/%d; %d WADs specified\n",
      consoleplayer+1, numplayers = sinfo->players, sinfo->numwads);
    {
      char *p = sinfo->wadnames;
      int i = sinfo->numwads;

      while (i--) {
        printf("wad name %s\n", p);
  D_AddFile(p, source_net);
  p += strlen(p) + 1;
      }
    }
    Z_Free(packet);
  }
  localcmds = netcmds[displayplayer = consoleplayer];
  for (i=0; i<numplayers; i++)
    playeringame[i] = true;
  for (; i<MAXPLAYERS; i++)
    playeringame[i] = false;
  if (!playeringame[consoleplayer]) I_Error("D_InitNetGame: consoleplayer not in game");
}
#else
void D_InitNetGame (void)
{
  int i;

  doomcom = Z_Malloc(sizeof *doomcom, PU_STATIC, NULL);
  doomcom->consoleplayer = 0;
  doomcom->numnodes = 0; doomcom->numplayers = 1;
  localcmds = netcmds[consoleplayer];
  netgame = (M_CheckParm("-solo-net") != 0);

  for (i=0; i<doomcom->numplayers; i++)
    playeringame[i] = true;
  for (; i<MAXPLAYERS; i++)
    playeringame[i] = false;

  consoleplayer = displayplayer = doomcom->consoleplayer;
}
#endif // HAVE_NET

#ifdef HAVE_NET
void D_CheckNetGame(void)
{
  if (server) {
    packet_header_t *packet = Z_Malloc(sizeof(packet_header_t)+1, PU_STATIC, NULL);

    lprintf(LO_INFO, "D_CheckNetGame: waiting for server to signal game start\n");
    packet_set(&packet, PKT_SETUP, 0);
    I_SendPacket(&packet, sizeof(packet_header_t)+sizeof(struct setup_packet_s) + 1);
    do {
      while (!I_GetPacket(packet, sizeof(packet_header_t)+1)) {
        packet_set(packet, PKT_GO, 0);
        *(byte*)(packet+1) = consoleplayer;
        I_SendPacket(packet, sizeof(packet_header_t)+1);
        I_WaitForPacket(1000);

      }
    } while (packet->type != PKT_GO);
    Z_Free(packet);
  }
}

boolean D_NetGetWad(const char* name)
{
#if defined(HAVE_WAIT_H)
  size_t psize = sizeof(packet_header_t) + strlen(name) + 500;
  packet_header_t *packet;
  boolean done = false;

  if (!server || strchr(name, '/')) return false; // If it contains path info, reject

  do {
    // Send WAD request to remote
    packet = Z_Malloc(psize, PU_STATIC, NULL);
    packet_set(packet, PKT_WAD, 0);
    *(byte*)(packet+1) = consoleplayer;
    strcpy(1+(byte*)(packet+1), name);
    I_SendPacket(packet, sizeof(packet_header_t) + strlen(name) + 2);

    I_uSleep(10000);
  } while (!I_GetPacket(packet, psize) || (packet->type != PKT_WAD));
  Z_Free(packet);

  if (!strcasecmp((void*)(packet+1), name)) {
    pid_t pid;
    int   rv;
    byte *p = (byte*)(packet+1) + strlen(name) + 1;

    /* Automatic wad file retrieval using wget (supports http and ftp, using URLs)
     * Unix systems have all these commands handy, this kind of thing is easy
     * Any windo$e port will have some awkward work replacing these.
     */
    /* cph - caution here. This is data from an untrusted source.
     * Don't pass it via a shell. */
    if ((pid = fork()) == -1)
      perror("fork");
    else if (!pid) {
      /* Child chains to wget, does the download */
      execlp("wget", "wget", p, NULL);
    }
    /* This is the parent, i.e. main LxDoom process */
    wait(&rv);
    if (!(done = !access(name, R_OK))) {
      if (!strcmp(p+strlen(p)-4, ".zip")) {
  p = strrchr(p, '/')+1;
  if ((pid = fork()) == -1)
    perror("fork");
  else if (!pid) {
    /* Child executes decompressor */
    execlp("unzip", "unzip", p, name, NULL);
  }
  /* Parent waits for the file */
  wait(&rv);
  done = !!access(name, R_OK);
      }
      /* Add more decompression protocols here as desired */
    }
    Z_Free(buffer);
  }
  return done;
#else /* HAVE_WAIT_H */
  return false;
#endif
}

void NetUpdate(void)
{
  static int lastmadetic;
  if (isExtraDDisplay)
    return;
  //if (server) { // Receive network packets
  if (1) { // Receive network packets
    size_t recvlen;
    packet_header_t *packet = Z_Malloc(10000, PU_STATIC, NULL);
    while ((recvlen = I_GetPacket(packet, 10000))) {
      printf("recv len: %d\nData: ", recvlen);
      PrintHexData(packet, recvlen);
      printf("packet type: %d\n", packet->type);
      switch(packet->type) {
      //switch(PKT_TICS) {
      case PKT_TICS:
  {
    printf("PKT_TICS tic = %d\n", packet->tic);
    byte *p = (void*)(packet+1);
    int tics = *p++;
    unsigned long ptic = doom_ntohl(packet->tic);
    if (ptic > (unsigned)remotetic) { // Missed some
      printf("Missed some, ptic: %ld, remotetic: %d\n", ptic, remotetic);
      packet_set(packet, PKT_RETRANS, remotetic);
      *(byte*)(packet+1) = consoleplayer;
      I_SendPacket(packet, sizeof(*packet)+1);
    } else {
      if (ptic + tics <= (unsigned)remotetic) break; // Will not improve things
      
      remotetic = ptic;
      while (tics--) {
        int players = *p++;
        while (players--) {
            int n = *p++;
            RawToTic(&netcmds[n][remotetic%BACKUPTICS], p);
            p += sizeof(ticcmd_t);
        }
        remotetic++;
      }
    }
  }
  break;
      case PKT_RETRANS: // Resend request
          printf("PKT_RETRANS\n");
          remotesend = doom_ntohl(packet->tic);
          break;
      case PKT_DOWN: // Server downed
  {
    printf("PKT_DOWN\n");
    int j;
    for (j=0; j<MAXPLAYERS; j++)
      if (j != consoleplayer) playeringame[j] = false;
    server = false;
    doom_printf("Server is down\nAll other players are no longer in the game\n");
  }
  break;
      case PKT_EXTRA: // Misc stuff
      case PKT_QUIT: // Player quit
      printf("PKT_QUIT\n");
  // Queue packet to be processed when its tic time is reached
  queuedpacket = Z_Realloc(queuedpacket, ++numqueuedpackets * sizeof *queuedpacket,
         PU_STATIC, NULL);
  queuedpacket[numqueuedpackets-1] = Z_Malloc(recvlen, PU_STATIC, NULL);
  memcpy(queuedpacket[numqueuedpackets-1], packet, recvlen);
  break;
      case PKT_BACKOFF:
        printf("PKT_BACKOFF\n");
        /* cph 2003-09-18 -
   * The server sends this when we have got ahead of the other clients. We should
   * stall the input side on this client, to allow other clients to catch up.
   */
        lastmadetic++;
  break;
      default: // Other packet, unrecognised or redundant
      printf("default\n");
  break;
      }
    }
    Z_Free(packet);
  }
  { // Build new ticcmds
    
    int newtics = I_GetTime() - lastmadetic;
    newtics = (newtics > 0 ? newtics : 0);
    lastmadetic += newtics;
    if (ffmap) newtics++;
    while (newtics--) {
      I_StartTic();
      if (maketic - gametic > BACKUPTICS/2) break;
      G_BuildTiccmd(&localcmds[maketic%BACKUPTICS]);
      maketic++;
    }
    if (/*server && */maketic > remotesend) { // Send the tics to the server
      int sendtics;
      remotesend -= xtratics;
      if (remotesend < 0) remotesend = 0;
      sendtics = maketic - remotesend;
      {
        printf("sendtics = %d\n", sendtics);
        size_t pkt_size = sizeof(packet_header_t) + 2 + sendtics * sizeof(ticcmd_t);
        packet_header_t *packet = Z_Malloc(pkt_size, PU_STATIC, NULL);

        packet_set(packet, PKT_TICS, maketic - sendtics);
        *(byte*)(packet+1) = sendtics;
        *(((byte*)(packet+1))+1) = consoleplayer;
        {
          void *tic = ((byte*)(packet+1)) +2;
          while (sendtics--) {
            TicToRaw(tic, &localcmds[remotesend++%BACKUPTICS]);
            tic = (byte *)tic + sizeof(ticcmd_t);
          }
        }
        I_SendPacket(packet, pkt_size);
        Z_Free(packet);
      }
    }
  }

}
#else

void D_BuildNewTiccmds(void)
{
    static int lastmadetic;
    int newtics = I_GetTime() - lastmadetic;
    lastmadetic += newtics;
    while (newtics--)
    {
      I_StartTic();
      if (maketic - gametic > BACKUPTICS/2) break;
      G_BuildTiccmd(&localcmds[maketic%BACKUPTICS]);
      maketic++;
    }
}
#endif

#ifdef HAVE_NET
/* cph - data passed to this must be in the Doom (little-) endian */
void D_NetSendMisc(netmisctype_t type, size_t len, void* data)
{
  if (netgame) {
    size_t size = sizeof(packet_header_t) + 3*sizeof(int) + len;
    packet_header_t *packet = Z_Malloc(size, PU_STATIC, NULL);
    int *p = (void*)(packet+1);

    packet_set(packet, PKT_EXTRA, gametic);
    *p++ = LONG(type); *p++ = LONG(consoleplayer); *p++ = LONG(len);
    memcpy(p, data, len);
    I_SendPacket(packet, size);

    Z_Free(packet);
  }
}

static void CheckQueuedPackets(void)
{
  int i;
  for (i=0; (unsigned)i<numqueuedpackets; i++)
    if (doom_ntohl(queuedpacket[i]->tic) <= gametic)
      switch (queuedpacket[i]->type) {
      case PKT_QUIT: // Player quit the game
  {
    int pn = *(byte*)(queuedpacket[i]+1);
    playeringame[pn] = false;
    doom_printf("Player %d left the game\n", pn);
  }
  break;
      case PKT_EXTRA:
  {
    int *p = (int*)(queuedpacket[i]+1);
    size_t len = LONG(*(p+2));
    switch (LONG(*p)) {
    case nm_plcolour:
      G_ChangedPlayerColour(LONG(*(p+1)), LONG(*(p+3)));
      break;
    case nm_savegamename:
      if (len < SAVEDESCLEN) {
        memcpy(savedescription, p+3, len);
        // Force terminating 0 in case
        savedescription[len] = 0;
      }
      break;
    }
  }
  break;
      default: // Should not be queued
  break;
      }

  { // Requeue remaining packets
    int newnum = 0;
    packet_header_t **newqueue = NULL;

    for (i=0; (unsigned)i<numqueuedpackets; i++)
      if (doom_ntohl(queuedpacket[i]->tic) > gametic) {
  newqueue = Z_Realloc(newqueue, ++newnum * sizeof *newqueue,
           PU_STATIC, NULL);
  newqueue[newnum-1] = queuedpacket[i];
      } else Z_Free(queuedpacket[i]);

    Z_Free(queuedpacket);
    numqueuedpackets = newnum; queuedpacket = newqueue;
  }
}
#endif // HAVE_NET

void TryRunTics (void)
{
  int runtics;
  int entertime = I_GetTime();

  // Wait for tics to run
  while (1) {
#ifdef HAVE_NET
    //printf("loop from d_client : TryRunTics() : 655\n");
    NetUpdate();
#else
    D_BuildNewTiccmds();
#endif
    runtics = (server ? remotetic : maketic) - gametic;
    if (!runtics) {
      if (!movement_smooth) {
#ifdef HAVE_NET
        if (server)
          I_WaitForPacket(ms_to_next_tick);
#endif
      }
      if (I_GetTime() - entertime > 10) {
#ifdef HAVE_NET
        if (server) {
          char buf[sizeof(packet_header_t)+1];
          remotesend--;
          packet_set((packet_header_t *)buf, PKT_RETRANS, remotetic);
          buf[sizeof(buf)-1] = consoleplayer;
          I_SendPacket((packet_header_t *)buf, sizeof buf);
        }
#endif
        M_Ticker(); return;
      }
      //if ((displaytime) < (tic_vars.next-SDL_GetTicks()))
      {
        WasRenderedInTryRunTics = true;
        if (V_GetMode() == VID_MODEGL ? 
            movement_smooth : 
            movement_smooth && gamestate==wipegamestate)
        {
          isExtraDDisplay = true;
          //printf("D_Display from here");
          D_Display();
          isExtraDDisplay = false;
        }
      }
    } else break;
  }
  printf("exited this\n");
  while (runtics--) {
#ifdef HAVE_NET
    if (server) CheckQueuedPackets();
#endif
    if (advancedemo)
      D_DoAdvanceDemo ();
    M_Ticker ();
    I_GetTime_SaveMS();
    G_Ticker ();
    P_Checksum(gametic);
    gametic++;
#ifdef HAVE_NET
    //printf("loop from d_client : TryRunTics() : 709\n");
    NetUpdate(); // Keep sending our tics to avoid stalling remote nodes
#endif
  }
  printf("returning from TryRunTics\n");
}

#ifdef HAVE_NET
static void D_QuitNetGame (void)
{
  byte buf[1 + sizeof(packet_header_t)];
  packet_header_t *packet = (void*)buf;
  int i;

  if (!server) return;
  buf[sizeof(packet_header_t)] = consoleplayer;
  packet_set(packet, PKT_QUIT, gametic);

  for (i=0; i<4; i++) {
    I_SendPacket(packet, 1 + sizeof(packet_header_t));
    I_uSleep(10000);
  }
}
#endif
