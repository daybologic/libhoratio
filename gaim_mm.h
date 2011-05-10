/* Header for DPCRTLMM use in GAIM, this file by DDRP
(Overlord@DayboLogic.co.uk)

Attention programmers: To enable trapping in a C module include
this header, all memory management calls will be rerouted to
DPCRTLMM.  No point disabling traps with special library calls
and you can insulate yourselves from depending on my library
this way if you wish to.

Simple instructions:
All calls are macros starting with GAIM_MM_....
It might be an idea to rename main() gaimmain() and then replace
main with code similar to this:

#include <stdlib.h>
int main(const int argc, const char* argv[])
{
  int gaimReturn;
  GAIM_MM_BEGIN
  gaimReturn = gaimmain(argc, argv);
  GAIM_MM_END
  return gaimReturn;
}

int gaimmain(const int argc, const char* argv[])
{
  your code would go here
}

*/
/*---------------------------------------------------------------*/
#ifndef __INC_GAIM_MM_H
#define __INC_GAIM_MM_H

/* You might have to change these includes to dpcrtlmm/dpcrtlmm.h etc. */
#include "dpcrtlmm.h" /* Library C header */
#ifdef __cplusplus
#  include "dpccap.h" /* C++ support, you probally won't use this */
#endif /*__cplusplus*/

/* GAIM_MM_... macros */
#define GAIM_MM_BEGIN dpcrtlmm_Startup();
#define GAIM_MM_END dpcrtlmm_Shutdown();

/* Routing macros, you'll need stddef.h included for NULL,
in future versions of DPCRTLMM I may include __LINE__ and __FILE__
passed in an extended version of dpcrtlmm_Alloc() etc. */

#define malloc(size) dpcrtlmm_Alloc(NULL, (size));
#define free(p) dpcrtlmm_Free(NULL, (p));
#define calloc(nitems, size) dpcrtlmm_Calloc(NULL, (ntimes), (size));
#define realloc(p, newsize), dpcrtlmm_Realloc(NULL, (nitems), (newsize));

/* Notes about 16-bit compatibillty you won't use anyway:
By default all disabled but the lib can be rebuilt with all pointers
explicity far, things go wildly wrong with near data and DPCRTLMM
see build.h if you want to play with any settings... then rebuild
the library. */
/*---------------------------------------------------------------*/
#endif /*!__INC_GAIM_MM_H*/