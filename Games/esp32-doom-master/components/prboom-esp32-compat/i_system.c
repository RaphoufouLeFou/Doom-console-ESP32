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
 *  Misc system stuff needed by Doom, implemented for Linux.
 *  Mainly timer handling, and ENDOOM/ENDBOOM.
 *
 *-----------------------------------------------------------------------------
 */

#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#ifdef _MSC_VER
#define    F_OK    0    /* Check for file existence */
#define    W_OK    2    /* Check for write permission */
#define    R_OK    4    /* Check for read permission */
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>



#include "config.h"
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "m_argv.h"
#include "lprintf.h"
#include "doomtype.h"
#include "doomdef.h"
#include "lprintf.h"
#include "m_fixed.h"
#include "r_fps.h"
#include "i_system.h"
#include "i_joy.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>


// #include "dma.h"
#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"

#include <sys/time.h>

#define MODE_SPI 1
//#define PIN_NUM_MISO 2 //4
//#define PIN_NUM_MOSI 13
//#define PIN_NUM_CLK  14
//#define PIN_NUM_CS   15

//#define PIN_NUM_MISO 23 //4
//#define PIN_NUM_MOSI 19
//#define PIN_NUM_CLK  18
//#define PIN_NUM_CS   2


#define PIN_NUM_MISO 14
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  32
#define PIN_NUM_CS   27


/*
SDMMC pin configuration
#define MODE_SPI 0
MOSI = 15
MISO = 2
CLK = 14
*/


int realtime=0;
//SemaphoreHandle_t dmaChannel2Sem;

void I_uSleep(unsigned long usecs)
{
	//vTaskDelay(usecs/1000);
}

static unsigned long getMsTicks() {
  //struct timeval tv;
  //struct timezone tz;
  //unsigned long thistimereply;

  //gettimeofday(&tv, &tz);
  //unsigned long now = esp_timer_get_time() / 1000;
  //convert to ms
  //unsigned long now = tv.tv_usec/1000+tv.tv_sec*1000;
  return now;
}

int I_GetTime_RealTime (void)
{
  unsigned long thistimereply;
  //thistimereply = ((esp_timer_get_time() * TICRATE) / 1000000);
  return thistimereply;

}

const int displaytime=0;

fixed_t I_GetTimeFrac (void)
{
  unsigned long now;
  fixed_t frac;


  now = getMsTicks();

  if (tic_vars.step == 0)
    return FRACUNIT;
  else
  {
    frac = (fixed_t)((now - tic_vars.start + displaytime) * FRACUNIT / tic_vars.step);
    if (frac < 0)
      frac = 0;
    if (frac > FRACUNIT)
      frac = FRACUNIT;
    return frac;
  }
}


void I_GetTime_SaveMS(void)
{
  if (!movement_smooth)
    return;

  tic_vars.start = getMsTicks();
  tic_vars.next = (unsigned int) ((tic_vars.start * tic_vars.msec + 1.0f) / tic_vars.msec);
  tic_vars.step = tic_vars.next - tic_vars.start;
}

unsigned long I_GetRandomTimeSeed(void)
{
	return 4; //per https://xkcd.com/221/
}

const char* I_GetVersionString(char* buf, size_t sz)
{
  sprintf(buf,"%s v%s (http://prboom.sourceforge.net/)",PACKAGE,VERSION);
  return buf;
}

const char* I_SigString(char* buf, size_t sz, int signum)
{
  return buf;
}

extern unsigned char *doom1waddata;
//static bool init_SD = false;

static const char *TAG = "sdcard";

#define MOUNT_POINT "/sdcard"

#define SPI_DMA_CHAN    2

/*
void Init_SD()
{


	    esp_err_t ret;
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 4 * 1024
    };
    sdmmc_card_t* card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

       
    //printf("Init_SD() MODE_SPI = 1\n");
    
    //printf("PIN_NUM_MOSI %d , PIN_NUM_MISI = %d\n", PIN_NUM_MOSI, PIN_NUM_MISO);
    //printf("PIN_NUM_CLK %d, PIN_NUM_CLK %d\n", PIN_NUM_CS, PIN_NUM_CS);


    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    //slot_config.gpio_cs = PIN_NUM_CS;
    //slot_config.host_id = host.slot;

    printf("Use SPI mode\n");

    gpio_set_pull_mode(PIN_NUM_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_MISO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS, GPIO_PULLUP_ONLY);

    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);



    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            lprintf(LO_INFO, "Init_SD: Failed to mount filesystem.\n");
        } else {
           lprintf(LO_INFO, "Init_SD: Failed to initialize the card. %d\n", ret);
        }
        return;
    }
	lprintf(LO_INFO, "Init_SD: SD card opened.\n");
	//sdmmc_card_print_info(stdout, card);
	init_SD = true;
}*/

