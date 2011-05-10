/*
    DPCRTLMM Memory Management Library : Configurator.
    Copyright (C) 2000 Overlord David Duncan Ross Palmer, Daybo Logic.
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Contact me: Overlord@DayboLogic.co.uk
Get updates: http://www.daybologic.co.uk/dev/dpcrtlmm
My official site: http://www.daybologic.co.uk/overlord
*/

/* Portable auto-configuration program for DPCRTLMM */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HDRSTOP
# pragma hdrstop
#endif

#include "dpcrtlmm.h" /* For calculating sizes of structures */

static const char header[] = "config.h";
static const char gpl[] = "template";
static int config_debug = 0; /* Release mode by default */
enum {
  endianForceLittle,
  endianForceBig,
  endianAuto
} config_endian = endianAuto;
static int config_threads = 0; /* Thread support disabled by default */
static int config_log = 1; /* Log file DPCRTLMM.LOG used by default */
static int config_inline = 0; /* Don't use new ANSI keyword by default */
static int config_stderr = 0; /* Do not use stderr, use stdout by default */
static int config_hookchainsize = 32; /* How many hooks per handler, 32 by default , may be 0 to disable the debug hook system */
static int config_maxsafetyentries = 512; /* Allows for 512 modules by default! */
static int config_wantfardata = 0; /* For obsolete computers in small memory models */
static int config_nonullarray = 0; /* For backwards compatibillity, removes the NULL (default) array, default = 0 */
static int config_maxport = 0; /* For mental portabillty at any cost! */
static int config_private = 0; /* For marking a build for a private tree which will not be released */
static int config_special = 0; /* For marking a specially modified or specially built library, not neccersarily private */

static void Banner(void);
static int MakeHeader(void);
static int RemakeHeader(void);
static int crapcopy(const char* source, const char* dest);
static int ConfigEndian(FILE* FileHandle);
static int ConfigDebug(FILE* FileHandle);
static int ConfigLongSize(FILE* FileHandle);
static int ConfigRegisterSize(FILE* FileHandle);
static int ConfigThreads(FILE* FileHandle);
static int ConfigInline(FILE* FileHandle);
static int ConfigStderr(FILE* FileHandle);
static int ConfigDebugHooks(FILE* FileHandle);
static int ConfigSafetyList(FILE* FileHandle);
static int ConfigFarData(FILE* FileHandle);
static int ConfigNULLArray(FILE* FileHandle);
static int ConfigLog(FILE* FileHandle);
static int ConfigMaxPort(FILE* FileHandle);
static int ConfigPrivate(FILE* FileHandle);
static int ConfigSpecial(FILE* FileHandle);

static void Syntax(void);

