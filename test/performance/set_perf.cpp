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

#include <sbg/pw_inter.hpp>

TEST(SetPerf, Intersection)
{
  int N = 10000;

  SBG::LIB::InterSet is1, is2;
  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    is1.emplace_hint(is1.cend(), i);
  }
  SBG::LIB::Set s1(is1);

  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    is2.emplace_hint(is2.cend(), i);
  }
  SBG::LIB::Set s2(is2);

  auto start = std::chrono::high_resolution_clock::now();
  intersection(s1, s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "INTERSECTION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, Difference)
{
  int N = 10000;

  SBG::LIB::InterSet is1, is2;
  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    is1.emplace_hint(is1.cend(), i);
  }
  SBG::LIB::Set s1(is1);

  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    is2.emplace_hint(is2.cend(), i);
  }
  SBG::LIB::Set s2(is2);

  auto start = std::chrono::high_resolution_clock::now();
  difference(s1, s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "DIFFERENCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, Union)
{
  int N = 10000;

  SBG::LIB::InterSet is1, is2;
  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    is1.emplace_hint(is1.cend(), i);
  }
  SBG::LIB::Set s1(is1);

  for (int j = 0; j < N; j++) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    is2.emplace_hint(is2.cend(), i);
  }
  SBG::LIB::Set s2(is2);

  auto start = std::chrono::high_resolution_clock::now();
  cup(s1, s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}
