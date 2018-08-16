/*
 * File name: route_path_utils.h
 * Date:      2016/12/10 18:12
 * Author:    Jan Faigl
 */

#ifndef __ROUTE_PATH_UTILS_H__
#define __ROUTE_PATH_UTILS_H__

#include "coords.h"

/// ----------------------------------------------------------------------------
/// @brief get_path_length
/// 
/// @param pts 
/// 
/// @return 
/// ----------------------------------------------------------------------------
double get_path_length(const CoordsVector &pts, bool closed = true);

/// ----------------------------------------------------------------------------
/// @brief two_opt
/// 
/// @param path 
/// ----------------------------------------------------------------------------
void two_opt(CoordsVector &path);

#endif

/* end of route_path_utils.h */