typedef struct {
	//const esp_partition_t* part;
	//const esp_partition_t* part;
	FILE* file;
	int offset;
	int size;
	char name[12];
} FileDesc;

static FileDesc fds[32];

int I_Open(const char *wad, int flags) {
	int x=3;
	//while (fds[x].part!=NULL) x++;
	if (strcmp(wad, "doom1-cut.wad")==0) {
		//fds[x].part=esp_partition_find_first(66, 6, NULL);
		fds[x].offset=0;
		//fds[x].size=fds[x].part->size;
	} else {
		lprintf(LO_INFO, "I_Open: open %s failed\n", wad);
		return -1;
	}
	return x;
}


int I_Lseek(int ifd, off_t offset, int whence) {
	printf("I_Lseek: ifd %d, offset %d, whence %d----------------------------------584848448zq4fqfsfsq\n", ifd, (int)offset, whence);
	if (whence==SEEK_SET) {
		fds[ifd].offset=offset;
	} else if (whence==SEEK_CUR) {
		fds[ifd].offset+=offset;
	} else if (whence==SEEK_END) {
		lprintf(LO_INFO, "I_Lseek: SEEK_END unimplemented\n");
	}
	return fds[ifd].offset;
}

int I_Filelength(int ifd)
{
	return fds[ifd].size;
}

void I_Close(int fd) {
	lprintf(LO_INFO, "I_Open: Closing File: %s\n", fds[fd].name);
	sprintf(fds[fd].name, " ");
	fclose(fds[fd].file);
	fds[fd].file=NULL;
	//esp_vfs_fat_sdmmc_unmount();
}


typedef struct {
	//spi_flash_mmap_handle_t handle;
	int ifd;
	void *addr;
	int offset;
	size_t len;
	int used;
} MmapHandle;

#define NO_MMAP_HANDLES 128
static MmapHandle mmapHandle[NO_MMAP_HANDLES];

static int nextHandle=0;

static int getFreeHandle() {
	//lprintf(LO_INFO, "getFreeHandle: Get free handle... \n");
	int n=NO_MMAP_HANDLES;
	
	while (mmapHandle[nextHandle].used!=0 && n!=0) {
		nextHandle++;
		if (nextHandle==NO_MMAP_HANDLES) nextHandle=0;
		n--;
	}
	
	if (n==0) {
		lprintf(LO_ERROR, "getFreeHandle: More mmaps than NO_MMAP_HANDLES!\n");
		
		exit(0);
	}
	
	if (mmapHandle[nextHandle].addr) {
		//spi_flash_munmap(mmapHandle[nextHandle].handle);
		
		//free(mmapHandle[nextHandle].addr);
		
		mmapHandle[nextHandle].addr=NULL;
		
		//printf("mmap: freeing handle %d\n", nextHandle);
	}
	
	int r=nextHandle;
	nextHandle++;
	if (nextHandle==NO_MMAP_HANDLES) nextHandle=0;
//	lprintf(LO_INFO, "Got: %d\n", r);
	return r;
}

void freeUnusedMmaps(void) {
	//lprintf(LO_INFO, "freeUnusedMmaps...\n");
	for (int i=0; i<NO_MMAP_HANDLES; i++) {
		//Check if handle is not in use but is mapped.
		if (mmapHandle[i].used==0 && mmapHandle[i].addr!=NULL) {
			//spi_flash_munmap(mmapHandle[i].handle);
			free(mmapHandle[i].addr);
			mmapHandle[i].addr=NULL;
			mmapHandle[i].ifd=NULL;
			//printf("Freeing handle %d\n", i);
		}
	}
}

