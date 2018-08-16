/*
 * File name: tloader.cc
 * Date:      2006/04/27 15:09
 * Author:    Jan Faigl
 */

#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include <crl/bbox.h>
#include <crl/boost_args_config.h>
#include <crl/config.h>
#include <crl/exceptions.h>
#include <crl/logging.h>
#include <crl/perf_timer.h>
#include <crl/timerN.h>

#include <crl/gui/canvas.h>
#include <crl/gui/colors.h>
#include <crl/gui/guifactory.h>
#include <crl/gui/mouseclick.h>
#include <crl/gui/shapes.h>
#include <crl/gui/win_adjust_size.h>

#include <crl/tsp/tsplibloader.h>
#include <crl/tsp/tsplibtypes.h>

using crl::logger;
using crl::tsplib::loader;

using crl::logger;
using namespace crl::gui;
using namespace crl::tsp;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

typedef crl::gui::CCanvasBase Canvas;
typedef std::vector<int> IntVector;
typedef std::vector<IntVector> Routes;
typedef TSPProblem::Node Node;
typedef std::vector<Node> NodeVector;

#define foreach BOOST_FOREACH

const std::string TSPLIB_LOADER_VERSION = "0.9";

/// ----------------------------------------------------------------------------
/// Program options variables
/// ----------------------------------------------------------------------------
crl::CConfig guiConfig;
std::string tspFile;
std::string citiesFile;
std::string routesFile;
std::string routeFile;
std::string cityShape;
std::string canvasOutput;
int test;

/// ----------------------------------------------------------------------------
bool parseArgs(int argc, char *argv[])
{
   bool ret = true;
   std::string configFile;
   std::string loggerCfg = "";
   std::string guiConfigFile;

   po::options_description desc("General options");
   desc.add_options()("help,h", "produce help message")(
       "logger-config",
       po::value<std::string>(&loggerCfg)->default_value(loggerCfg),
       "logger configuration file")(
       "config,c",
       po::value<std::string>(&configFile)
           ->default_value(std::string(argv[0]) + ".cfg"),
       "configuration file")(
       "config-gui",
       po::value<std::string>(&guiConfigFile)
           ->default_value(std::string(argv[0]) + "-gui.cfg"),
       "dedicated gui configuration file");

   po::options_description guiOptions("Gui options");
   crl::gui::CGuiFactory::getConfig(guiConfig);
   crl::gui::CWinAdjustSize::getConfig(guiConfig);
   boost_args_add_options(guiConfig, "", guiOptions);
   guiOptions.add_options()("result-canvas-output",
                            po::value<std::string>(&canvasOutput),
                            "result canvas outputfile")(
       "city-shape", po::value<std::string>(&cityShape), "city shape");

   po::options_description program("Program options");
   program.add_options()("tsp", po::value<std::string>(&tspFile),
                         "tsplib file")(
       "cities", po::value<std::string>(&citiesFile),
       "cities filename is as output for tsp->cities conversion of as input if "
       "tsp is not given")("routes", po::value<std::string>(&routesFile),
                           "routes filename, set of routes, depot is first, if "
                           "route is closed, depot must also be last node in "
                           "the route")(
       "route", po::value<std::string>(&routeFile),
       "route filename, route as sequnace of cities indexes, the route is "
       "close, so the last city is connected to the first one")(
       "test", po::value<int>(&test)->default_value(0),
       "test \n0 - convert tsp->cities\n1 - show depot\n2 - show routes\n3 - "
       "show route (close route to form tour)");

   try {
      po::options_description cmdline_options;
      cmdline_options.add(desc).add(guiOptions).add(program);

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
         std::cerr << "Simple tsplib loader program ver. "
                   << TSPLIB_LOADER_VERSION << std::endl;
         std::cerr << cmdline_options << std::endl;
         ret = false;
      }
      if (ret && loggerCfg != "" && fs::exists(fs::path(loggerCfg))) {
         crl::initLogger("tload", loggerCfg.c_str());
      } else {
         crl::initLogger("tload");
      }
   }
   catch (std::exception &e) {
      std::cerr << std::endl;
      std::cerr << "Error in parsing arguments: " << e.what() << std::endl;
      ret = false;
   }
   return ret;
}

