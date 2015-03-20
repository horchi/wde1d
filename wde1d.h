/*
 * wse1.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __WSED1_H
#define __WSED1_H

//***************************************************************************

#include <unistd.h>

#include "lib/common.h"
#include "lib/db.h"
#include "lib/tabledef.h"

#include "serial.h"
#include "csv.h"

//***************************************************************************

#define VERSION      "0.0.1"
#define confDirDefault "/etc"

extern char dbHost[100+TB];
extern int  dbPort;
extern char dbName[100+TB];
extern char dbUser[100+TB];
extern char dbPass[100+TB];

//***************************************************************************
// Update
//***************************************************************************

class cUpdate
{
   public:

      // declarations

      enum Item
      {
         iStart,             // $1;    Startzeichen, Kanal 1
         iState,             //  1;    Zustand (immer 1);
         iSrcSp,             //  -;    Zeitstempel (ohne)

         iFirst,
         iTemp1 = iFirst,    // 21,2;   3 - Temperatur Sensor 1 (°C)
         iTemp2,             // 21,2;   4 - Temperatur Sensor 2 (°C)
         iTemp3,             // 21,2;   5 - Temperatur Sensor 3 (°C)
         iTemp4,             // 21,2;   6 - Temperatur Sensor 4 (°C)
         iTemp5,             // 21,2;   7 - Temperatur Sensor 5 (°C)
         iTemp6,             // 21,2;   8 - Temperatur Sensor 6 (°C)
         iTemp7,             // 21,2;   9 - Temperatur Sensor 7 (°C)
         iTemp8,             // 21,2;  10 - Temperatur Sensor 8 (°C)

         iHum1,              // 67;    11 - Feuchte Sensor 1 (%)
         iHum2,              // 67;    12 - Feuchte Sensor 2 (%)
         iHum3,              // 67;    13 - Feuchte Sensor 3 (%)
         iHum4,              // 67;    14 - Feuchte Sensor 4 (%)
         iHum5,              // 67;    15 - Feuchte Sensor 5 (%)
         iHum6,              // 67;    16 - Feuchte Sensor 6 (%)
         iHum7,              // 67;    17 - Feuchte Sensor 7 (%)
         iHum8,              // 67;    18 - Feuchte Sensor 8 (%)

         iTemp,              // 22,7   19 - Temperatur Kombisensor (Â°C)
         iHum,               // 42;    20 - Feuchte Kombisensor (%)
         iWind,              // 8,9;   21 - Windgeschwindigkeit (km/h)
         iRainVol,           // 3200;  22 - Niederschlag (WippenschlÃ¤ge)
         iRain,              // 1;     23 - Regen (Ja=1, Nein=0)

         iLast = iRain,

         iEnd                // 0<cr> <lf> - Stoppzeichen
      };

      // object

      cUpdate();
      ~cUpdate();

      // interface 

      void loop();

      static void downF(int aSignal)    { shutdown = yes; }
      static void triggerF(int aSignal) { trigger = yes; }

   private:

      int initDb();
      int exitDb();
      int store(time_t now, int id, double value);

      int doShutDown()    { return shutdown; }

      // data

      cSerialLine line;
      Csv csv;
      cDbTable* table;
      cDbConnection* connection;

      static int shutdown;
      static int trigger;
};

//***************************************************************************

#endif // __WSED1_H
