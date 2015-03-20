/*
 * serial.h: WDE1 (Weather) Deamon
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <termios.h>
#include <strings.h>

#include "lib/common.h"

//***************************************************************************
// 
//***************************************************************************

class cSerialLine
{
   public:

      cSerialLine();
      ~cSerialLine()  { free(pattern); close(); }

      int open();
      int close();
      int isOpen()  { return fd != na; }

      int check(int timeout = 1);
      int read();
      int readPacket(char* packet, int max);

      void setDevice(const char* aDevice) 
      { free(deviceName); deviceName = strdup(aDevice); }

      void setDetectPattern(const char* aPattern) 
      { free(pattern); pattern = strdup(aPattern); };

   private:

      int detect();
   
      // data

      int dataBytesSend;

      int mode;
      int dataBytes;
      int byteMode;
      int byteStart;
      int byteEnd;

      char* deviceName;
      char* pattern;
      int fd;
      struct termios oldtio;
};

//*************************************************************************

#endif // __serial_H__