int main(const int argc, const char* argv[])
{
  int i;

  Banner();
  for ( i = 1U; i < argc; i++ ) {
    if ( strcmp(argv[i], "--enable-debug") == 0 ) {
      config_debug = 1;
      continue;
    }
    if ( strcmp(argv[i], "--disable-debug") == 0 ) {
      config_debug = 0;
      continue;
    }
    if ( strcmp(argv[i], "--force-bigendian") == 0 ) {
      config_endian = endianForceBig;
      continue;
    }
    if ( strcmp(argv[i], "--force-littleendian") == 0 ) {
      config_endian = endianForceLittle;
      continue;
    }
    if ( strcmp(argv[i], "--auto-endian") == 0 ) {
      config_endian = endianAuto;
      continue;
    }
    if ( strcmp(argv[i], "--disable-threads") == 0 ) {
      config_threads = 0;
      continue;
    }
    if ( strcmp(argv[i], "--enable-threads") == 0 ) {
      config_threads = 1;
      continue;
    }
    if ( strcmp(argv[i], "--enable-inline") == 0 ) {
      config_inline = 1;
      continue;
    }
    if ( strcmp(argv[i], "--disable-inline") == 0 ) {
      config_inline = 0;
      continue;
    }
    if ( strcmp(argv[i], "--enable-stderr") == 0 ) {
      config_stderr = 1;
      continue;
    }
    if ( strcmp(argv[i], "--disable-stderr") == 0 ) {
      config_stderr = 0;
      continue;
    }
    if ( strcmp(argv[i], "--far") == 0 ) {
      config_wantfardata = 1;
      continue;
    }
    if ( strcmp(argv[i], "--near") == 0 ) {
      config_wantfardata = 0;
      continue;
    }
    if ( strcmp(argv[i], "--disable-null-array") == 0 ) {
      config_nonullarray = 1;
      continue;
    }
    if ( strcmp(argv[i], "--enable-null-array") == 0 ) {
      config_nonullarray = 0;
      continue;
    }
    if ( strcmp(argv[i], "--enable-log") == 0 ) {
      config_log = 1;
      continue;
    }
    if ( strcmp(argv[i], "--disable-log") == 0 ) {
      config_log = 0;
      continue;
    }
    if ( strcmp(argv[i], "--maxport") == 0 ) {
      config_maxport = 1;
      continue;
    }
    if ( strcmp(argv[i], "--private") == 0 ) {
      config_private = 1;
      continue;
    }
    if ( strcmp(argv[i], "--special") == 0 ) {
      config_special = 1;
      continue;
    }
    if ( strcmp(argv[i], "--help") == 0 ) {
      Syntax();
      return EXIT_SUCCESS;
    }
    if ( strcmp(argv[i], "--hook-chain-size") == 0 ) {
      printf("Hook chain size requires an argument!\n\n");
      Syntax();
      return EXIT_FAILURE;
    }
    if ( strncmp(argv[i], "--hook-chain-size:", 18) == 0 ) {
      unsigned int hookchainsize;
      const char* numptr;
      if ( strlen(argv[i]) == 18 ) {
        printf("ERROR: --hook-chain-size: did not specify correctly\n");
        return EXIT_FAILURE;
      }
      numptr = argv[i];
      numptr += 18; /* Move along to number */
      hookchainsize = atoi(numptr);
      if ( !hookchainsize ) {
        if ( numptr[0] != '0' ) {
          /* atoi() reported zero but actually the user put some nonsense in, tell them off! */
          printf("ERROR: --hook-chain-size expects a positive integer.\n");
          return EXIT_FAILURE;
        }
      }
      config_hookchainsize = hookchainsize; /* Validated */
      continue;
    }
    if ( strcmp(argv[i], "--safety-list-size") == 0 ) {
      printf("The maximum number of entries in the safety list requires an argument!\n\n");
      Syntax();
      return EXIT_FAILURE;
    }
    if ( strncmp(argv[i], "--safety-list-size:", 19) == 0 ) {
      unsigned int maxsafetyentries;
      const char* numptr;
      if ( strlen(argv[i]) == 19 ) {
        printf("ERROR: --safety-list-size: did not specify correctly.\n");
        return EXIT_FAILURE;
      }
      numptr = argv[i];
      numptr += 19; /* Move along to number */
      maxsafetyentries = atoi(numptr);
      if ( !maxsafetyentries ) {
        if ( numptr[0] != '0' ) {
          /* Not sent a valid integer after : */
          printf("ERROR: --safety-list-size expects a positive integer.\n");
          return EXIT_FAILURE;
        }
      }
      config_maxsafetyentries = maxsafetyentries; /* Validated */
      continue;
    }
  }

  printf("Configuring build...\n");
  if ( MakeHeader() )
  {
    printf("Configuration complete.\n");
    return EXIT_SUCCESS;
  }

  printf("Configuration failed\n");
  return EXIT_FAILURE;
}

static void Banner()
{
  printf("DPCRTLMM configurator (C)2001 DDRP, Daybo Logic, Overlord@DayboLogic.co.uk\n");
  printf("Distributed under the GNU General Public License\n\n");
}

