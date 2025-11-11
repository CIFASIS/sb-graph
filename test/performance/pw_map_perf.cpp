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

TEST(PWMapPerf, UnordCombine)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz * 25 / 100; k < map_sz * 75 / 100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  std::cout << "PWL MAP UNORDERED COMBINE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, OrdCombine)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz * 25 / 100; k < map_sz * 75 / 100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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

TEST(PWMapPerf, UnordConcatenation)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  Set s = SBG::LIB::SET_FACT.createSet();
  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz*2; k++) {
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
    if(k%2==0)
      pw.emplaceBack(Map(s1, exp));
    else
      pw2.emplaceBack(Map(s1, exp));

  }

  
  auto start = std::chrono::high_resolution_clock::now();
  pw.concatenation(pw2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED CONCATENATION WITH SET TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, OrdConcatenation)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  Set s = SBG::LIB::SET_FACT.createSet();
  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz*2; k++) {
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
    if(k%2==0)
      pw.emplaceBack(Map(s1, exp));
    else
      pw2.emplaceBack(Map(s1, exp));

  }

  
  auto start = std::chrono::high_resolution_clock::now();
  pw.concatenation(pw2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED CONCATENATION WITH SET TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, UnordRestrict)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 400;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  Set s = SBG::LIB::SET_FACT.createSet();
  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw.restrict(s);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED RESTTRICT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, OrdRestrict)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 400;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  Set s = SBG::LIB::SET_FACT.createSet();
  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw.restrict(s);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED RESTTRICT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, UnordComposition)
{
  unsigned int inter_sz = 100000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw2.composition(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED COMPOSITION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdComposition)
{
  unsigned int inter_sz = 100000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw2.composition(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED COMPOSITION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, UnordOffsetDomMap)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  for (unsigned int k = map_sz*25/100; k < map_sz*75/100; k++) {
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
  pw.offsetDom(pw2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL UNORDERED MAP OFFSETDOM TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, OrdOffsetDomMap)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz*25/100; k < map_sz*75/100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw.offsetDom(pw2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL ORDERED MAP OFFSETDOM TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, UnordFirstInvSet)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  Set s = SBG::LIB::SET_FACT.createSet();
  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  std::cout << "PWL MAP UNORDERED FIRSTINV WITH SET TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdFirstInvSet)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  Set s = SBG::LIB::SET_FACT.createSet();
  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
    for (unsigned int k = 0; k < map_sz; k++) {
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



TEST(PWMapPerf, UnordCompact)
{
  unsigned int inter_sz = 100;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  int fin;
  for (unsigned int k = 0; k < map_sz; k++) {
    LExp le1(1, k);
    Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    Set s1 = SBG::LIB::SET_FACT.createSet();
     int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i0(map_offset + 1, 1, map_offset + inter_sz);
      Interval idims(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      fin = map_offset + (j+1)*inter_sz;
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i0);
      mdi.emplaceBack(idims);
      mdi.emplaceBack(idims); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(Map(s1, exp));


  }
  
   for (unsigned int k = 1; k < map_sz; k++) {
    LExp le1(1, k);
    Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    Set s1 = SBG::LIB::SET_FACT.createSet();
    int new_map_offset = (map_sz+k) * fin * set_sz;
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i0(new_map_offset+ 1, 1, new_map_offset + inter_sz);
      Interval idims(new_map_offset + (j*inter_sz) + 1, 1, new_map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i0);
      mdi.emplaceBack(idims);
      mdi.emplaceBack(idims); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(Map(s1, exp));

  }


  auto start = std::chrono::high_resolution_clock::now();
  pw.compact();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED COMPACT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdCompact)
{
  unsigned int inter_sz = 100;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  int fin;
  for (unsigned int k = 0; k < map_sz; k++) {
    LExp le1(1, k);
    Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    Set s1 = SBG::LIB::SET_FACT.createSet();
     int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i0(map_offset + 1, 1, map_offset + inter_sz);
      Interval idims(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      fin = map_offset + (j+1)*inter_sz;
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i0);
      mdi.emplaceBack(idims);
      mdi.emplaceBack(idims); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(Map(s1, exp));


  }
  
   for (unsigned int k = 1; k < map_sz; k++) {
    LExp le1(1, k);
    Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    Set s1 = SBG::LIB::SET_FACT.createSet();
    int new_map_offset = (map_sz+k) * fin * set_sz;
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i0(new_map_offset+ 1, 1, new_map_offset + inter_sz);
      Interval idims(new_map_offset + (j*inter_sz) + 1, 1, new_map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i0);
      mdi.emplaceBack(idims);
      mdi.emplaceBack(idims); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(Map(s1, exp));

  }


  auto start = std::chrono::high_resolution_clock::now();
  pw.compact();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED COMPACT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, UnordAdd)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 200;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz*25/100; k < map_sz*75/100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw+pw2;
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL UNORDERED MAP ADD TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdAdd)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 200;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz*25/100; k < map_sz*75/100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw+pw2;
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL ORDERED MAP ADD TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, UnordMinus)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz*25/100; k < map_sz*75/100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw-pw2;
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL UNORDERED MAP MINUS TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdMinus)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz*25/100; k < map_sz*75/100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw-pw2;
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL ORDERED MAP MINUS TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, UnordIqualImage)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz*25/100; k < map_sz*75/100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw.equalImage(pw2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED EQUALIMAGE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, OrdIqualImage)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  PWMap pw2 = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = map_sz*25/100; k < map_sz*75/100; k++) {
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
  
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw.equalImage(pw2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED EQUALIMAGE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}



TEST(PWMapPerf, UnordMinAdj)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  auto start = std::chrono::high_resolution_clock::now();
  pw.minAdjMap(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL UNORDERED MAP MINADJ TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdMinAdj)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  auto start = std::chrono::high_resolution_clock::now();
  pw.minAdjMap(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL ORDERED MAP MINADJ TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, UnordMinMap)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw.minMap(sum);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED MINMAP TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, OrdMinMap)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  pw.minMap(sum);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED MINMAP TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, UnordInverse)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  auto start = std::chrono::high_resolution_clock::now();
  pw.inverse();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL UNORDERED MAP INVERSE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdInverse)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  auto start = std::chrono::high_resolution_clock::now();
  pw.inverse();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL ORDERED MAP INVERSE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, UnordReduce)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 300;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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


  auto start = std::chrono::high_resolution_clock::now();
  pw.reduce();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED REDUCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdReduce)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 50;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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


  auto start = std::chrono::high_resolution_clock::now();
  pw.reduce();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED REDUCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, UnordMapInf)
{
  int N = 1000;
  int sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (int i = 0; i < N; i += 6) {
    Interval i1(i * sz + 1, 1, (i + 1) * sz);
    Interval i2((i + 1) * sz + 1, 1, (i + 2) * sz);
    Interval i3((i + 2) * sz + 1, 1, (i + 3) * sz);
    Interval i4((i + 3) * sz + 1, 1, (i + 4) * sz);
    Interval i5((i + 4) * sz + 1, 1, (i + 5) * sz);
    Interval i6((i + 5) * sz + 1, 1, (i + 6) * sz);
    
    LExp le(1, sz);
    Exp exp1;
    exp1.emplaceBack(le);
    exp1.emplaceBack(le);

    SBG::LIB::MultiDimInter mdi1;
    mdi1.emplaceBack(i1);
    mdi1.emplaceBack(i1);
    
    SBG::LIB::MultiDimInter mdi2;
    mdi2.emplaceBack(i2);
    mdi2.emplaceBack(i2);
    
    SBG::LIB::MultiDimInter mdi3;
    mdi3.emplaceBack(i3);
    mdi3.emplaceBack(i3);
    
    SBG::LIB::MultiDimInter mdi4;
    mdi4.emplaceBack(i4);
    mdi4.emplaceBack(i4);
    
    SBG::LIB::MultiDimInter mdi5;
    mdi5.emplaceBack(i5);
    mdi5.emplaceBack(i5);
    
    SBG::LIB::MultiDimInter mdi6;
    mdi6.emplaceBack(i6);
    mdi6.emplaceBack(i6);
    
    Map sm1(mdi1, exp1);
    pw.emplaceBack(sm1);
    Map sm2(mdi2, exp1);
    pw.emplaceBack(sm2);
    Map sm3(mdi3, exp1);
    pw.emplaceBack(sm3);
    Map sm4(mdi4, exp1);
    pw.emplaceBack(sm4);
    Map sm5(mdi5, exp1);
    pw.emplaceBack(sm5);
    Map sm6(mdi6, exp1);
    pw.emplaceBack(sm6);
  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.mapInf();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED MAPINF TEST elapsed time: " << elapsed.count() << "ms\n";
}


TEST(PWMapPerf, OrdMapInf)
{
  int N = 1000;
  int sz = 1000;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (int i = 0; i < N; i += 6) {
    Interval i1(i * sz + 1, 1, (i + 1) * sz);
    Interval i2((i + 1) * sz + 1, 1, (i + 2) * sz);
    Interval i3((i + 2) * sz + 1, 1, (i + 3) * sz);
    Interval i4((i + 3) * sz + 1, 1, (i + 4) * sz);
    Interval i5((i + 4) * sz + 1, 1, (i + 5) * sz);
    Interval i6((i + 5) * sz + 1, 1, (i + 6) * sz);
    
    LExp le(1, sz);
    Exp exp1;
    exp1.emplaceBack(le);
    exp1.emplaceBack(le);

    SBG::LIB::MultiDimInter mdi1;
    mdi1.emplaceBack(i1);
    mdi1.emplaceBack(i1);
    
    SBG::LIB::MultiDimInter mdi2;
    mdi2.emplaceBack(i2);
    mdi2.emplaceBack(i2);
    
    SBG::LIB::MultiDimInter mdi3;
    mdi3.emplaceBack(i3);
    mdi3.emplaceBack(i3);
    
    SBG::LIB::MultiDimInter mdi4;
    mdi4.emplaceBack(i4);
    mdi4.emplaceBack(i4);
    
    SBG::LIB::MultiDimInter mdi5;
    mdi5.emplaceBack(i5);
    mdi5.emplaceBack(i5);
    
    SBG::LIB::MultiDimInter mdi6;
    mdi6.emplaceBack(i6);
    mdi6.emplaceBack(i6);
    
    Map sm1(mdi1, exp1);
    pw.emplaceBack(sm1);
    Map sm2(mdi2, exp1);
    pw.emplaceBack(sm2);
    Map sm3(mdi3, exp1);
    pw.emplaceBack(sm3);
    Map sm4(mdi4, exp1);
    pw.emplaceBack(sm4);
    Map sm5(mdi5, exp1);
    pw.emplaceBack(sm5);
    Map sm6(mdi6, exp1);
    pw.emplaceBack(sm6);
  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.mapInf();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED MAPINF TEST elapsed time: " << elapsed.count() << "ms\n";
}

TEST(PWMapPerf, UnordFirstInv)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  auto start = std::chrono::high_resolution_clock::now();
  pw.firstInv();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL UNORDERED MAP FIRSTINV TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, OrdFirstInv)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(1);
  SBG::Eval::setPWFactory(1);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
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
  
  auto start = std::chrono::high_resolution_clock::now();
  pw.firstInv();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL ORDERED MAP FIRSTINV TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, minAdj)
{
  unsigned int inter_sz = 100000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(0);

  PWMap pw = SBG::LIB::PW_FACT.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    LExp le1(1, k);
    Set s1 = SBG::LIB::SET_FACT.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; ++j) {
      Interval i(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      s1.emplaceBack(SetPiece(i));
    }
    pw.emplaceBack(Map(s1, Exp(le1)));
  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.minAdjMap(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP COMPOSITION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, MapInf)
{
  int N = 300;
  int sz = 1000;

  SBG::Eval::setSetFactory(2);
  SBG::Eval::setPWFactory(0);

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
