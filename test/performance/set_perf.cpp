/*****************************************************************************

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

#include <chrono>
#include <gtest/gtest.h>

#include "sbg/ord_pw_mdinter.hpp"

TEST(SetPerf, Intersection)
{
  int N = 3;

  SBG::LIB::OrdSet s1, s2;
  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.intersection(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "INTERSECTION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, Difference)
{
  int N = 3;

  SBG::LIB::OrdSet s1, s2;
  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.difference(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "DIFFERENCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, Union)
{
  int N = 3;

  SBG::LIB::OrdSet s1, s2;
  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*95+1, 1, (j+1)*95);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.cup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}
