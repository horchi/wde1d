/*
 * update.c
 *
 * See the README file for copyright information
 *
 */

#include "wde1d.h"

void fixDecimalPoint(char* value)
{
   char* p = value;

   while (*p)
   {
      if (*p == ',')
         *p = '.';

      p++;
   }
}

int cUpdate::shutdown = no;
int cUpdate::trigger = no;

//***************************************************************************
// Class Update
//***************************************************************************

cUpdate::cUpdate()
{
   connection = 0;
   table = 0;

   cDbConnection::init();
   cDbConnection::setEncoding("utf8");
   cDbConnection::setHost(dbHost);
   cDbConnection::setPort(dbPort);
   cDbConnection::setName(dbName);
   cDbConnection::setUser(dbUser);
   cDbConnection::setPass(dbPass);

   line.setDetectPattern("module:cp210x");
   line.setDevice("/dev/ttyUSB1");
}

cUpdate::~cUpdate()
{
   cDbConnection::exit();
}

//***************************************************************************
// 
//***************************************************************************

int cUpdate::initDb()
{
   if (connection || table)
      exitDb();

   tell(0, "Try conneting to database");

   connection = new cDbConnection();

   table = new cTableSamples(connection);
   
   if (table->open() != success)
   {
      exitDb();
      
      return fail;
   }

   tell(0, "Connection to database established");  

   return success;
}

int cUpdate::exitDb()
{
   table->close();
   
   delete table;      table = 0;
   delete connection; connection = 0;

   return done;
}

//***************************************************************************
// Loop
//***************************************************************************

void cUpdate::loop()
{
   const int sizeMax = 200;

   char packet[sizeMax+TB];
   char value[sizeMax+TB];
   int size;
   double dLastRain = na;

   if (line.open() != success)
      return ;

   while (!doShutDown())
   {
      // anything to read ..

      if (line.check(1) != success)
         continue;

      // read serial data 
      
      size = line.readPacket(packet, sizeMax);

      if (size)
      {
         time_t now = time(0);
         
         tell(0, "Got (%d) [%s]", size, packet);
         
         csv.set(packet);
         
         for (int item = 0; csv.getNext(value) == success; item++)
         {
            double dValue;

            if (item < iFirst || item > iLast)
               continue;
         
            if (isEmpty(value))
               continue;

            fixDecimalPoint(value);
            dValue = atof(value);
            
            // special for rain sensor
            
            if (item == iRainVol)
            {
               int l = dLastRain;

               dLastRain = dValue;
               
               if (l == na)
                  dValue = 0;
               else if (dValue >= l)
                  dValue = dValue - l;
               else
                  dValue = (4096 - l) + dValue;
               
               dValue *= 0.295;        // [l/m] 1 => 295 ml/m
            }
            
            store(now, item, dValue);
         } 
      }
   }

   line.close();
}

//***************************************************************************
// Store
//***************************************************************************

int cUpdate::store(time_t now, int id, double value)
{
   int timeout = time(0) + 60;

   while (!table || !table->isConnected())
   {
      if (initDb() == success)
         break;

      if (time(0) > timeout || doShutDown())
      {
         tell(0, "Aborting reconnect after 60 seconds");
         return fail;
      }

      tell(0, "Retrying in 10 seconds");
      sleep(10);
   }

   table->setValue(cTableSamples::fiTime, now);
   table->setValue(cTableSamples::fiAddress, id);
   table->setValue(cTableSamples::fiType, "AI");
   table->setValue(cTableSamples::fiValue, value);
   
   table->store();
   
   return success;
}
