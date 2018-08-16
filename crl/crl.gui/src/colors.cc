/*
 * File name: colors.cc
 * Date:      2006/04/30 16:00
 * Author:    Jan Faigl
 */

#include <crl/gui/colors.h>

using namespace crl::gui;

/// --------------------------------------------------------------------------
CColors Colors::DEFAULT;

/// --------------------------------------------------------------------------
/// Class CSkeleton
/// --------------------------------------------------------------------------
CColors::CColors() : TColors()
{
   i = 0;
   push_back("red");
   push_back("blue");
   push_back("orange");
   push_back("green");
   push_back("seagreen");
   push_back("yellow");
   push_back("moccasin");
   push_back("cyan");
   push_back("RoyalBlue1");
   push_back("SpringGreen2");
   push_back("wheat1");
   push_back("sienna2");
   push_back("magenta");
   push_back("gold");
   // push_back("brown");
}

/// --------------------------------------------------------------------------
CColors::~CColors() {}

/// --------------------------------------------------------------------------
void CColors::reset(void) { i = 0; }

/// --------------------------------------------------------------------------
Color &CColors::cur(void) { return (*this)[i]; }

/// --------------------------------------------------------------------------
Color &CColors::next(void)
{
   Color &c = (*this)[i];
   i = (i + 1) % size();
   return c;
}

/* end of colors.cc */
