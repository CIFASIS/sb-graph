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

#include "sbg/af_pwmap.hpp"


TEST(PWMapPerf, UnordRestrict)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::Set s = set_fact.createSet();
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
      if(k%2==0)
        s.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  
  auto start = std::chrono::high_resolution_clock::now();
  pw.restrict(s);
  //res.print(std::cout);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED RESTTRICT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdRestrict)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::Set s = set_fact.createSet();
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::OrdPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
      if(k%2==0)
        s.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  
  auto start = std::chrono::high_resolution_clock::now();
  pw.restrict(s);
  //res.print(std::cout);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED RESTTRICT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, UnordIqualImage)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }
  
  auto start = std::chrono::high_resolution_clock::now();
  pw.equalImage(pw);
  //res.print(std::cout);
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

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::OrdPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.equalImage(pw);
  //res.print(std::cout);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED EQUALIMAGE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, UnordCombine)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 20;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  SBG::LIB::PWMap res = pw.combine(pw);
  //res.print(std::cout);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP UNORDERED COMBINE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdCombine)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 20;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::OrdPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  SBG::LIB::PWMap res = pw.combine(pw);
  //res.print(std::cout);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED COMBINE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}



TEST(PWMapPerf, UnordComposition)
{
  unsigned int inter_sz = 100000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  //std::cout << "original\n";
  //pw.print(std::cout);
  pw.composition(pw);
  //std::cout << "\n";
  //std::cout << "composition\n";
  //pw.print(std::cout);
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

  SBG::LIB::OrdDenseAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::OrdPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  //std::cout << "original\n";
  //pw.print(std::cout);
  //std::cout << "\n";
  pw.composition(pw);
  //std::cout << "composition\n";
  //pw.print(std::cout);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP ORDERED COMPOSITION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, UnordAdd)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  pw=pw+pw;
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL UNORDERED MAP ADD TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, OrdAdd)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::OrdPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }
  
  auto start = std::chrono::high_resolution_clock::now();
  pw=pw+pw;
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

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  auto start = std::chrono::high_resolution_clock::now();
  pw=pw-pw;
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

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::OrdPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  
  auto start = std::chrono::high_resolution_clock::now();
  pw=pw-pw;
  
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL ORDERED MAP MINUS TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}




TEST(PWMapPerf, UnordOffsetDomMap)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }

  
  auto start = std::chrono::high_resolution_clock::now();
  pw.offsetDom(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL UNORDERED MAP OFFSETDOM TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, OrdOffsetDomMap)
{
  unsigned int inter_sz = 10000;
  unsigned int set_sz = 10;
  unsigned int map_sz = 1000;

  SBG::LIB::OrdAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::OrdPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Exp exp;
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    exp.emplaceBack(le1);
    SBG::LIB::Set s1 = map_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i1(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      SBG::LIB::MultiDimInter mdi;
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1);
      mdi.emplaceBack(i1); 
      s1.emplaceBack(mdi);
    }
    pw.emplaceBack(map_fact.createMap(s1, exp));

  }
  

  
  auto start = std::chrono::high_resolution_clock::now();
  pw.offsetDom(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL ORDERED MAP OFFSETDOM TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(PWMapPerf, minAdj)
{
  unsigned int inter_sz = 100000;
  unsigned int set_sz = 100;
  unsigned int map_sz = 100;

  SBG::LIB::OrdDenseAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
  for (unsigned int k = 0; k < map_sz; k++) {
    SBG::LIB::LExp le1(1, k);
    SBG::LIB::Set s1 = set_fact.createSet();
    int map_offset = k * inter_sz * set_sz;
    for (unsigned int j = 0; j < set_sz; j++) {
      SBG::LIB::Interval i(map_offset + (j*inter_sz) + 1, 1, map_offset + (j+1)*inter_sz);
      s1.emplaceBack(SBG::LIB::SetPiece(i));
    }
    pw.emplaceBack(map_fact.createMap(s1, SBG::LIB::Exp(le1)));
  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.minAdjMap(pw);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP COMPOSITION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(PWMapPerf, MapInf) {
  int N = 300;
  int sz = 1000;

  SBG::LIB::OrdDenseAF set_fact;
  SBG::LIB::MapAF map_fact(set_fact);
  SBG::LIB::UnordPWMapAF pw_fact(map_fact);
  SBG::LIB::PWMap pw = pw_fact.createPWMap();
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

    SBG::LIB::Map sm1 = map_fact.createMap(i1, le1);
    pw.emplaceBack(sm1);
    SBG::LIB::Map sm2 = map_fact.createMap(i2, le2);
    pw.emplaceBack(sm2);
    SBG::LIB::Map sm3 = map_fact.createMap(i3, le3);
    pw.emplaceBack(sm3);
    SBG::LIB::Map sm4 = map_fact.createMap(i4, le4);
    pw.emplaceBack(sm4);
    SBG::LIB::Map sm5 = map_fact.createMap(i5, le5);
    pw.emplaceBack(sm5);
    SBG::LIB::Map sm6 = map_fact.createMap(i6, le6);
    pw.emplaceBack(sm6);
  }

  auto start = std::chrono::high_resolution_clock::now();
  pw.mapInf();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "PWL MAP MAPINF TEST elapsed time: " << elapsed.count() << "ms\n";
}
