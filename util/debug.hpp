/** @file debug.hpp

 @brief <b>Debugging</b>

 This debugging routines allow the user to turn on selected
 debugging messages, controllable from the command line arguments
 passed to SBG modules after de -d option.

 <hr>

 This file is part of Set--Based Graph Library.

 SBG Library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Library.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#ifndef SBG_DEBUG
#define SBG_DEBUG

#include <cstddef>
#include <cstdio>
#include <cstring>

namespace SBG {

namespace Util {

/*
 * Print an ERROR message. Then exits with EXIT_FAILURE status.
*/
template<class... Args>
void ERROR(Args... args)
{
  (std::cerr << "ERROR>>" << ... << args);
  exit(EXIT_FAILURE);
}


/**
 * Print an ERROR message if condition is false.
 * Then exits with EXIT_FAILURE status.
 */
template<class... Args>
void ERROR_UNLESS(bool cond, Args... args)
{
  if (!cond) {
    (std::cerr << "ERROR>>" << ... << args);
    exit(EXIT_FAILURE);
  }
}

} // namespace Util

} // namespace SBG

#endif
