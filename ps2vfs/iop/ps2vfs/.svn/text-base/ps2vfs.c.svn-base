/*
  _____     ___ ____
   ____|   |    ____|      PSX2 OpenSource Project
  |     ___|   |____       (C)2003, Bigboss ( bigboss@ps2reality.net )
                           
  ------------------------------------------------------------------------
  ps2vfs.c                PS2VFS CLIENT FILE SYSTEM DRIVER.


  The PS2VFS file io driver is a driver that slots into the PS2
  IO subsystem and provides access to files in pc side.

  Based in ps2http.c from oobles and sjeep
*/
#include <tamtypes.h>
#include <fileio.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>
#include <sysclib.h>
#include <ps2debug.h>

#include "ps2ip.h"

// How many handles do we keep open.
#define HCOUNT 32

//i will make a separate include now i am very tired 31th March 01:07
typedef struct
{
	u32 code;
    u8 cmd[5];
}ps2Vfs_command;
#define OPEN "opena"
#define CLOSE "close"
#define READ "reada"
#define HELLO "hello"
#define EXIT  "exita"
#define SEEK  "seeka"
//i modified sjeep structure add javaFd and sockFd
typedef struct
{
	int fd;
	int sockId;
	int javaFd;
	int fileSize;
	int filePos;
	int used;

} handle;

// These are the list of socket file handles.
static handle handles[ HCOUNT ];

// FileIO structure.
static struct fileio_driver file_driver;

// Function array for fileio structure.
static void *filedriver_functarray[16];

int ps2VfsSendCommand(char *command,int code,int socket)
{
	int rc;
	ps2Vfs_command request;
	strncpy(request.cmd,command,5);
    request.code=htonl((u32)code);
	rc = send(socket, &request,  9, 0 );
	if(rc<0)
	{
		return -1;
	}
	printf("Sent command: %s code=%d ...\n",request.cmd,htonl(request.code));
	
	return rc;
}
int ps2VfsReceiveCommand(int socket)
{
	int rc;
	ps2Vfs_command response;
    rc = recv( socket, &response,  9, 0 );
	if(rc<0)
	{
		return -1;
	}
	rc=htonl(response.code);
    printf("Command receive: %s code= %d \n",response.cmd,rc);
	return rc;
			
}
int ps2VfsDisconnect(int socket)
{
	int rc;
	rc=ps2VfsSendCommand(EXIT,0,socket);
	if(rc<0)
	{  
		return -1;
	}
    rc=ps2VfsReceiveCommand(socket);
	disconnect(socket);
	return rc;
}

int ps2VfsConnect()
{
	
	int s;
	int rc;
    struct sockaddr_in ps2VfsClient;

	s = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if(s<0)
	{ 
		printf("Error getting socket...\n");
		return -1;
	}
	memset( &ps2VfsClient, 0 , sizeof(ps2VfsClient));
	ps2VfsClient.sin_family = AF_INET;
	//please put here your pc java server ip
	ps2VfsClient.sin_addr.s_addr = htonl(((u32)(172 & 0xff) << 24) | ((u32)(26& 0xff) << 16 ) | ((u32)(0 & 0xff) << 8) | (u32)(3 & 0xff));
	ps2VfsClient.sin_port = htons(6969);
	rc = connect( s, (struct sockaddr *) &ps2VfsClient, sizeof(ps2VfsClient));
	if ( rc < 0 )
	{
		printf("Error connecting...\n");
		return -1;
	}
	
    rc=ps2VfsSendCommand(HELLO,0,s);
    if (rc!=9)
    {
		return -1;
    }
	rc=ps2VfsReceiveCommand(s);
	if(rc<0)
	{
		return -1;
	}
	
	printf("Ps2VfsServer is listening\n");
	return s;
}
//
// Any calls we don't implement calls dummy.
// 
int dummy()
{
	printf("PS2HTTP: dummy function called\n");
	return -5;
}

//
// Initialise clears our list of socket handles that
// keeps track of open connections.
//
void ps2Vfs_initialize( struct fileio_driver *driver)
{

	printf("PS2VFS: initializing '%s' file driver.\n", driver->device);
  
	// Clear the file handles. 
	memset(&handles, 0, sizeof(handles));
	

}


//
// Open has the most work to do in the file driver.  It must:
//
//  1. Find a free file Handle.
//  2. Try and connect to the remote server.
//  3. Send a command opena request to the server
//  5. Receive command OPENA
int ps2Vfs_open( int fd, char *name, int mode)
{
	
	int rc;
	int size;
	int handle;


	// First find an open Handle.
	for(handle = 0; handle < HCOUNT; handle++)
		if(handles[handle].used == 0) break;

	// No free handles. exit.
	if(handle >= HCOUNT) return -1;

	// Reserve this for our use.
	// We don't have mutex semaphores protecting.. so quicker we
	// stop another thread stealing our it the better.
	handles[handle].used = 1;

	// Store kernel file handle
	handles[handle].fd = fd;

	handles[handle].fileSize = 0;
	handles[handle].filePos = 0;
	//connect to java server
    handles[handle].sockId=ps2VfsConnect();
	if(handles[handle].sockId<0)
	{
		handles[handle].sockId=0;
		return -1;
	}
	size=strlen(name);
	//send command
    rc=ps2VfsSendCommand(OPEN,size,handles[handle].sockId);
    if (rc!=9)
    {
		return -1;
    }
	//send filename string
    rc=send(handles[handle].sockId,name,size,0);
	if(rc!=size)
	{
		return -1;
	}
	//here to send mode default r 
   // rc=send(handles[handle].sockId, , ,0);
//	if(rc!=)
//	{
//		return -1;
//	}

    //recieve command
	rc=ps2VfsReceiveCommand(handles[handle].sockId);
	if(rc<0)
	{
		return -1;
	}
    handles[handle].javaFd = rc;

	return fd;
	
	
}



