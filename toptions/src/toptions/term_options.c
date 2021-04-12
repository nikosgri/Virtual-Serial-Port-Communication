#include <stdio.h>   //standard input output
#include <string.h>  // string functions
#include <unistd.h>
#include <fcntl.h>   //file control
#include <errno.h> 
#include <termios.h> // posix terminal
#include <stdlib.h>
#include <sys/types.h>
#include "term_options.h"


void set_block(int fd,int block){

	struct termios tty;
	
	memset(&tty,0,sizeof(tty));
	
	if(tcgetattr(fd,&tty)!=0) {
		 printf("Error %i from tcgetattr : %s \n",errno,strerror(errno));
		 exit(1);
	}
	
	tty.c_cc[VMIN]=block;	
	tty.c_cc[VTIME]=10;           // 1 second read timeout
	tcflush(fd,TCIFLUSH);         // Flushes written data , but not read
	tcsetattr(fd,TCSANOW,&tty);

}

void set_attributes(int fd,int speed,int parity){
	
	struct termios tty;


	//Read existing settings and handle errors.
	if(tcgetattr(fd,&tty)!=0){
		 printf("Error %i from tcgetattr : %s \n",errno,strerror(errno));
		 exit(1);
	}
	cfsetospeed(&tty,speed);
	cfsetispeed(&tty,speed);
	
	tty.c_cflag=(tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
	tty.c_iflag &= ~IGNBRK;                   // Disable break processing 
	tty.c_lflag  = 0;					      // No echo or signaling chars
	tty.c_oflag  = 0;                         // no remaping no delays
	tty.c_cc[VMIN]=0;						  
	tty.c_cc[VTIME]=10;                       // 1 sec read timeout
	
	tty.c_iflag &= ~(IXON | IXOFF |IXANY);
	
	tty.c_cflag |= (CLOCAL |CREAD);          // Ignore modem controls enable reading 
	tty.c_cflag &= ~(PARENB | PARODD);       // off parity bit
	tty.c_cflag |=parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;                 //Disable RTS/CTS hardware flow control
	
	tcsetattr(fd,TCSANOW,&tty);


}

