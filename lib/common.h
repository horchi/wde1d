/*
 * common.h: EPG2VDR plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __COMMON_H
#define __COMMON_H

#include <stdlib.h>
#include <string>

struct CurlMemoryStruct;
extern int loglevel;
extern int logstdout;

using namespace std;

//***************************************************************************
// MemoryStruct for curl callbacks
//***************************************************************************

struct CurlMemoryStruct 
{
   CurlMemoryStruct()   { memory = 0; clear(); }
   ~CurlMemoryStruct()  { clear(); }

   // data

   char* memory;
   size_t size;

   // tag attribute

   int match;
   char tag[100];          // the tag to be compared 
   char ignoreTag[100];

   void clear() 
   {
      free(memory);
      memory = 0;
      size = 0;
      match = 0;
      *tag = 0;
      *ignoreTag = 0;
   }
};

//***************************************************************************
// 
//***************************************************************************

inline long min(long a, long b) { return a < b ? a : b; }

enum Misc
{
   success = 0,
   done    = success,
   fail    = -1,
   na      = -1,
   ignore  = -2,
   all     = -3,
   yes     = 1,
   on      = 1,
   off     = 0,
   no      = 0,
   TB      = 1,

   tmeSecondsHour = 60 * 60,
   tmeSecondsPerDay = 24 * tmeSecondsHour
};

//***************************************************************************
// Tell
//***************************************************************************

void tell(int eloquence, const char* format, ...);

//***************************************************************************
// Tools
//***************************************************************************

void removeChars(string& str, string chars);
void removeWord(string& pattern, string word);
void prepareCompressed(string& pattern);

char* rTrim(char* buf);
char* lTrim(char* buf);
char* allTrim(char* buf);
char* sstrcpy(char* dest, const char* src, int max);
string num2Str(int num);
string l2pTime(time_t t);

int fileExists(const char* path);
int createLink(const char* link, const char* dest, int force);
int isLink(const char* path);
int isEmpty(const char* str);
int removeFile(const char* filename);

const char* getHostName();
const char* getFirstIp();

#ifdef WITH_GUNZIP
//***************************************************************************
// Zip
//***************************************************************************

int gunzip(CurlMemoryStruct* zippedData, CurlMemoryStruct* unzippedData);
void tellZipError(int errorCode, const char* op, const char* msg);

#endif // WITH_GUNZIP

//***************************************************************************
#endif //___COMMON_H
