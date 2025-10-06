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

#include "eval/user_impl_map.hpp"

namespace Test {

namespace Internal {

using SBG::LIB::Interval;
using SBG::LIB::SetPiece;
using SBG::LIB::Set;
using SBG::LIB::LExp;
using SBG::LIB::Exp;
using SBG::LIB::Map;
using SBG::LIB::PWMap;

TEST(PWMapPerf, Combine)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(2);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    LExp le1(1, k);
    Set s1 = SBG::LIB::SET_FACT.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      Interval i(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      s1.emplaceBack(SetPiece(i));
    }
    pw.emplaceBack(Map(s1, Exp(le1)));
  }

  auto start = std::chrono::high_resolution_clock::now();
  PWMap res = pw.combine(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP COMBINE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, Composition)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(2);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    LExp le1(1, k);
    Set s1 = SBG::LIB::SET_FACT.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      Interval i(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      s1.emplaceBack(SetPiece(i));
    }
    pw.emplaceBack(Map(s1, Exp(le1)));
  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.composition(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP COMPOSITION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, MapInf) {
  int N = 300;
  int sz = 1000;

  SBG::Eval::setSetFactory(2);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (int i = 0; i < N; i += 6) {
    Interval i1(i * sz + 1, 1, (i + 1) * sz);
    Interval i2((i + 1) * sz + 1, 1, (i + 2) * sz);
    Interval i3((i + 2) * sz + 1, 1, (i + 3) * sz);
    Interval i4((i + 3) * sz + 1, 1, (i + 4) * sz);
    Interval i5((i + 4) * sz + 1, 1, (i + 5) * sz);
    Interval i6((i + 5) * sz + 1, 1, (i + 6) * sz);

    LExp le1(1, sz);
    LExp le2(1, sz);
    LExp le3(1, sz);
    LExp le4(1, sz);
    LExp le5(1, sz);
    LExp le6(1, sz);

    Map sm1(i1, le1);
    pw.emplaceBack(sm1);
    Map sm2(i2, le2);
    pw.emplaceBack(sm2);
    Map sm3(i3, le3);
    pw.emplaceBack(sm3);
    Map sm4(i4, le4);
    pw.emplaceBack(sm4);
    Map sm5(i5, le5);
    pw.emplaceBack(sm5);
    Map sm6(i6, le6);
    pw.emplaceBack(sm6);
  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.mapInf();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP MAPINF TEST elapsed time: " << elapsed.count() << "ms\n";
}

} // namespace Internal

} // namespace Test