static int MakeHeader()
{
  int status;
  FILE* Hfile;

  status = RemakeHeader();
  if ( !status ) return status;

  Hfile = fopen(header, "ab");
  if ( !Hfile ) {
    printf("ERROR! Problems reopening header %s\n", header);
    return 0;
  }
  fprintf(Hfile, "/* Auto generated header - do not edit */\n\n");
  status = ConfigEndian(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigDebug(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigLongSize(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigRegisterSize(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigThreads(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigInline(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigStderr(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigDebugHooks(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigSafetyList(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigFarData(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigNULLArray(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigLog(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigMaxPort(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigPrivate(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }
  status = ConfigSpecial(Hfile);
  if ( !status ) {
    fclose(Hfile);
    return status;
  }

  printf("Header created\n");
  fclose(Hfile);
  return 1;
}

static int RemakeHeader()
{
  int status;

  printf("Creating %s...\n", header);
  if ( remove(header) != 0 ) {
    printf("ERROR! Can't remove previous %s config file, the build will fail\n", header);
    return 0;
  }

  printf("GPLizing!...");
  status = crapcopy(gpl, header);
  if ( !status ) return status;

  return 1;
}

static int crapcopy(const char* source, const char* dest)
{
  FILE *hsource, *hdest;

  printf("Copy \"%s\"  \"%s\"\n", source, dest);
  hsource = fopen(source, "rb");
  if ( !hsource ) {
    printf("ERROR! Can\'t open %s to read!  The build will fail.\n", source);
    return 0;
  }
  hdest = fopen(dest, "wb");
  if ( !hdest ) {
    fclose(hsource);
    printf("ERROR! Can\'t open %s to write!  The build will fail.\n", dest);
    return 0;
  }

  /* Note: This is a crap lazy way to copy a file, but the autoconfiguration is
  far from a critical section of the code. */
  do {
    unsigned char c;
    c = (unsigned char)fgetc(hsource);
    if ( c != (unsigned char)EOF ) {
      if ( fputc((int)c, hdest) == EOF ) {
        fclose(hsource);
        fclose(hdest);
        printf("ERROR! Probally out of space on the medium!\n");
        return 0;
      }
    }
  } while ( !feof(hsource) );
  fclose(hsource);
  fclose(hdest);
  return 1; /* ok */
}

static int ConfigEndian(FILE* FileHandle)
{
  static int status = 1;
  const unsigned short test = 0x1234;
  const unsigned char* ptest = (unsigned char*)&test;

  printf("Is machine big endian? ... ");

  fputc('\n', FileHandle);
  if ( *ptest == 0x12 || config_endian == endianForceBig ) {
    printf("yes");
    fprintf(FileHandle, "#define __B_ENDIAN__");
  }
  else if ( *ptest == 0x34 || config_endian == endianForceLittle ) {
    printf("no");
    fprintf(FileHandle, "#define __L_ENDIAN__");
  }
  else {
    printf("UNKNOWN (FAIL)");
    fprintf(FileHandle, "#error (\"configure could not determine endian - contact me! overlord@daybologic.co.uk\")");
  }

  fputc('\n', stdout);
  fputc('\n', FileHandle);
  return status;
}

static int ConfigDebug(FILE* FileHandle)
{
  static int status = 1;

  printf("Is debug mode? ... ");

  fprintf(FileHandle, "\n/* Autoconfigurated debug options */\n");
  if ( config_debug ) {
    printf("yes");
    fprintf(FileHandle, "#ifndef DEBUG\n# define DEBUG\n#endif\n");
    fprintf(FileHandle, "#ifdef NDEBUG\n# undef NDEBUG\n#endif\n");
  }
  else {
    printf("no");
    fprintf(FileHandle, "#ifdef DEBUG\n# undef DEBUG\n#endif\n");
    fprintf(FileHandle, "#ifndef NDEBUG\n# define NDEBUG\n#endif\n");
  }
  fputc('\n', stdout);
  fputc('\n', FileHandle);
  return status;
}

static int ConfigLongSize(FILE* FileHandle)
{
  unsigned int bitsize;

  printf("sizeof() long ... ");

  fprintf(FileHandle, "\n/* Size of type long, generally means maximum register length */\n");
  fprintf(FileHandle, "#define DPCRTLMM_LONGSIZE (");

  bitsize = sizeof(long) * 8;
  printf("%u-bit\n", bitsize);
  fprintf(FileHandle, "%u)\n", bitsize);

  return 1;
}

static int ConfigRegisterSize(FILE* FileHandle)
{
  unsigned int bitsize;

  printf("sizeof() int ... ");

  fprintf(FileHandle, "\n/* Size of type int, generally defines maximum sized chuck */\n");
  fprintf(FileHandle, "#define DPCRTLMM_INTSIZE (");

  bitsize = sizeof(int) * 8;
  printf("%u-bit\n", bitsize);
  fprintf(FileHandle, "%u)\n", bitsize);

  if ( bitsize < 16 )
    printf("CRITICAL! %u-bit ints, I have no idea if this will work!!\n", bitsize);

  return 1;
}

static int ConfigThreads(FILE* FileHandle)
{
  printf("Add thread support? ... ");

  if ( config_threads ) {
    printf("yes\n");
    fprintf(FileHandle, "\n#define DPCRTLMM_THREADS\n");
  }
  else {
    printf("no\n");
    fprintf(FileHandle, "\n/*#define DPCRTLMM_THREADS*/\n");
  }
  return 1;
}

static int ConfigInline(FILE* FileHandle)
{
  printf("Supporting keyword \'inline\'? ... ");

  if ( config_inline ) {
    printf("yes\n");
    fprintf(FileHandle, "\n#define _INLINE inline /* C99, use inline keyword to speed things up */\n");
  }
  else {
    printf("no\n");
    fprintf(FileHandle, "\n#define _INLINE  /* Remove references to inline as the keyword is not supported */\n");
  }
  return 1;
}

static int ConfigStderr(FILE* FileHandle)
{
  printf("Use stderr for error output? ... ");
  fprintf(FileHandle, "\n#define DPCRTLMM_DEV_ERROR ");

  if ( config_stderr ) {
    printf("yes\n");
    fprintf(FileHandle, "stderr /* Use stderr for standard error output */\n");
  }
  else {
    printf("no\n");
    fprintf(FileHandle, "stdout /* Redirect error traffic */\n");
  }
  return 1;
}

static int ConfigDebugHooks(FILE* FileHandle)
{
  size_t sizematrix;

  printf("Add support for advanced debug hooks? ... ");
  fprintf(FileHandle, "\n/*Advanced debug hook support*/\n");

  if ( config_hookchainsize ) { /* Nonzero */
    printf("yes\n");
    fprintf(FileHandle, "#define DPCRTLMM_DEBUGHOOKS\n");
  }
  else { /* Disabled */
    printf("no\n");
    fprintf(FileHandle, "/* #define DPCRTLMM_DEBUGHOOKS */\n");
  }

  /* Pretend void* is a pointer to a function, they will be the same anyway */
  sizematrix = sizeof(void*) * config_hookchainsize * (DPCRTLMM_DEBUGHOOK_LASTHOOK+1);
  printf("Fixed debug hook chain size set as ... %d\n  SIZE OF MATRIX=%u bytes (%u Kb)\n", config_hookchainsize, sizematrix, sizematrix / 1024);
  fprintf(FileHandle, "#define DPCRTLMM_HOOKCHAIN_SIZE (%d)\n", config_hookchainsize);

  if ( config_hookchainsize < 0 ) {
    printf("ERROR: Hook chain size is a negative number!\n");
    return 0;
  }
  return 1;
}

static int ConfigSafetyList(FILE* FileHandle)
{
  size_t minsize, maxsize;

  minsize = sizeof(PS_DPCRTLMM_BLOCKDESCARRAY) * config_maxsafetyentries;
  maxsize = minsize + ( sizeof(S_DPCRTLMM_BLOCKDESCARRAY) * config_maxsafetyentries );

  printf("Fixed safety list size set as ... %d\n  SIZE=%u bytes, %u Kb MIN static, %u bytes, %u Kb MAX static+dynamic.\n", config_maxsafetyentries, minsize, minsize / 1024, maxsize, maxsize / 1024);
  fprintf(FileHandle, "\n#define DPCRTLMM_SAFETYLIST_MAXSIZE (%d)\n", config_maxsafetyentries);
  if ( config_maxsafetyentries < 0 ) {
    printf("ERROR: Safety list size set to negative number!\n");
    return 0;
  }
  else if ( !config_maxsafetyentries ) {
    printf("ERROR: Safety list size may not be zero, source would not presently build, although plans to make it optional are afoot.\n");
    return 0;
  }
  else if ( config_maxsafetyentries < 3 )
    printf("warning: Safety list is small but not too small, ensure this is what you want.\n");
  else if ( config_maxsafetyentries > 2048 )
    printf("warning: Safety list is very large, only recommened in large programs.\n");

  return 1;
}

static int ConfigFarData(FILE* FileHandle)
{
  printf("Use explicit far data? ... ");

  if ( config_wantfardata ) {
    printf("yes\n");
    fprintf(FileHandle, "\n#define DPCRTLMM_WANTFARDATA /* For 16-bit small memory models */\n");
  }
  else {
    printf("no\n");
  }
  return 1;
}

static int ConfigNULLArray(FILE* FileHandle)
{
  printf("Disallow NULL array usage for backward compatibillity? ... ");

  if ( config_nonullarray ) {
    printf("yes, disallowed\n");
    fprintf(FileHandle, "\n#define DPCRTLMM_NONULL_BLOCKDESCARRAY\n");
  }
  else {
    printf("no, allowed\n");
  }
  return 1;
}

static int ConfigLog(FILE* FileHandle)
{
  printf("Write events to DPCRTLMM.LOG? ... ");

  if ( config_log ) {
    printf("yes\n");
    fprintf(FileHandle, "\n#define DPCRTLMM_LOG /* Logging enabled (this will make things slower!). */\n");
  }
  else {
    printf("no\n");
    fprintf(FileHandle, "\n/* DPCRTLMM_LOG support disabled, rebuild to turn it on, things should be faster without it though */\n");
  }
  return 1;
}

static int ConfigMaxPort(FILE* FileHandle)
{
  printf("Use MaxPort to compile at any cost, anywhere? ... ");

  if ( config_maxport ) {
    printf("yes\n");
    fprintf(FileHandle, "\n#define DPCRTLMM_MAXPORT /* Do not use anything which might not run everywhere */\n");
  }
  else {
    printf("no\n");
    fprintf(FileHandle, "\n/*#define DPCRTLMM_MAXPORT*/\n");
  }
  return 1;
}

static int ConfigPrivate(FILE* FileHandle)
{
  printf("Private tree build? ... ");

  if ( config_private ) {
    puts("yes");
    fprintf(FileHandle, "\n#define PRIVATE /* Private tree build */\n");
  }
  else {
    puts("no");
    fprintf(FileHandle, "\n/* Not a private build */\n");
  }
  return 1;
}

static int ConfigSpecial(FILE* FileHandle)
{
  printf("Special build? ... ");

  if ( config_special ) {
    puts("yes");
    fprintf(FileHandle, "\n#define SPECIAL /* Special build */\n");
  }
  else {
    puts("no");
    fprintf(FileHandle, "\n/* Not special but a regualr build */\n");
  }
  return 1;
}

static void Syntax()
{
  printf("\nAutoconfiguration options: (defaults have been omitted)\n"
         "--help = Displays available configuration options\n"
         "--enable-debug = Adds internal library debugging traps and assertions.\n"
         "                 Unfortunately no debugging symbols are added yet.\n"
         "--force-bigendian/--force-littleendian = Not recommend, supresses byte order\n"
         "                                         auto-detection.\n"
         "--enable-threads = Enables thread safe locking support for POSIX or Windows NT.\n"
         "--enable-inline = Only try this if you know your compiler supports C99's new\n"
         "                  \'inline\' keyword.\n"
         "--enable-stderr = Makes errors report on stderr, (default=stdout)\n"
         "--hook-chain-size:nnnn = If not specified defaults to 32 entries per chain,\n"
         "                         zero may be used to disable disable debug hooks.\n"
         "--safety-list-size:nnnn = If not specified defaults to 512, this defines the\n"
         "                          maximum number of block arrays available.\n"
         "--far = Read the manual to understand this, it is useless on all Linux\n"
         "        machines, 32-bit programs and large model DOS 16-bit apps.\n"
         "--disable-null-array = Restores old behaviour before the special NULL array\n"
         "                       existed.\n"
         "--disable-log = Supresses DPCRTLMM.LOG\n"
         "--maxport = Disable anything which might not run on a games console\n"
         "--private = Set the PRIVATE tree flag for subtrees on other projects\n"
         "--special = Set the SPECIAL flag for specialised builds\n"
  );
}
