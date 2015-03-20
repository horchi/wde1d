/*
 * main.c: WDE1 (Weather) Deamon
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include "wde1d.h"

char* confDir = (char*)confDirDefault;

// defaults 

char dbHost[100+TB] = "localhost";
int dbPort;
char dbName[100+TB] = "wde1";
char dbUser[100+TB] = "wde1";
char dbPass[100+TB] = "wde1";

//***************************************************************************
// Configuration
//***************************************************************************

int atConfigItem(const char* Name, const char* Value)
{
   // Parse setup parameters and store values.
   
   if      (!strcasecmp(Name, "DbHost"))      sstrcpy(dbHost, Value, sizeof(dbHost));
   else if (!strcasecmp(Name, "DbPort"))      dbPort = atoi(Value);
   else if (!strcasecmp(Name, "DbName"))      sstrcpy(dbName, Value, sizeof(dbName));
   else if (!strcasecmp(Name, "DbUser"))      sstrcpy(dbUser, Value, sizeof(dbUser));
   else if (!strcasecmp(Name, "DbPass"))      sstrcpy(dbPass, Value, sizeof(dbPass));
   
   else if (!strcasecmp(Name, "LogLevel"))    loglevel = atoi(Value);
   
   else
      return fail;
   
   return success;
}

//***************************************************************************
// Read Config
//***************************************************************************

int readConfig()
{
   int count = 0;
   FILE* f;
   char* line = 0;
   size_t size = 0;
   char* value;
   char* name;
   char* fileName;

   asprintf(&fileName, "%s/wde1d.conf", confDir);

   if (!fileName || access(fileName, F_OK) != 0)
   {
      printf("Cannot access configuration file '%s'\n", fileName ? fileName : "<null>");
      free(fileName);
      return fail;
   }

   f = fopen(fileName, "r");

   while (getline(&line, &size, f) > 0)
   {
      char* p = strchr(line, '#');
      if (p) *p = 0;

      allTrim(line);

      if (isEmpty(line))
         continue;

      if (!(value = strchr(line, '=')))
         continue;
      
      *value = 0;
      value++;
      lTrim(value);
      name = line;
      allTrim(name);

      if (atConfigItem(name, value) != success)
      {
         printf("Found unexpected parameter '%s', aborting\n", name);
         free(fileName);
         return fail;
      }

      count++;
   }

   free(line);
   fclose(f);

   tell(0, "Read %d option from %s", count , fileName);

   free(fileName);

   return success;
}

void showUsage()
{
   printf("Usage: wde1d [-n][-c <config-dir>][-l <log-level>][-t]\n");
   printf("    -n              don't daemonize\n");
   printf("    -t              log to stdout\n");
   printf("    -v              show version\n");
   printf("    -c <config-dir> use config in <config-dir>\n");
   printf("    -l <log-level>  set log level\n");
}

//***************************************************************************
// Main
//***************************************************************************

int main(int argc, char** argv)
{
   cUpdate* job;
   int nofork = no;
   int pid;
   int _stdout = na;
   int _level = na;

   // Usage ..

   if (argc > 1 && (argv[1][0] == '?' || (strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)))
   {
      showUsage();
      return 0;
   }

   // Parse command line

   for (int i = 0; argv[i]; i++)
   {
      if (argv[i][0] != '-' || strlen(argv[i]) != 2)
         continue;

      switch (argv[i][1])
      {
         case 'l': if (argv[i+1]) _level = atoi(argv[i+1]); break;
         case 't': _stdout = yes;                           break;
         case 'n': nofork = yes;                            break;
         case 'c': if (argv[i+1]) confDir = argv[i+1];      break;
         case 'v': printf("Version %s\n", VERSION);  return 1;
      }
   }

   // read configuration ..

   if (readConfig() != success)
      return 1;

   if (_stdout != na) logstdout = _stdout;
   if (_level != na)  loglevel = _level;

   job = new cUpdate();

   // fork daemon

   if (!nofork)
   {
      if ((pid = fork()) < 0)
      {
         printf("Can't fork daemon, %s\n", strerror(errno));
         return 1;
      }
      
      if (pid != 0)
         return 0;
   }

   // register SIGINT

   ::signal(SIGINT, cUpdate::downF);
   ::signal(SIGTERM, cUpdate::downF);
   ::signal(SIGHUP, cUpdate::triggerF);

   // do work ...

   job->loop();

   // shutdown

   tell(0, "shutdown");

   delete job;

   return 1;
}
