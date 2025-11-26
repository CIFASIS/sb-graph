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

TEST(DomOrdPWPerf, OrdDifference)
{
  int N = 10000;

  SBG::Eval::setSetFactory(1);

  Set s1 = SBG::LIB::SET_FACT.createSet();
  Set s2 = SBG::LIB::SET_FACT.createSet();
  for (int j = 0; j < N; ++j) {
    Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.difference(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DIFFERENCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(DomOrdPWPerf, OrdDisjointUnion)
{
  int N = 10000;

  SBG::Eval::setSetFactory(1);

  Set s1 = SBG::LIB::SET_FACT.createSet();
  Set s2 = SBG::LIB::SET_FACT.createSet();
  for (int j = 0; j < N; j+=2) {
    Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
    Interval i2((j+1)*100+1, 1, (j+2)*100);
    s2.emplaceBack(i2);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.disjointCup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DISJOINT UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(DomOrdPWPerf, OrdRestrict)
{
  unsigned int inter_sz = 100;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);

  Set s = SBG::LIB::SET_FACT.createSet();
  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; ++k) {
    LExp le1(1, k);
    Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    Set s1 = SBG::LIB::SET_FACT.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SetPiece mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    if (k > map_sz*25/100 && k%2 == 0 && k < map_sz*75/100) {
      s = s.cup(s1);
    }
    pw.emplaceBack(Map(s1, exp));
  }
  
  auto start = std::chrono::high_resolution_clock::now();
  pw.restrict(s);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED RESTRICT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(DomOrdPWPerf, OrdDom)
{
  SBG::LIB::NAT inter_sz = 100;
  SBG::LIB::NAT set_sz = 10;
  SBG::LIB::NAT map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; ++k) {
    LExp le1(1, k);
    Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::NAT map_offset = k*inter_sz*set_sz;
    Set s = SBG::LIB::SET_FACT.createSet();
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SetPiece mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s.emplaceBack(mdi);
    }
    pw.emplaceBack(Map(s, exp));
  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.dom();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED DOM TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

} // namespace Internal

} // namespace Test
