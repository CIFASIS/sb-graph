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

TEST(PWMapPerf, Composition)
{
  unsigned int inter_sz = 100;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::LIB::MapSet ms1;
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::InterSet is1;
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      is1.emplace_hint(is1.cend(), i);
    }
    SBG::LIB::Set s1(is1);
    ms1.emplace(SBG::LIB::SBGMap(s1, le1));
  }
  SBG::LIB::PWMap pw1(ms1);

  auto start = std::chrono::high_resolution_clock::now();
  SBG::LIB::PWMap res = composition(pw1, pw1);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP COMPOSITION TEST elapsed time: " << elapsed.count() << "ms\n";

  //std::cout << pw1 << "\n";
  //std::cout << res << "\n";

  SUCCEED();
}

TEST(PWMapPerf, MapInf) {
  int N = 600;
  int sz = 10;

  SBG::LIB::MapSet aux;
  for (int i = 0; i < N; i += 6) {
    SBG::LIB::Interval i1(i * sz + 1, 1, (i + 1) * sz);
    SBG::LIB::Interval i2((i + 1) * sz + 1, 1, (i + 2) * sz);
    SBG::LIB::Interval i3((i + 2) * sz + 1, 1, (i + 3) * sz);
    SBG::LIB::Interval i4((i + 3) * sz + 1, 1, (i + 4) * sz);
    SBG::LIB::Interval i5((i + 4) * sz + 1, 1, (i + 5) * sz);
    SBG::LIB::Interval i6((i + 5) * sz + 1, 1, (i + 6) * sz);

    SBG::LIB::Set s1(i1);
    SBG::LIB::Set s2(i2);
    SBG::LIB::Set s3(i3);
    SBG::LIB::Set s4(i4);
    SBG::LIB::Set s5(i5);
    SBG::LIB::Set s6(i6);

    SBG::LIB::LExp le1(1, sz);
    SBG::LIB::LExp le2(1, sz);
    SBG::LIB::LExp le3(1, sz);
    SBG::LIB::LExp le4(1, sz);
    SBG::LIB::LExp le5(1, sz);
    SBG::LIB::LExp le6(1, sz);

    SBG::LIB::SBGMap sm1(s1, le1);
    aux.emplace(sm1);
    SBG::LIB::SBGMap sm2(s2, le2);
    aux.emplace(sm2);
    SBG::LIB::SBGMap sm3(s3, le3);
    aux.emplace(sm3);
    SBG::LIB::SBGMap sm4(s4, le4);
    aux.emplace(sm4);
    SBG::LIB::SBGMap sm5(s5, le5);
    aux.emplace(sm5);
    SBG::LIB::SBGMap sm6(s6, le6);
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
