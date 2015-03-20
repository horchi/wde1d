
#include <stdint.h>   // uint_64_t
#include <sys/time.h>

#include <stdio.h>
#include <string>

#include "common.h"
#include "db.h"
#include "tabledef.h"

cDbConnection* connection = 0;

class cTimeMs {
private:
  uint64_t begin;
public:
  cTimeMs(int Ms = 0);
      ///< Creates a timer with ms resolution and an initial timeout of Ms.
      ///< If Ms is negative the timer is not initialized with the current
      ///< time.
  static uint64_t Now(void);
  void Set(int Ms = 0);
  bool TimedOut(void);
  uint64_t Elapsed(void);
  };

// --- cTimeMs ---------------------------------------------------------------

cTimeMs::cTimeMs(int Ms)
{
  if (Ms >= 0)
     Set(Ms);
  else
     begin = 0;
}

uint64_t cTimeMs::Now(void)
{
#define MIN_RESOLUTION 5 // ms
  static bool initialized = false;
  static bool monotonic = false;
  struct timespec tp;
  if (!initialized) {
     // check if monotonic timer is available and provides enough accurate resolution:
     if (clock_getres(CLOCK_MONOTONIC, &tp) == 0) {
        long Resolution = tp.tv_nsec;
        // require a minimum resolution:
        if (tp.tv_sec == 0 && tp.tv_nsec <= MIN_RESOLUTION * 1000000) {
           if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0) {
              monotonic = true;
              }
           else
              tell(0, "cTimeMs: clock_gettime(CLOCK_MONOTONIC) failed");
           }
        else
           tell(0, "cTimeMs: not using monotonic clock - resolution is too bad (%ld s %ld ns)", tp.tv_sec, tp.tv_nsec);
        }
     else
        tell(0, "cTimeMs: clock_getres(CLOCK_MONOTONIC) failed");
     initialized = true;
     }
  if (monotonic) {
     if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0)
        return (uint64_t(tp.tv_sec)) * 1000 + tp.tv_nsec / 1000000;
     tell(0, "cTimeMs: clock_gettime(CLOCK_MONOTONIC) failed");
     monotonic = false;
     // fall back to gettimeofday()
     }
  struct timeval t;
  if (gettimeofday(&t, NULL) == 0)
     return (uint64_t(t.tv_sec)) * 1000 + t.tv_usec / 1000;
  return 0;
}

void cTimeMs::Set(int Ms)
{
  begin = Now() + Ms;
}

bool cTimeMs::TimedOut(void)
{
  return Now() >= begin;
}

uint64_t cTimeMs::Elapsed(void)
{
  return Now() - begin;
}

//***************************************************************************
// Init Connection
//***************************************************************************

void initConnection()
{
   cDbConnection::init();

   cDbConnection::setEncoding("utf8");
   cDbConnection::setHost("localhost");
   // cDbConnection::setPort();
   cDbConnection::setName("wde1");
   cDbConnection::setUser("wde1");
   cDbConnection::setPass("wde1");
   Table::setConfPath("/etc");

   connection = new cDbConnection();
}

void exitConnection()
{
   cDbConnection::exit();
   
   if (connection)
      delete connection;
}

//***************************************************************************
// 
//***************************************************************************

//***************************************************************************
// 
//***************************************************************************

void chkStatement1()
{
   int status;
   
   Table* db = new Table(connection, "samples", cSampleFields::fields);
   
   if (db->open() != success)
   { 
      tell(0, "Could not access database '%s:%d' (%s)", 
           cDbConnection::getHost(), cDbConnection::getPort(), db->TableName());
      
      return ;
   }

   tell(0, "---------------------------------------------------");

    // prepare statement to mark wasted DVB events
   
   cDbStatement* s = new cDbStatement(db);
   
   db->setValue(cSampleFields::fiAddress, 1);
   db->setValue(cSampleFields::fiTime, time(0));
   db->setValue(cSampleFields::fiValue, atof("22.3"));

   db->store();

//    s->build("update %s set ", db->TableName());
//    s->bind(cEventFields::fiAddress, cDBS::bndIn | cDBS::bndSet);
//    s->bind(cEventFields::fiTime, cDBS::bndIn | cDBS::bndSet, ", ");
//    s->build(" where ");
//    s->bind(cEventFields::fiChannelId, cDBS::bndIn | cDBS::bndSet);
//    s->bind(cEventFields::fiSource, cDBS::bndIn | cDBS::bndSet, " and ");

//    s->bindCmp(0, endTime, ">", " and ");

//    s->bindCmp(0, cEventFields::fiStartTime, 0, "<" ,  " and ");
//    s->bindCmp(0, cEventFields::fiTableId,   0, ">" ,  " and (");
//    s->bindCmp(0, cEventFields::fiTableId,   0, "=" ,  " or (");
//    s->bindCmp(0, cEventFields::fiVersion,   0, "<>" , " and ");
//    s->build("));");
   
//    s->prepare();
   
   tell(0, "---------------------------------------------------");

   delete s;
   delete db;
}

