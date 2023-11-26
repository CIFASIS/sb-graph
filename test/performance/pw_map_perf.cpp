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

#include "sbg/pw_map.hpp"

/*
TEST(PWMapPerf, Composition)
{
  unsigned int inter_sz = 5;
  unsigned int set_sz = 5;
  unsigned int map_sz = 5;

  SBG::LIB::CanonPWMap ms1;
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::OrdSet s1;
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      s1.emplace_hint(s1.end(), i);
    }
    ms1.emplace(SBG::LIB::CanonMap(s1, le1));
  }
  SBG::LIB::PWMap pw1(ms1);

  auto start = std::chrono::high_resolution_clock::now();
  SBG::LIB::PWMap res = composition(pw1, pw1);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP COMPOSITION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, MapInf) {
  int N = 600;
  int sz = 10;

  SBG::LIB::CanonPWMap aux;
  for (int i = 0; i < N; i += 6) {
    SBG::LIB::Interval i1(i * sz + 1, 1, (i + 1) * sz);
    SBG::LIB::Interval i2((i + 1) * sz + 1, 1, (i + 2) * sz);
    SBG::LIB::Interval i3((i + 2) * sz + 1, 1, (i + 3) * sz);
    SBG::LIB::Interval i4((i + 3) * sz + 1, 1, (i + 4) * sz);
    SBG::LIB::Interval i5((i + 4) * sz + 1, 1, (i + 5) * sz);
    SBG::LIB::Interval i6((i + 5) * sz + 1, 1, (i + 6) * sz);

    SBG::LIB::LExp le1(1, sz);
    SBG::LIB::LExp le2(1, sz);
    SBG::LIB::LExp le3(1, sz);
    SBG::LIB::LExp le4(1, sz);
    SBG::LIB::LExp le5(1, sz);
    SBG::LIB::LExp le6(1, sz);

    SBG::LIB::CanonMap sm1(i1, le1);
    aux.emplace(sm1);
    SBG::LIB::CanonMap sm2(i2, le2);
    aux.emplace(sm2);
    SBG::LIB::CanonMap sm3(i3, le3);
    aux.emplace(sm3);
    SBG::LIB::CanonMap sm4(i4, le4);
    aux.emplace(sm4);
    SBG::LIB::CanonMap sm5(i5, le5);
    aux.emplace(sm5);
    SBG::LIB::CanonMap sm6(i6, le6);
    aux.emplace(sm6);
  }
  SBG::LIB::PWMap pw(aux);

  auto start = std::chrono::high_resolution_clock::now();
  SBG::LIB::PWMap res = mapInf(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP MAPINF TEST elapsed time: " << elapsed.count() << "ms\n";

  std::cout << pw << "\n";
  std::cout << res << "\n";
}
*/
