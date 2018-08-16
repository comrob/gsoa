/*
 * File name: tsplibloader.cc
 * Date:      2006/04/27 13:51
 * Author:    Jan Faigl
 */

#include <crl/tsp/loadertour.h>
#include <crl/tsp/loadertsp.h>

#include <crl/tsp/tsplibloader.h>

using namespace crl::tsplib;

/// --------------------------------------------------------------------------
/// Class CTSPLibLoader
/// --------------------------------------------------------------------------
CTSPLibLoader::CTSPLibLoader()
{
   crl::tsp::CLoaderTSP loader;
   tsplib.registerLoader(loader);
   crl::tsp::CLoaderTour loaderTour;
   tsplib.registerLoader(loaderTour);
}

/// --------------------------------------------------------------------------
CTSPLibLoader::~CTSPLibLoader() {}

/// --------------------------------------------------------------------------
crl::tsp::CTSPLibData *CTSPLibLoader::load(std::string filename)
{
   return tsplib.load(filename);
}

/// --------------------------------------------------------------------------
/// Loader
/// --------------------------------------------------------------------------
CTSPLibLoader crl::tsplib::loader;

/* end of tsplibloader.cc */
