/*
 * csv.h: WDE1 (Weather) Deamon
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __CSV_H__
#define __CSV_H__

//*************************************************************************
// Class Csv
//*************************************************************************

class Csv
{
   public:

      // object

      Csv(char aDelimiter = ';');
      ~Csv();

      // interface

      virtual int set(const char* data);
      virtual void clear();

      virtual int getNext(char* value);
  
   protected: 

      // functions

      char eat()         { return *pBuf ? *(++pBuf) : *pBuf; }
      char last()        { return *pBuf; }

      int isChar(char c) { return *pBuf == c; }
      int isDelimiter()  { return *pBuf == delimiter; }
      int isEnd()        { return !*pBuf; }

      // data

      char delimiter;
      char* pBuf;
      char* buffer;
};

//*************************************************************************

#endif // __CSV_H__
