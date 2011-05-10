/**********************************************************************
 *                                                                    *
 * "DPCRTLMM" David Palmer's C-RTL Memory Manager Copyright (c) 2000  *
 * David Duncan Ross Palmer, Daybo Logic all rights reserved.         *
 * http://daybologic.com/Dev/dpcrtlmm                                 *
 *                                                                    *
 * D.D.R. Palmer's official homepage: http://daybologic.com/overlord  *
 * See the included license file for more information.                *
 *                                                                    *
 **********************************************************************
*/
// DPCRTLMM 1.0 compatible encapsulation object for C++
// Add this file to your C++ project
// You can have this particular module it's free
// Last modification: 3rd August 2000

// I Appologise for using xalloc, you might want to use std_alloc now because
// that is ANSI/ISO and I'm on Borland C++ 5.02 (1997), which is a year before
// the standard was finally finished, damn they took their time and buggered
// C++ developers didn't they?

#include <stddef.h>
#include <string.h>
#include <stdexcept>
#include <except.h>
#ifdef DPCRTLMM_HDRSTOP // Same as in library source
#  pragma hdrstop
#endif //DPCRTLMM_HDRSTOP

#include "dpcrtlmm.h" // Library header
#include "dpccap.h" // Class definitions

using namespace DPCRTLMM;

TMemManager MemManager;
//---------------------------------------------------------------------------
void* DPCRTLMM::TMemManager::Alloc(const size_t NewBlockSize)
{
  void* ptr = dpcrtlmm_Alloc(NULL, NewBlockSize);
  if (!ptr)
  {
    const std::string s("DPCRTLMM CAP: Failed to allocate block in NULL array");
    throw xalloc((const string&)s, NewBlockSize);
  }
  return ptr;
}
//---------------------------------------------------------------------------
DPCRTLMM::TBlockArray::TBlockArray()
{
  _PblockArray = dpcrtlmm_CreateBlockArray();
  if (!_PblockArray)
  {
    const std::string s("DPCRTLMM CAP: Couldn\'t allocate a block array");
    throw xalloc((const string&)s, sizeof(S_DPCRTLMM_BLOCKDESCARRAY));
  }
}
//---------------------------------------------------------------------------
void* DPCRTLMM::TBlockArray::Alloc(const size_t NewBlockSize)
{
  void* ptr = dpcrtlmm_Alloc(_PblockArray, NewBlockSize);
  if (!ptr)
  {
    const std::string s("DPCRTLMM CAP: Failed to allocate block in dynamic array");
    throw xalloc((const string&)s, NewBlockSize);
  }
  return ptr;
}
//---------------------------------------------------------------------------
