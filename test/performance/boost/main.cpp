/** @file main.cpp

 @brief <b>Boost matching profiling</b>

 This file is meant to test the execution time of scalar algorithms for graphs
 from the BGL library.

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

#include "test/performance/boost/boost_exec.hpp"

int main(int argc, char**argv)
{
  Boost::BoostExecutor boost_exec;
  boost_exec.execute(argc, argv);

  return 0;
}
