/*
 * File name: boost_args_config.h
 * Date:      2009/01/24 14:30
 * Author:    Jan Faigl
 */

/**
 * helper function to map crl::CConfig from config.h
 * to boost::program_options,
 * as common should not be dependent on boost, path to
 * appropriate boost headers and libraries must be set
 * elsewhere. As defined functions are used in context of boost::program_options
 * it must be set anyway for main program.
 */

#ifndef __BOOST_ARGS_CONFIG_H__
#define __BOOST_ARGS_CONFIG_H__

#include <boost/program_options.hpp>

#include <crl/config.h>

namespace po = boost::program_options;

namespace crl {

/// NOTE binding to boost cause that isSet method has not correct value
/// TODO bind through variables_map and count method to update isSet

/// ----------------------------------------------------------------------------
/// @brief boost_args_add_option bind config("name") of type T to flag program
///        options. if config parameter name has default value it used used
///	      as well as associated description is pass to boost::program_options
///
///          NOTE binding to boost cause that isSet method has not correct value
/// @param config
/// @param name
/// @param flag
/// @param desc
/// ----------------------------------------------------------------------------
template <class T>
inline void boost_args_add_option(crl::CConfig &config, std::string name,
                                  std::string flag,
                                  po::options_description &desc)
{
   if (config.hasDefaultValue<T>(name)) {
      desc.add_options()(flag.c_str(),
                         po::value<T>(config.getPtr<T>(name))
                             ->default_value(config.getDefaultValue<T>(name)),
                         config.getDescription<T>(name).c_str());
   } else {
      desc.add_options()(flag.c_str(), po::value<T>(config.getPtr<T>(name)),
                         config.getDescription<T>(name).c_str());
   }
}

/// ----------------------------------------------------------------------------
/// @brief boost_args_add_options bind all config parameters of type T
///		to boost::program_options, argument name is constructed from
///		parameter name and prefix (prefix + name), so prefix should
///		contain '-' letter to distinguish particular module
///          NOTE binding to boost cause that isSet method has not correct value
///
/// @param config
/// @param prefix
/// @param desc
/// ----------------------------------------------------------------------------
template <class T>
inline void boost_args_add_options(crl::CConfig &config, std::string prefix,
                                   po::options_description &desc)
{
   crl::CConfig::NameList lst;
   config.getNames<T>(lst);
   for (crl::CConfig::NameList::iterator it = lst.begin(); it != lst.end();
        it++) {
      boost_args_add_option<T>(config, *it, prefix + *it, desc);
   }
}

/// ----------------------------------------------------------------------------
/// @brief boost_args_add_options bind all config parameters to
/// boost::program_options
///		to boost::program_options, argument name is constructed from
///		parameter name and prefix (prefix + name), so prefix should
///		contain '-' letter to distinguish particular module.
///		e.g. let have algorithm CSolver named 'solver' with several options
///		1. At first get config from solver CSolver::getConfig(cfg);
///		2. Create options_description boost::program_options::options_description
///desc("solver");
///		3. Bind all parameters to arguments by calling
///			boost_args_add_options(cfg, "sovler-", desc);
///		Alternatively call by two lines
///				po::options_description solverDesc("solver");
///			cmd_options.add(boost_args_add_options(
///				CSolver::getConfig(solverCfg),
///				"solver-",
///                          solverDesc
///				)
///				);
///
///          NOTE due to binding isSet method has not correct value
/// @param config
/// @param desc
/// ----------------------------------------------------------------------------
inline po::options_description &
boost_args_add_options(crl::CConfig &config, std::string prefix,
                       po::options_description &desc)
{
   /* boost_args_add_options<std::string>(config, prefix, desc);
      boost_args_add_options<int>(config, prefix, desc);
      boost_args_add_options<double>(config, prefix, desc);
      boost_args_add_options<float>(config, prefix, desc);
      */
   for (CConfig::item_iterator it = config.items_begin();
        it != config.items_end(); it++) {
      switch (it->type) {
      case CConfig::STRING_CRL:
         boost_args_add_option<std::string>(config, it->name, prefix + it->name,
                                            desc);
         break;
      case CConfig::INT_CRL:
         boost_args_add_option<int>(config, it->name, prefix + it->name, desc);
         break;
      case CConfig::DOUBLE_CRL:
         boost_args_add_option<double>(config, it->name, prefix + it->name,
                                       desc);
         break;
      case CConfig::FLOAT_CRL:
         boost_args_add_option<float>(config, it->name, prefix + it->name,
                                      desc);
         break;
      case CConfig::BOOL_CRL:
         boost_args_add_option<bool>(config, it->name, prefix + it->name, desc);
         break;
      default:
         break;
      } // end switch
   }
   return desc;
}

} // end namespace crl

#endif

/* end of boost_args_config.h */
