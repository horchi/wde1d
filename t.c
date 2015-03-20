
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
//***************************************************************************
// Left Trim
//***************************************************************************

char* lTrim(char* buf)
{
   if (buf)
   {
      char *tp = buf;

      while (*tp && strchr("\n\r\t ",*tp)) 
         tp++;

      memmove(buf, tp, strlen(tp) +1);
   }
   
   return buf;
}

//*************************************************************************
// Right Trim
//*************************************************************************

char* rTrim(char* buf)
{
   if (buf)
   {
      char *tp = buf + strlen(buf);

      while (tp >= buf && strchr("\n\r\t ",*tp)) 
         tp--;

      *(tp+1) = 0;
   }
   
   return buf;
}

//*************************************************************************
// All Trim
//*************************************************************************

char* allTrim(char* buf)
{
   return lTrim(rTrim(buf));
}

int isEmpty(const char* str)
{
   return !str || !*str;
}

int main()
{
   FILE* fp;
   char line[500+1]; *line = 0;
   char *c, *p, *name;
   char* value;
   const char* file = "./xx.txt";

   fp = fopen(file, "r");
   
   if (fp)
   {
      while (c = fgets(line, 500, fp))
      {
         // cut linefeed
         
         line[strlen(line)] = 0;

         // cut comments
         
         if (p = strstr(line, "//"))
            *p = 0;

         allTrim(line);

         if (isEmpty(line))
            continue;

         // check line, search value
         
         if (!(name = strstr(line, "var ")) || !(value = strchr(line, '=')))
         {
            printf("Info: Ignoring invalid line [%s] in '%s'\n", line, file);
            continue;
         }
         
         if (name >= value)   // check positions
         {
            printf("Info: Ignoring invalid line [%s] in '%s'\n", line, file);
            continue;
         }
         
         name += strlen("var ");
         *value = 0;
         value++;
         
         allTrim(name);
         allTrim(value);

         printf("append variable '%s' with vaue '%s'\n", name, value);
         
      }
      
      fclose(fp);
   }
   else
      printf("Can't open '%s', error was '%s'\n", file, strerror(errno));
   
   return 0;
}
