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

/**
 * @class DomOrdPWPerf
 * @brief Test suite created to analyze the time growth of the different domain
 * ordered PWs operations.
 */
class DomOrdPWPerf : public ::testing::TestWithParam<SBG::LIB::NAT> {
  protected:
  std::pair<PWMap, PWMap> contiguousMaps(SBG::LIB::NAT map_sz) {
    SBG::LIB::NAT inter_sz = 100;
    SBG::LIB::NAT set_sz = 100;

    Interval second_dim(0, 1, inter_sz - 1);
    PWMap pw1 = SBG::LIB::PW_FACT.createPWMap();
    PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
    for (unsigned int j = 0; j < map_sz; ++j) {
      Set dom1 = SBG::LIB::SET_FACT.createSet();
      Set dom2 = SBG::LIB::SET_FACT.createSet();
      SBG::LIB::NAT off = j*set_sz*inter_sz;
      for (unsigned int h = 0; h < set_sz; ++h) {
        Interval i1(off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1);
        SetPiece mdi1;
        mdi1.emplaceBack(i1);
        mdi1.emplaceBack(second_dim);
        dom1.emplaceBack(mdi1); 

        SBG::LIB::NAT off2 = off + inter_sz/2;
        Interval i2(off2 + (h*inter_sz), 1, off2 + (h + 1)*inter_sz - 1);
        SetPiece mdi2;
        mdi2.emplaceBack(i2);
        mdi2.emplaceBack(second_dim);
        dom2.emplaceBack(mdi2); 
      }

      LExp id;
      Exp multidim_id(1, id);

      pw1.emplaceBack(Map(dom1, multidim_id));
      pw2.emplaceBack(Map(dom2, multidim_id));
    }

    return {pw1, pw2};
  }
};

TEST_P(DomOrdPWPerf, DomOrdEq)
{
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);

  SBG::LIB::NAT map_sz = GetParam();
  bool result = false;

  auto [pw1, pw2] = contiguousMaps(map_sz);
  auto start = std::chrono::high_resolution_clock::now();
  result = pw1 == pw2;
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
    end - start);
  std::cout << elapsed.count() << "ms\n";

  EXPECT_EQ(result, false);
}

TEST_P(DomOrdPWPerf, DomOrdPlus)
{
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);

  SBG::LIB::NAT map_sz = GetParam();
  auto [pw1, pw2] = contiguousMaps(map_sz);

  auto start = std::chrono::high_resolution_clock::now();
  pw1 + pw2;
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
    end - start);
  std::cout << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST_P(DomOrdPWPerf, DomOrdDom)
{
  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);

  SBG::LIB::NAT map_sz = GetParam();
  auto [pw1, pw2] = contiguousMaps(map_sz);

  auto start = std::chrono::high_resolution_clock::now();
  pw1.dom();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
    end - start);
  std::cout << elapsed.count() << "ms\n";

  SUCCEED();
}

INSTANTIATE_TEST_SUITE_P(
  DomOrdPWGrowth,
  DomOrdPWPerf,
  ::testing::Values(1000, 10000, 100000, 1000000)
);

TEST(DomOrdPWPerfOld, DomOrdRestrict)
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

// image

// preImage

// inverse

// composition

// mapInf

// fixedPoints

// concatenation

TEST(DomOrdPWPerfOld, DomOrdCombine)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz * 25 / 100; k < map_sz * 75 / 100; ++k) {
    LExp le1(1, k);
    Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    Set s1 = SBG::LIB::SET_FACT.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(Map(s1, exp));

  }
  
  for (unsigned int k = 0; k < map_sz; ++k) {
    LExp le1(1, k);
    Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    Set s1 = SBG::LIB::SET_FACT.createSet();
    int map_offset = k * inter_sz * set_sz;
    map_offset += map_offset/2;
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw2.emplaceBack(Map(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.combine(pw2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED COMBINE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

// reduce

// minAdj

TEST(DomOrdPWPerfOld, DomOrdFirstInvSet)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

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
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    if(k > map_sz * 25 / 100 && k%2==0 && k < map_sz * 75 / 100 )
        s=s.cup(s1);
    pw.emplaceBack(Map(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.firstInv(s);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED FIRSTINV WITH SET TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

// equalImage

TEST(DomOrdPWPerfOld, DomOrdLessImage)
{
  unsigned int inter_sz = 100;
  unsigned int set_sz = 10;
  unsigned int map_sz = 10000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(2);

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
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(Map(s1, exp));

  }

  PWMap sum = pw+pw;
  auto start = std::chrono::high_resolution_clock::now();
  pw.lessImage(sum);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED MINMAP TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

// sharedImage

// offsetDom

// compact

} // namespace Internal

} // namespace Test