int ps2Vfs_read( int fd, char * buffer, int size )
{
	int bytesRead = 0;
	int handle;
	int left = size;
	u32 sizeaux=(u32)htonl((u32)size);
	int totalRead = 0;
    int rc;
	// First find correct handle
	for(handle = 0; handle < HCOUNT; handle++)
		if(handles[handle].fd == fd) break;

	if(handle >= HCOUNT) return -1;

    rc=ps2VfsSendCommand(READ,handles[handle].javaFd,handles[handle].sockId);
	if (rc!=9)
    {
		return -1;
    }
    rc=send(handles[handle].sockId,&sizeaux,4,0);
	if (rc!=4)
    {
		return -1;
    }
    rc=ps2VfsReceiveCommand(handles[handle].sockId);
	if(rc<0)
	{
		return -1;
	}
	left=rc;
	// Read until: there is an error, we've read "size" bytes or the remote 
	//             side has closed the connection.
	do {

		bytesRead = recv( handles[handle].sockId, buffer + totalRead, left, 0 ); 

		printf("bytesRead = %d\n", bytesRead);

		if(bytesRead <= 0) break;

		left -= bytesRead;
		totalRead += bytesRead;

	} while(left);

	return totalRead; 
}


//
// Close finds the correct handle and
// calls disconnect.
//
int ps2Vfs_close( int fd)
{
	int handle;
    int rc;
	for(handle = 0; handle < HCOUNT; handle++)
		if(handles[handle].fd == fd) break;

	if(handle >= HCOUNT) return -1;

	rc=ps2VfsSendCommand(CLOSE,handles[handle].javaFd,handles[handle].sockId);
    if (rc!=9)
    {
		return -1;
    }
    rc=ps2VfsReceiveCommand(handles[handle].sockId);
	if(rc<0)
	{   handles[handle].used = 0;
		return -1;
	}
	
	
    rc=ps2VfsDisconnect(handles[handle].sockId);
	handles[handle].fd = -1;
	handles[handle].javaFd = -1;
	handles[handle].used = 0;
    
	
	return 0;
}

//
// lseek is full supported
int ps2Vfs_lseek(int fd, int offset, int whence)
{
	int handle;
    u32 offsetaux;
	u32 whenceaux;
	int rc;
	for(handle = 0; handle < HCOUNT; handle++)
		if(handles[handle].fd == fd) break;

	if(handle >= HCOUNT) return -1;
    rc=ps2VfsSendCommand(SEEK,handles[handle].javaFd,handles[handle].sockId);
	if (rc!=9)
    {
		return -1;
    }
    offsetaux=(u32)htonl((u32)offset);
	whenceaux=(u32)htonl((u32)whence);
    rc=send(handles[handle].sockId,&offsetaux,4,0);
	if (rc!=4)
    {
		return -1;
    }
    rc=send(handles[handle].sockId,&whenceaux,4,0);
	if (rc!=4)
    {
		return -1;
    }
    rc=ps2VfsReceiveCommand(handles[handle].sockId);
    handle[handles].filePos = rc;
	return rc;


}


//
// Main..  registers the File driver.
//
int _start( int argc, char **argv)
{
   int	i;
   
	printf("Bigboss/PS2REALITY Ps2 Virtual File System\n");

	file_driver.device = "ps2vfs";
	file_driver.xx1 = 16;
	file_driver.version = 1;
	file_driver.description = "ps2vfs client file driver";
	file_driver.function_list = filedriver_functarray;
	for (i=0;i < 16; i++)
		filedriver_functarray[i] = dummy;
	filedriver_functarray[ FIO_INITIALIZE ] = ps2Vfs_initialize;
	filedriver_functarray[ FIO_OPEN ] = ps2Vfs_open;
	filedriver_functarray[ FIO_CLOSE ] = ps2Vfs_close;
	filedriver_functarray[ FIO_READ ] = ps2Vfs_read;
	filedriver_functarray[ FIO_SEEK ] = ps2Vfs_lseek;
 
	printf("PS2VFS: Removing any drivers of same name\n"); 
	FILEIO_del( "ps2vfs");

	printf("PS2VFS: Adding 'ps2vfs' driver into io system\n");
	FILEIO_add( &file_driver);


	return 0;
}

