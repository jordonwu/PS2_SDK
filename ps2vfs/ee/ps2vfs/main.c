/*
  _____     ___ ____
   ____|   |    ____|      PSX2 OpenSource Project
  |     ___|   |____       (C)2003, bigboss ( bigboss@ps2reality.net)
  ------------------------------------------------------------------------
  ps2vfs.c                PS2  example using ps2vfs in ee
*/
#include <compat.h> //last ps2lib changes
#include <tamtypes.h>
#include <stdio.h>
#include <kernel.h>
#include <sifrpc.h>
#include <malloc.h>
#include <fileio.h>


#include "hw.h"

#include <ps2ip.h>

t_ip_info ip_info;
void loadModules(void)
{
    int ret;
	//add your pc java server net config here
    IP4_ADDR(&ip_info.ipaddr, , , , );
	IP4_ADDR(&ip_info.gw, , , , );
	IP4_ADDR(&ip_info.netmask, , , , );

    ret = sif_load_module("host:../../../ps2debug/ps2debug.irx",0,NULL);
    if ( ret< 0 ) {
       return;
    }
    
    ret = sif_load_module("host:../../iop/bin/ps2ip.irx", 0, NULL);
    if (ret < 0) {
	return;  // Failed to load driver.
    }

    

    /*ret = sif_load_module("host:ps2klsi.irx", 0, NULL);
    if (ret < 0) {
	return; // Failed to load driver.
    }*/
    ret = sif_load_module("host:../../../ps2eth/bin/ps2smap.irx", sizeof(t_ip_info), (char *)&ip_info);
    if (ret < 0) {
	return; // Failed to load driver.
    }
	ret = sif_load_module("host:../../iop/bin/ps2ips.irx", 0, NULL);
    if (ret < 0) {
	return;  // Failed to load driver.
    }
	ret = sif_load_module("host:../../iop/bin/ps2vfs.irx", 0, NULL);
    if (ret < 0) {
	return;  // Failed to load driver.
    }



}



int main()
{
    int whichdrawbuf = 0;
    int s;
    char *buffer;
	int i;
	int size;
	int rc;
	int fd;
    // Initialise RPC system.

    sif_rpc_init(0);
    
    // Setup the Video.

    DmaReset();
    initGraph(3);
    SetVideoMode();
    //install_VRstart_handler();

    // Setup the double buffers.

   // SetDrawFrameBuffer(1);
   // SetDrawFrameBuffer(0);
   // SetCrtFrameBuffer(1);

    // Load the modules!

    loadModules();


    // Loaded the modules.. now try ps2ip now..
    if(ps2ip_init()<0)
	{
		printf("ERROR: ps2ip_init failed2");
		k_SleepThread();
	}
	//put here your file path 
    fd=fio_open("ps2vfs:\\primer\\segun\\mio.txt",O_RDONLY);
	if (fd>0)
	{
		printf("file id kernel is %d \n");
		size=fio_lseek(fd,0,SEEK_END);
		i=fio_lseek(fd,0,SEEK_SET);
		buffer=(char *)malloc(sizeof(char)*size);
		i=fio_read(fd,buffer,size);
		
		printf("receive size:  %d \n",i);
		printf("receive: buffer= %s \n",buffer);
		fio_close(fd);
			
	}
           
	
	

	
	
	while ( 1 )
	{
        //WaitForNextVRstart(1);

        //ClearVRcount();
        //SetCrtFrameBuffer(whichdrawbuf);
        //whichdrawbuf ^= 1;
        //SetDrawFrameBuffer(whichdrawbuf);
//        scr_printf( "t" );
    } 

    // We shouldn't get here.. but just in case.

    k_SleepThread();

}


