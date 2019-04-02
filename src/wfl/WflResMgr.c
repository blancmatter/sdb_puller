/*
** WflResMgr.c
**
** Program to test the functionality of a resource manager using
** a receive loop.
**
*/

/* System include files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

/* Ttl include files */
#include "Wfl.h"

int mXxxWrite( int pid, char *buf, int size, int offset )
{
   int Ret;

#if defined E_WFL_OS_QNX4

   Ret = Writemsg(pid, offset, buf, size);

#endif

#if defined E_WFL_OS_QNX6 || defined E_WFL_OS_LINUX

   printf("MESSAGE = %s\n",buf);
   printf("offset = %d\n",offset);
   printf("size = %d\n",size);
   printf("pid = %d\n",pid);
   Ret = MsgWrite( pid, buf, size, offset); 

#endif

   return size;

}


int mXxxIoOpen( eWflResMgrMsg_t *pMsg, int rcvid )
{
   int ReplySize;

   eWflOpenMessage(&pMsg, &ReplySize, EOK); 

   return ReplySize;
}

int mXxxIoRead( eWflResMgrMsg_t *pMsg, int rcvid )
{
   Status_t Status = SYS_NOMINAL;   
   int Size;
   int Offset;

   /* Get the size of the outgoing message */
   Status = eWflReadMessInfo( pMsg, &Size, &Offset );
   if( Status != SYS_NOMINAL )
   {
      printf("Error reading message size 0x%x\n",Status);
      exit(1);
   }


   /**************************************************/
              /* iAgc call goes here */
   Size = mXxxWrite( rcvid, "Test Message" , sizeof("Test Message"),0);

   /***************************************************/


   Status = eWflReadReply( &pMsg, Size, EOK );
   if( Status != SYS_NOMINAL )
   {
      printf("Error reply to read message 0x%x\n",Status);
      exit(1);
   }

   return 0;

}


int mXxxIoWrite( eWflResMgrMsg_t *pMsg, int rcvid )
{
   int Status;
   int Size;
   int Offset;
   char *buff;
   int ReplySize;

   /* Determine the size of the incoming message */
   Status = eWflWriteMessInfo( pMsg, &Size, &Offset );
   if( Status != SYS_NOMINAL )
   {
      printf("Error getting the incoming message size 0x%x\n",Status);
      exit(1);
   }
   printf("Incoming message size = %d\n",Size);

   /* Allocate the appropriate memory */
   buff = (char *) malloc( Size );
   if( buff == NULL )
   {
       printf("Error allocating memory\n");
   }

   Status = eWflReadMessage( rcvid, buff, Offset, Size );
   if( Status != SYS_NOMINAL )
   {
      printf("Error reading incoming message 0x%x\n",Status);
   }
            
   printf("Message = \"%s\"\n",buff);
   Status = eWflWriteReply( &pMsg, &ReplySize );
   if(Status != SYS_NOMINAL )
   {
      printf("Error generating reply to write 0x%x\n",Status);
   }



   /* Free the message buffer */
   free(buff);

   return ReplySize;
}


void server()
{
   Status_t Status;
   int devno;
   int ret;
   int rcvid;
   int type;
   eWflResMgrMsg_t *pMsg;
   eWflPulseId_t RecvId;
   eWflPulseId_t ReplId;
   int                  id;
   int             doreply;


   printf("Running server\n");

   Status = eWflDeviceAttach( &devno );
   if( Status != SYS_NOMINAL )
   {
      printf("Error attaching device number 0x%x\n",Status);
      exit(EXIT_FAILURE);
   }

   Status = eWflPrefixAttach("/dev/camera",devno);
   if( Status != SYS_NOMINAL )
   { 
      printf("Error attaching prefix to device number 0x%x\n",Status);
      exit(EXIT_FAILURE);
   }

   /* Allocate memory for message */
   Status = eWflResMgrMessAlloc( &pMsg );
   if( Status != SYS_NOMINAL )
   {
      printf("Error allocating message memory 0x%x\n",Status);
      exit(EXIT_FAILURE);
   }

   printf("Starting Receive loop\n");  

   while(1)
   {
      Status = eWflResMgrReceive(&rcvid,&type,&pMsg);
      if( Status != SYS_NOMINAL )
      {
         printf("Error in resource manager receive 0x%x\n",Status);
         exit(EXIT_FAILURE);
      }

      switch(type)
      {
         case E_WFL_IO_OPEN:
            printf("io_open request\n");
            doreply = mXxxIoOpen( pMsg, rcvid );
            break;
            
         case E_WFL_IO_CLOSE:
            printf("io_close request\n");
            break;
            
         case E_WFL_IO_DUP:
            printf("io_dup request\n");
            break;
            
            /* I/O messages */
         case E_WFL_IO_READ:
            printf("io_read request\n");
            doreply = mXxxIoRead( pMsg, rcvid );
            break;
            
         case E_WFL_IO_WRITE:
            printf("io_write request\n");
            doreply = mXxxIoWrite( pMsg, rcvid );
            break;
            
            /* Ioctl message */
         case E_WFL_IO_QIOCTL:
            printf("io_qioctl request\n");
            break;
            
         default:
            printf("printf unknown message request\n");
            break;
      }
      
      Status = eWflResMgrReply(rcvid, pMsg, doreply);
      if( Status != SYS_NOMINAL )
      {
         printf("Error replying to resource manager message\n");
         exit(1);
      }


   }/* End of while */
  

}

void client()
{
   int fd;
   int ret;
   int i,j;
   char buf[100];
   char rbuf[100];
   char *tmp;
   printf("Running client\n");

   /* Open device */
   fd = open("/dev/camera",O_RDWR);

   if( fd == -1 )
   {
      printf("Error opening device %s\n",strerror(errno));
      exit(1);
   }

   for(i=0;i<5;i++)
   {
      printf("Client message %d\n",i);

      sprintf(buf,"Message %d",i);

      printf("Writing \"%s\" to server\n",buf);

      ret = write(fd,buf,sizeof(buf));
      if( ret == -1 )
      {
         printf("Error writing string %s\n",strerror(errno));
         exit(1);
      }
      delay(1000);

      ret = read(fd,rbuf,sizeof("Test Message"));
      if( ret == -1 )
      {
         printf("Error reading string\n",strerror(errno));
         exit(1);
      }
      printf("Read reply ret %d \"%s\"\n",ret,rbuf);
      for(j=0;j<20;j++)
         printf("%c",rbuf[j]);

      printf("\n");

      delay(1000);

   }

   delay(5000);
   close(fd);

}

int main( int argc, char *argv[] )
{
   if( argc == 2 )
   {
      /* Check to see if we're server or client */  
      if( strcmp(argv[1],"-s") == 0 )
      {
         /* Start the server */
         server();

      }
      else if( strcmp(argv[1],"-c") == 0 )
      {
         /* Start client */
         client();
      }    
   }

   printf("Usage:- WflResMgr -c/-s (client | server)\n");
   exit( EXIT_FAILURE );
   
   return EXIT_FAILURE;
}