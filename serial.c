/*
 * serial.c: WDE1 (Weather) Deamon
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "serial.h"

//***************************************************************************
// Class cSerialLine
//***************************************************************************
//***************************************************************************
// Object
//***************************************************************************

cSerialLine::cSerialLine()
{
   dataBytesSend = 0;
   fd = na;  
   bzero(&oldtio, sizeof(oldtio)); 
   deviceName = 0;
   pattern = 0;
}

//***************************************************************************
// Detect
//***************************************************************************

int cSerialLine::detect()
{
   FILE* fd;
   regex_t reg;
   char line[200];
   int detected = no;
   
   if (regcomp(&reg, pattern, REG_EXTENDED | REG_NOSUB))
   {
      tell(0, "Invalid regular expression '%s'for usb device", pattern);
      regfree(&reg);
      return fail;
   }
   
   if (!(fd = fopen("/proc/tty/driver/usbserial", "r")))
   {
      tell(0, "Could not open '/proc/tty/driver/usbserial' '%m'");
      
      regfree(&reg);
      return fail;
   }
   
   while (fgets(line, sizeof(line), fd)) 
   {
      char* p;
      
      if (!regexec(&reg, line, 0, 0, 0) && (p = index(line, ':')))
      {
         detected = yes;
         free(deviceName);
         *p = 0;
         asprintf(&deviceName, "/dev/ttyUSB%s", line);

         break;
      }
   }
   
   fclose(fd);
   regfree(&reg);
   
   if (!detected) 
   {
      tell(1, "Could not auto detect a usb device like '%s' "
           "in '/proc/tty/driver/usbserial'", pattern);
      return fail;
   }
   
   return success;
}

//***************************************************************************
// Open/Close
//***************************************************************************

int cSerialLine::open()
{
   struct termios newtio;

   if (detect() != success)
      tell(1, "Falling back to '%s'", deviceName);

   if (isOpen())
      close();

	// open serial line with 8 data bits, no parity, 1 stop bit

   if ((fd = ::open(deviceName, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
   {
      fd = na;
      tell(0, "Error: Opening '%s' failed", deviceName);
  
      return fail;
   }

   tell(0, "Opening '%s' succeeded!", deviceName);
   
   // configure serial line

   tcgetattr(fd, &oldtio);
   bzero(&newtio, sizeof(newtio));

   /* BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
      CRTSCTS : output hardware flow control (only used if the cable has
                all necessary lines. See sect. 7 of Serial-HOWTO)
      CS8     : 8n1 (8bit,no parity,1 stopbit)
      CLOCAL  : local connection, no modem control
      CREAD   : enable receiving characters  */
   
   newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
   newtio.c_iflag = IGNPAR;
   newtio.c_oflag = 0;
   newtio.c_lflag = 0;

   if (tcsetattr(fd, TCSANOW, &newtio) != 0)
      tell(0, "tcsetattr failed!");

   tcflush(fd, TCIFLUSH);

   return success;
}

int cSerialLine::close()
{
   if (isOpen())
   {
      tcsetattr(fd, TCSANOW, &oldtio);
      ::close(fd);
      fd = na;
   }

   free(deviceName);
   deviceName = 0;

   return success;
}

//***************************************************************************
// Read
//***************************************************************************

int cSerialLine::read()
{
   fd_set readfs;
   timeval tv;
   unsigned char c;

   if (!isOpen())
      return fail;

   // check if something to read ...
   
   tv.tv_sec = 0;
   tv.tv_usec = 100000;
   
   FD_ZERO(&readfs);
   FD_SET(fd, &readfs);
   select(fd+1, &readfs, 0, 0, &tv);

   if (FD_ISSET(fd, &readfs))
   {
      while (::read(fd, &c, 1) > 0)
         tell(2, "%02X ", c);
   }
   else
   {
      tell(3, ".. no data available");
      return fail;
   }

   return success;
}

int cSerialLine::readPacket(char* packet, int max)
{
   unsigned char c;
   int size = 0;
   char* p = packet;
   int res;
   int timeoutAt = time(0) + 10;

   *packet = 0;

   if (!isOpen())
      return fail;

   while ((res = ::read(fd, &c, 1)) >= 0 && size < max)
   {
      if (res == 0)
      {
         if (time(0) >= timeoutAt)
            break;

         continue;
      }

      if (c == '\n')
         break;
      
      if (c == '\r')
         continue;

      size++;         
      *p++ = c;
   }
   
   if (res < 0)
      tell(0, "Read failed '%s'", strerror(errno));

   *p = 0;  // terminate

   return size;
}

//***************************************************************************
// Check Line
//***************************************************************************

int cSerialLine::check(int timeout)
{
   fd_set readfs;
   timeval tv;

   if (!isOpen())
      return fail;

   tv.tv_sec = timeout;
   tv.tv_usec = 0;

   // check if anything to read
   
   FD_ZERO(&readfs);
   FD_SET(fd, &readfs);
   select(fd+1, &readfs, 0, 0, &tv);

   if (FD_ISSET(fd, &readfs))
      return success;

   return fail;
}
