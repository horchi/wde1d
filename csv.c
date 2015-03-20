/*
 * csc.c: WDE1 (Weather) Deamon
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

//*************************************************************************
// Includes
//*************************************************************************

#include <string.h>

#include "lib/common.h"
#include "csv.h"

//*************************************************************************
// Class Csv
//*************************************************************************

Csv::Csv(char aDelimiter)
{
   delimiter = aDelimiter;
   buffer = 0;
   pBuf = 0;
}

Csv::~Csv()
{
}

//***************************************************************************
// Set / Clear
//***************************************************************************

int Csv::set(const char* data)
{
   clear();

   buffer = strdup(data);
   pBuf = buffer;
   
   return success;
}

void Csv::clear()
{
   free(buffer);
   buffer = 0;
   pBuf = 0; 
}

//*************************************************************************
// getItem
//*************************************************************************

int Csv::getNext(char* value)
{
   if (isEnd())
      return fail;

   char* v = value;

   while (!isDelimiter() && !isEnd())
   {
      *(v++) = *(pBuf++);
   }

   *v = 0;

   allTrim(value);

   if (!isEnd())
   {
      if (!isDelimiter())
         return fail;
      
      eat();
   }

   return success;
}


