/*
 * File name: tgsoa-cetsp.cc
 * Date:      2016/12/07 08:33
 * Author:    Jan Faigl
 */

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <crl/config.h>
#include <crl/logging.h>
#include <crl/perf_timer.h>
#include <crl/boost_args_config.h>

#include <crl/gui/guifactory.h>
#include <crl/gui/win_adjust_size.h>

#include "gsoa.h"

using crl::logger;
using namespace gsoa;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

const std::string GSOA_VERSION = "0.4";

typedef crl::gui::CCanvasBase Canvas;

/// ----------------------------------------------------------------------------
/// Program options variables
/// ----------------------------------------------------------------------------
std::string guiType = "none";

crl::CConfig guiConfig;
crl::CConfig gsoaConfig;
std::string canvasOutput = "";
std::string solutionFile = "";

/// ----------------------------------------------------------------------------
/// Global variable
/// ----------------------------------------------------------------------------
crl::gui::CGui *g = 0;
#define GUI(x)  if(gui) { x;}

/// ----------------------------------------------------------------------------
bool parseArgs(int argc, char *argv[]) 
{
   bool ret = true;
   std::string configFile;
   std::string guiConfigFile;
   std::string loggerCfg = "";

   po::options_description desc("General options");
   desc.add_options()
      ("help,h", "produce help message")
      ("config,c", po::value<std::string>(&configFile)->default_value(std::string(argv[0]) + ".cfg"),
       "configuration file")
      ("logger-config,l", po::value<std::string>(&loggerCfg)->default_value(loggerCfg),
       "logger configuration file")
      ("config-gui", po::value<std::string>(&guiConfigFile)->default_value(std::string(argv[0]) + "-gui.cfg"),
       "dedicated gui configuration file")
      ("solution-file", po::value<std::string>(&solutionFile)->default_value(""))
      ;
   try {
      po::options_description guiOptions("Gui options");
      crl::gui::CGuiFactory::getConfig(guiConfig);
      crl::gui::CWinAdjustSize::getConfig(guiConfig);
      guiConfig.add<double>("gui-add-x",
	    "add the given value to the loaded goals x coord to determine the canvas size and transformation",
	    0);
      guiConfig.add<double>("gui-add-y",
	    "add the given value to the loaded goals y coord to determine the canvas size and transformation",
	    0);
      boost_args_add_options(guiConfig, "", guiOptions);
      guiOptions.add_options()
	 ("canvas-output", po::value<std::string>(&canvasOutput), "result canvas outputfile");

      po::options_description gsoaOptions("GSOA options");
      boost_args_add_options(CGSOA::getConfig(gsoaConfig), "", gsoaOptions);

      po::options_description cmdline_options;
      cmdline_options.add(desc).add(guiOptions).add(gsoaOptions);

      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, cmdline_options), vm);
      po::notify(vm);

      std::ifstream ifs(configFile.c_str());
      store(parse_config_file(ifs, cmdline_options), vm);
      po::notify(vm);
      ifs.close();
      ifs.open(guiConfigFile.c_str());
      store(parse_config_file(ifs, cmdline_options), vm);
      po::notify(vm);
      ifs.close();

      if (vm.count("help")) {
	 std::cerr << std::endl;
	 std::cerr << "GSOA solver ver. " << GSOA_VERSION << std::endl;
	 std::cerr << cmdline_options << std::endl;
	 ret = false;
      }
      if (
	    ret &&
	    loggerCfg != "" &&
	    fs::exists(fs::path(loggerCfg))
	 ) {
	 crl::initLogger("gsoa", loggerCfg.c_str());
      } else {
	 crl::initLogger("gsoa");
      }
      const std::string problemFile = gsoaConfig.get<std::string>("problem");
      if (!fs::exists(fs::path(problemFile))) {
	 ERROR("Problem file '" + problemFile + "' does not exists");
	 ret = false;
      }
   } catch (std::exception &e) {
      std::cerr << std::endl;
      std::cerr << "Error in parsing arguments: " << e.what() << std::endl;
      ret = false;
   }
   return ret;
}

/// - main ---------------------------------------------------------------------
int main(int argc, char *argv[]) 
{
   Canvas *canvas = 0;
   int ret = -1;
   if (parseArgs(argc, argv)) {
      INFO("Start Logging");
      try {
	 CoordsVector pts;
	 if (gsoaConfig.get<bool>("variable-radius")) {
	    double x, y, r;
	    std::ifstream in(gsoaConfig.get<std::string>("problem").c_str());
	    while (in >> x >> y >> r) {
	       pts.push_back(Coords(x - r, y - r));
	       pts.push_back(Coords(x + r, y + r));
	    }
	 } else {
	    crl::CPerfTimer t("Load problem time real:");
	    double r = gsoaConfig.get<double>("communication-radius");
	    double x, y;
	    std::ifstream in(gsoaConfig.get<std::string>("problem").c_str());
	    while (in >> x >> y) {
	       pts.push_back(Coords(x - r, y - r));
	       pts.push_back(Coords(x + r, y + r));
	    }
	 }
	 crl::gui::CWinAdjustSize::adjust(pts, guiConfig);
	 if ((g = crl::gui::CGuiFactory::createGui(guiConfig)) != 0) {
	    INFO("Start gui " + guiConfig.get<std::string>("gui"));
	    canvas = new Canvas(*g);
	 }
	 CGSOA gsoa(gsoaConfig);
	 gsoa.setCanvas(canvas);
	 { 
	    crl::CPerfTimer t("Total solve time: ");
	    if (solutionFile.empty()) {
	       gsoa.solve();
	    } else {
	       CoordsVector pts;
	       Coords pt;
	       std::ifstream in(solutionFile.c_str());
	       while (in >> pt.x >> pt.y) {
		  pts.push_back(pt);
	       }
	       gsoa.visualize(pts);
	    }
	 }
	 INFO("End Logging");
	 if (canvas) {
	    if (canvasOutput.size()) {
	       canvas->save(canvasOutput);
	    }
	    if (!guiConfig.get<bool>("nowait")) {
	       INFO("click to exit");
	       canvas->click();
	    }
	    delete canvas;
	    delete g;
	 }
      } catch (crl::exception &e) {
	 ERROR("Exception " << e.what() << "!");
      } catch (std::exception &e) {
	 ERROR("Runtime error " << e.what() << "!");
      }
      ret = EXIT_SUCCESS;
   }
   crl::shutdownLogger();
   return ret;
}

/* end of tgsoa-cetsp.cc */