void *I_Mmap(void *addr, size_t length, int prot, int flags, int ifd, off_t offset) {
	lprintf(LO_INFO, "I_Mmap: ifd %d, length: %d, offset: %d\n", ifd, (int)length, (int)offset);
	int i;
	//esp_err_t err;
	void *retaddr=NULL;

	for (i=0; i<NO_MMAP_HANDLES; i++) {
		if (mmapHandle[i].offset==offset && mmapHandle[i].len==length) {
			mmapHandle[i].used++;
			return mmapHandle[i].addr;
		}
	}
	i=getFreeHandle();
	//printf("test-------------------------------------------------------------------------------******************\n");

	//lprintf(LO_INFO, "I_Mmap: mmaping offset 0x%x size 0x%x handle %d Part 0x%lx\n", (int)offset, (int)length, i, fds[ifd].part->size);
	//err=esp_partition_mmap(fds[ifd].part, offset, length, SPI_FLASH_MMAP_DATA, (const void**)&retaddr, &mmapHandle[i].handle);
	//printf("test2-------------------------------------------------------------------------------******************\n");
	//if (err==ESP_ERR_NO_MEM) {
		lprintf(LO_ERROR, "I_Mmap: No free address space. Cleaning up unused cached mmaps...\n");
		freeUnusedMmaps();
	//	err=esp_partition_mmap(fds[ifd].part, offset, length, SPI_FLASH_MMAP_DATA, (const void**)&retaddr, &mmapHandle[i].handle);
	//}
	//printf("test3-------------------------------------------------------------------------------******************\n");

	mmapHandle[i].addr=retaddr;
	mmapHandle[i].len=length;
	mmapHandle[i].used=1;
	mmapHandle[i].offset=offset;
	//printf("test4-------------------------------------------------------------------------------******************\n");

	//if (err!=ESP_OK) {
		printf("Killing myself : --******************\n");
		//lprintf(LO_ERROR, "I_Mmap: Can't mmap: '%s' (len=%d)!\n", esp_err_to_name(err), length);
		return NULL;
	//}
	//printf("test5-------------------------------------------------------------------------------******************\n");

	return retaddr;
}
/*void *I_Mmap(void *addr, size_t length, int prot, int flags, int ifd, off_t offset) {
//	lprintf(LO_INFO, "I_Mmap: ifd %d, length: %d, offset: %d\n", ifd, (int)length, (int)offset);
	
	int i;
	esp_err_t err;
	void *retaddr=NULL;

	for (i=0; i<NO_MMAP_HANDLES; i++) {
		if (mmapHandle[i].offset==offset && mmapHandle[i].len==length && mmapHandle[i].ifd==ifd) {
			mmapHandle[i].used++;
			return mmapHandle[i].addr;
		}
	}

	i=getFreeHandle();

	retaddr = malloc(length);
	if(!retaddr)
	{
		lprintf(LO_ERROR, "I_Mmap: No free address space. Cleaning up unused cached mmaps...\n");
		freeUnusedMmaps();
		retaddr = malloc(length);
	}

	if(retaddr)
	{
		I_Lseek(ifd, offset, SEEK_SET);
		I_Read(ifd, retaddr, length);
		mmapHandle[i].addr=retaddr;
		mmapHandle[i].len=length;
		mmapHandle[i].used=1;
		mmapHandle[i].offset=offset;
		mmapHandle[i].ifd=ifd;
	} else {
		lprintf(LO_ERROR, "I_Mmap: Can't mmap offset: %d (len=%d)!\n", (int)offset, length);
		return NULL;
	}

	return retaddr;
}
*/

int I_Munmap(void *addr, size_t length) {
	int i;
	for (i=0; i<NO_MMAP_HANDLES; i++) {
		if (mmapHandle[i].addr==addr && mmapHandle[i].len==length/* && mmapHandle[i].ifd==ifd*/) break;
	}
	if (i==NO_MMAP_HANDLES) {
		lprintf(LO_ERROR, "I_Mmap: Freeing non-mmapped address/len combo!\n");
		exit(0);
	}
//	lprintf(LO_INFO, "I_Mmap: freeing handle %d\n", i);
	mmapHandle[i].used--;
	return 0;
}


/*
void I_Read(int ifd, void* vbuf, size_t sz)
{
	int readBytes = 0;
	lprintf(LO_INFO, "I_Read: Reading %d bytes... \n", (int)sz);
    for(int i = 0; i < 20; i++)
	{
		lprintf(LO_INFO, "I_Read: i : %d \n ", i);
		readBytes = fread(vbuf, sz, 1, fds[ifd].file);
		lprintf(LO_INFO, "I_Read: readBytes : %d \n ", readBytes);
		if( readBytes == 1)//(int)sz)
		{
			return;
		}	
		lprintf(LO_INFO, "Error Reading %d bytes\n", (int)sz);
		//vTaskDelay(300 / portTICK_RATE_MS);
	}

	I_Error("I_Read: Error Reading %d bytes after 20 tries", (int)sz);
}
*/
void I_Read(int ifd, void* vbuf, size_t sz)
{

	//printf("I_Read: %d bytes are about to be read, at offset %x, at part %lx, i = %d\n", (int)sz, fds[ifd].offset, fds[ifd].part->address, ifd);
	uint8_t *d=I_Mmap(NULL, sz, 0, 0, ifd, fds[ifd].offset);
	//printf("I_Read: %d bytes\n", *d);
	memcpy(vbuf, d, sz);
	I_Munmap(d, sz);
}

const char *I_DoomExeDir(void)
{
  return "";
}



char* I_FindFile(const char* wfname, const char* ext)
{
  char *p;
  p = malloc(strlen(wfname)+4);
  sprintf(p, "%s.%s", wfname, ext);
  return NULL;
}

void I_SetAffinityMask(void)
{
}

/*
int access(const char *path, int atype) {
    return 1;
}
*/



