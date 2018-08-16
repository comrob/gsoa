/*
 * File name: ws.cc
 * Date:      2006/04/27 22:33
 * Author:    Jan Faigl
 */

#include <crl/ws.h>

using namespace crl;

/// --------------------------------------------------------------------------
// Class CWS
/// ---------------------------------------------------------------------------
const int CWS::NUMBER = 256;
bool CWS::initVal[256];
bool CWS::initialized = false;
/// ---------------------------------------------------------------------------
CWS::CWS()
{
   if (!initialized) {
      initialized = true;
      set(0, 255, false, initVal);
      initVal[(int)' '] = true;
      initVal[(int)'\t'] = true;
   }
   init();
}

/// --------------------------------------------------------------------------
CWS::~CWS() {}

/// --------------------------------------------------------------------------
CWS *CWS::clone(void)
{
   CWS *ret = new CWS();
   for (int i = 0; i < 256; i++) {
      ret->local[i] = local[i];
   }
   return ret;
}

/// --------------------------------------------------------------------------
bool &CWS::operator[](unsigned char c) { return local[c]; }

/// --------------------------------------------------------------------------
void CWS::clear(void)
{
   for (int i = 0; i < 256; i++) {
      local[i] = false;
   }
}

/// --------------------------------------------------------------------------
void CWS::init(void)
{
   for (int i = 0; i < 256; i++) {
      local[i] = initVal[i];
   }
}

/// --------------------------------------------------------------------------
void CWS::set(int from, int to, bool val, bool array[256])
{
   if (from >= 0 && from < 256 && to >= from && to < 256) {
      for (int i = from; i <= to; i++) {
         array[i] = val;
      }
   } // end if
}

/// --------------------------------------------------------------------------
/// General WaitSpace
/// --------------------------------------------------------------------------
CWS crl::WhiteSpace;

/* end of ws.cc */