void chkStatement2()
{
   int status;
   
   Table* db = new Table(connection, "sensorfacts", cSensorFactFields::fields);
   
   if (db->open() != success)
   { 
      tell(0, "Could not access database '%s:%d' (%s)", 
           cDbConnection::getHost(), cDbConnection::getPort(), db->TableName());
      
      return ;
   }

   tell(0, "---------------------------------------------------");

   // temp

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 3);
   db->setValue(cSensorFactFields::fiName,  "tempSensor1");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 4);
   db->setValue(cSensorFactFields::fiName,  "tempSensor2");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 5);
   db->setValue(cSensorFactFields::fiName,  "tempSensor3");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 6);
   db->setValue(cSensorFactFields::fiName,  "tempSensor4");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 7);
   db->setValue(cSensorFactFields::fiName,  "tempSensor5");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 8);
   db->setValue(cSensorFactFields::fiName,  "tempSensor6");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 9);
   db->setValue(cSensorFactFields::fiName,  "tempSensor7");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 10);
   db->setValue(cSensorFactFields::fiName,  "tempSensor8");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   // hum

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 11);
   db->setValue(cSensorFactFields::fiName,  "humSensor1");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 12);
   db->setValue(cSensorFactFields::fiName,  "humSensor2");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 13);
   db->setValue(cSensorFactFields::fiName,  "humSensor3");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 14);
   db->setValue(cSensorFactFields::fiName,  "humSensor4");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 15);
   db->setValue(cSensorFactFields::fiName,  "humSensor5");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 16);
   db->setValue(cSensorFactFields::fiName,  "humSensor6");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 17);
   db->setValue(cSensorFactFields::fiName,  "humSensor7");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 18);
   db->setValue(cSensorFactFields::fiName,  "humSensor8");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "");
   db->store();

   // kombi sensor

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 19);
   db->setValue(cSensorFactFields::fiName,  "tempKombi");
   db->setValue(cSensorFactFields::fiUnit,  "°C");
   db->setValue(cSensorFactFields::fiTitle, "Temperatur außen");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 20);
   db->setValue(cSensorFactFields::fiName,  "humKombi");
   db->setValue(cSensorFactFields::fiUnit,  "%");
   db->setValue(cSensorFactFields::fiTitle, "Luftfeuchte außen");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 21);
   db->setValue(cSensorFactFields::fiName,  "windKombi");
   db->setValue(cSensorFactFields::fiUnit,  "km/h");
   db->setValue(cSensorFactFields::fiTitle, "Wind");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 22);
   db->setValue(cSensorFactFields::fiName,  "rainvolKombi");
   db->setValue(cSensorFactFields::fiUnit,  "l/m²");
   db->setValue(cSensorFactFields::fiTitle, "Niederschalgsmenge");
   db->store();

   db->clear();
   db->setValue(cSensorFactFields::fiAddress, 23);
   db->setValue(cSensorFactFields::fiName,  "rainKombi");
   db->setValue(cSensorFactFields::fiUnit,  "1|0");
   db->setValue(cSensorFactFields::fiTitle, "Regen");
   db->store();

   tell(0, "---------------------------------------------------");

   delete db;
}

//***************************************************************************
// Main
//***************************************************************************

int main()
{
   logstdout = yes;
   loglevel = 2;

   initConnection();

   chkStatement2();

   exitConnection();

   return 0;
}
