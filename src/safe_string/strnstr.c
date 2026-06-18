#include "libs_support.h"
#include "util.h"
#include <string.h>

/* -------------------------------------- strnstr ------------------------------------------------

   strnstr() is in BSD, but it's not a standard C or Posix function. This is the BSD source.
*/
char * strnstr(const char *s, const char *find, size_t slen)
{
   char c, sc;
   size_t len;

   if ((c = *find++) != '\0') {
      len = strlen(find);
      do {
         do {
            if (slen-- < 1 || (sc = *s++) == '\0')
               return (NULL);
         } while (sc != c);
         if (len > slen)
            return (NULL);
      } while (strncmp(s, find, len) != 0);
      s--;
   }
   return ((char *)s);
}

// ------------------------------------ eof ----------------------------------------------