/// ----------------------------------------------------------------------------
Routes &loadRoutes(const std::string &filename, Routes &routes)
{
   routes.clear();
   std::ifstream in(filename.c_str());
   std::string line;
   int t;
   while (in >> line) {
      crl::assert_argument(line == "[Route]", "[Route] tag is expected");
      routes.push_back(IntVector());
      while (in >> t) {
         routes.back().push_back(t);
      }
      in.clear();
   }
   return routes;
}

/// ----------------------------------------------------------------------------
NodeVector &loadCities(const std::string &filename, NodeVector &cities)
{
   Node ct;
   std::ifstream in(filename.c_str());
   cities.clear();
   while (in >> ct.x >> ct.y) {
      cities.push_back(ct);
   }
   return cities;
}

/// ----------------------------------------------------------------------------
int getClosests(Node &pt, NodeVector &points)
{
   int ret = -1;
   double min = std::numeric_limits<double>::max();
   int i = 0;
   foreach (Node &t, points) {
      const double d = pt.distanceNoSqrt(t);
      if (d < min) {
         min = d;
         ret = i;
      }
      i += 1;
   } // end foreach loop
   return ret;
}

/// ----------------------------------------------------------------------------
// Main
/// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   crl::gui::CGui *gui = 0;
   Canvas *canvas = 0;
   int ret = -1;
   if (parseArgs(argc, argv)) {
      INFO("Start Logging");
      try {
         NodeVector cities;
         if (!tspFile.empty()) {
            crl::CPerfTimer pft("Load tsplib problem ");
            TSPProblem *data = dynamic_cast<TSPProblem *>(loader.load(tspFile));
            pft.stop();
            crl::assert_argument(data, "TSP problem '" + tspFile +
                                           "' has not been loaded");
            foreach (const Node &node, data->getNodes()) {
               cities.push_back(node);
            }
            delete data;
         }
         if (cities.empty() && !citiesFile.empty()) {
            crl::CPerfTimer pft("Load cities ");
            loadCities(citiesFile, cities);
         }
         crl::assert_argument(!cities.empty(), "Number of cities is 0");

         crl::gui::CWinAdjustSize::adjust(cities, guiConfig);

         if ((gui = crl::gui::CGuiFactory::createGui(guiConfig)) != 0) {
            INFO("Start gui " + guiConfig.get<std::string>("gui"));
            canvas = new Canvas(*gui);
         }

         if (canvas) {
            CShape sh = Shape::CITY;
            if (!cityShape.empty()) {
               sh.setShape(cityShape);
            }
            *canvas << canvas::AREA;
            foreach (const Node &node, cities) {
               *canvas << node.x << node.y;
            }
            *canvas << canvas::END;
            *canvas << sh << canvas::POINT;
            foreach (const Node &node, cities) {
               *canvas << node.x << node.y;
            }
         }

         switch (test) {
         case 0:
            INFO("Convert tsp lib nodes to cities");
            if (!citiesFile.empty()) {
               std::fstream out;
               out.open(citiesFile.c_str(), std::ios::out);
               foreach (const Node &node, cities) {
                  out << node.x << " " << node.y << std::endl;
               }
               crl::assert_argument(not out.fail(), "Can not save cities '" + citiesFile +
                                             "' file");
               out.close();
            }
            break;
         case 1: {
            INFO("Depot Selection");
            crl::CBBox bbox;
            foreach (const Node &ct, cities) {
               bbox.add(ct.x, ct.y);
            }
            Node pt(bbox.minx() + bbox.width() / 2.,
                    bbox.miny() + bbox.height() / 2.);
            int closest = getClosests(pt, cities);
            crl::assert_argument(closest >= 0,
                                 "The Closest node has not been found");
            INFO("Closet city to the center is: " << closest);
            if (canvas) {
               CMouseClick click(*gui);
               do {
                  crl::assert_argument(closest >= 0 && closest < cities.size(),
                                       "Closest index is out of range");
                  *canvas << canvas::CLEAR << "closest"
                          << "closest" << Shape::GOLD_POINT << canvas::POINT
                          << cities[closest].x << cities[closest].y;
                  INFO("Current city index: " << closest);
                  *canvas << canvas::REDRAW;
                  click.getMouseClick(pt.x, pt.y);
                  closest = getClosests(pt, cities);
               } while (!canvas->getRenderer()->shutdownRequest());
            } // end if canvas
         } break;
         case 2:
            INFO("Routes visualization");
            if (!routesFile.empty()) {
               Routes routes;
               loadRoutes(routesFile, routes);
               INFO("Number loaded routes: " << routes.size());
               if (canvas) {
                  CColors colors;
                  *canvas << Shape::GREEN_LINE << canvas::LINESTRING << Width(4)
                          << Fill(false);
                  *canvas << "route";
                  foreach (const IntVector &route, routes) {
                     *canvas << PenColor(colors.next());
                     crl::assert_argument(!route.empty(),
                                          "Empty route detected");
                     foreach (int i, route) {
                        crl::assert_argument(i >= 0 && i < cities.size(),
                                             "Route index is out of range");
                        *canvas << cities[i].x << cities[i].y;
                     }
                     *canvas << canvas::END;
                  }
               }
	    } else {
               WARN("Can not show routes, routes file has not been given");
            }
            break;
         case 3:
            INFO("Route visualization");
            if (!routeFile.empty()) {
               Routes routes;
               loadRoutes(routeFile, routes);
               crl::assert_argument(routes.size() == 1,
                                    "Route file must contain exacly one route");
               crl::assert_argument(routes.front().size() == cities.size(),
                                    "Route does not cover all cities");
               if (canvas) {
                  *canvas << Shape::GREEN_LINE << canvas::LINESTRING << Width(4)
                          << Fill(false);
                  *canvas << "route";
                  foreach (int i, routes.front()) {
                     crl::assert_argument(i >= 0 && i < cities.size(),
                                          "Route index is out of range");
                     *canvas << cities[i].x << cities[i].y;
                  }
                  *canvas << cities[routes.front().front()].x
                          << cities[routes.front().front()].y;
                  *canvas << canvas::END;
               }
	    } else {
               WARN("Can not show route, route file has not been given");
            }
            break;
         default:
            ERROR("Unknown test");
            break;
         }
         /*
          *canvas << Shape::MAP_BORDER << canvas::POLYGON <<
          *loader::Border(outLoader) << canvas::END; canvas <<
          *Shape::MAP_OBSTACLE << canvas::POLYGON <<
          *loader::Obstacles(outLoader) << canvas::END; canvas <<
          *Shape::RED_BOLD_LINE << canvas::POLYGON << Fill(false) <<
          *loader::Border(inLoader) << canvas::END; canvas <<
          *loader::Obstacles(inLoader);
          */
         if (canvas) {
            if (canvasOutput.size() > 0) {
               canvas->save(canvasOutput.c_str());
            }
            if (!guiConfig.get<bool>("nowait") && canvas) {
               INFO("click to exit");
               canvas->click();
            }
            gui->stop();
            delete canvas;
            delete gui;
         } // end gui
         ret = EXIT_SUCCESS;
      }
      catch (crl::exception &e) {
         ERROR("Exception " << e.what() << "!");
      }
      catch (std::exception &e) {
         ERROR("Runtime error " << e.what() << "!");
      }
      INFO("End Logging");
   }
   return ret;
}

/* end of tloader.cc */
