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

#include "sbg/set_fact.hpp"
#include "sbg/pwmap_fact.hpp"

TEST(SetPerf, OrdDenseIntersection)
{
  int N = 10000;

  SBG::LIB::OrdUnidimDenseSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.intersection(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DENSE INTERSECTION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, OrdDenseDifference)
{
  int N = 10000;

  SBG::LIB::OrdUnidimDenseSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.difference(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DENSE DIFFERENCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, OrdDenseUnion)
{
  int N = 10000;

  SBG::LIB::OrdUnidimDenseSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }
  auto start = std::chrono::high_resolution_clock::now();
  s1.cup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DENSE UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, OrdDenseDisjointUnion)
{
  int N = 10000;

  SBG::LIB::OrdUnidimDenseSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; j+=2) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
    SBG::LIB::Interval i2((j+1)*100+1, 1, (j+2)*100);
    s2.emplaceBack(i2);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.disjointCup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DENSE DISJOINT UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, OrdDenseComplement)
{
  int N = 10000;

  SBG::LIB::OrdUnidimDenseSetFact fact;
  SBG::LIB::Set s1 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.complement();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DENSE COMPLEMENT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, UnordIntersection)
{
  int N = 10000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.intersection(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNORDERED INTERSECTION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, UnordDifference)
{
  int N = 10000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.difference(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNORDERED DIFFERENCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, UnordUnion)
{
  int N = 10000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.cup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNORDERED UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, UnordDisjointUnion)
{
  int N = 10000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; j+=2) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
    SBG::LIB::Interval i2((j+1)*100+1, 1, (j+2)*100);
    s2.emplaceBack(i2);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.disjointCup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNORDERED DISJOINT UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, UnordComplement)
{
  int N = 10000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.complement();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNORDERED COMPLEMENT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, OrdIntersection)
{
  int N = 10000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.intersection(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED INTERSECTION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, OrdDifference)
{
  int N = 10000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.difference(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DIFFERENCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, OrdUnion)
{
  int N = 10000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*105+1, 1, (j+1)*105);
    s2.emplaceBack(i);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.cup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, OrdDisjointUnion)
{
  int N = 10000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; j+=2) {
    SBG::LIB::Interval i(j*100+1, 1, (j+1)*100);
    s1.emplaceBack(i);
    SBG::LIB::Interval i2((j+1)*100+1, 1, (j+2)*100);
    s2.emplaceBack(i2);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.disjointCup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DISJOINT UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, OrdComplement)
{
  int N = 10000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i(j*100, 1, (j+1)*100);
    s1.emplaceBack(i);
    
    /*
            SBG::LIB::Interval i(j*100+1, 2, (j+1)*100);
    s1.emplaceBack(i);
    SBG::LIB::Interval i2(j*100+50, 2, (j+1)*100+48);
    s1.emplaceBack(i2);*/
  }
  
  
  
  auto start = std::chrono::high_resolution_clock::now();
  s1.complement();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED COMPLEMENT TEST elapsed time: " << elapsed.count() << "ms\n";
  
  SUCCEED();

}



TEST(SetPerf, OrdIntersectionThreeDims)
{ 
  int N = 1000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*100+1, 2, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s1.emplaceBack(mdi);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*105+1, 2, (j+1)*105);
    SBG::LIB::Interval i2(j*105+1, 1, (j+1)*105);
    SBG::LIB::Interval i3(j*105+1, 1, (j+1)*105);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s2.emplaceBack(mdi);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.intersection(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED INTERSECTION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, OrdDifferenceThreeDims)
{
  int N = 1000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*100+1, 2, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s1.emplaceBack(mdi);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*105+1, 2, (j+1)*105);
    SBG::LIB::Interval i2(j*105+1, 1, (j+1)*105);
    SBG::LIB::Interval i3(j*105+1, 1, (j+1)*105);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s2.emplaceBack(mdi);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.difference(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DIFFERENCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}



TEST(SetPerf, OrdUnionThreeDims)
{
  int N = 1000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*100+1, 2, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s1.emplaceBack(mdi);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*105+1, 2, (j+1)*105);
    SBG::LIB::Interval i2(j*105+1, 1, (j+1)*105);
    SBG::LIB::Interval i3(j*105+1, 1, (j+1)*105);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s2.emplaceBack(mdi);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.cup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, OrdComplementThreeDims)
{
  int N = 1000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet();
   for (int j = 0; j < N; ++j) {

    SBG::LIB::Interval i1(j*100+1, 2, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    
    //std::cout << mdi;
    s1.emplaceBack(mdi);

  }



  auto start = std::chrono::high_resolution_clock::now();
  s1.complement();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED COMPLEMENT TEST elapsed time: " << elapsed.count() << "ms\n";
  SUCCEED();

}

TEST(SetPerf, OrdDisjointUnionThreeDims)
{
  int N = 40000;

  SBG::LIB::OrdSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; j+=2) {
    SBG::LIB::Interval i1(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s1.emplaceBack(mdi);
    SBG::LIB::Interval i4((j+1)*100+1, 1, (j+2)*100);
    SBG::LIB::MultiDimInter mdi2;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i4);
    s2.emplaceBack(mdi2);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.disjointCup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED DISJOINT UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}


TEST(SetPerf, UnordIntersectionThreeDims)
{

  std::cout << "\n";
  int N = 1000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*100+1, 2, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s1.emplaceBack(mdi);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*105+1, 1, (j+1)*105);
    SBG::LIB::Interval i2(j*105+1, 1, (j+1)*105);
    SBG::LIB::Interval i3(j*105+1, 1, (j+1)*105);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s2.emplaceBack(mdi);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.intersection(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED UNINTERSECTION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, UnordDifferenceThreeDims)
{
  int N = 1000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*100+1, 2, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s1.emplaceBack(mdi);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*105+1, 2, (j+1)*105);
    SBG::LIB::Interval i2(j*105+1, 1, (j+1)*105);
    SBG::LIB::Interval i3(j*105+1, 1, (j+1)*105);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s2.emplaceBack(mdi);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.difference(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "ORDERED UNDIFFERENCE TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, UnordUnionThreeDims)
{
  int N = 1000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*100+1, 2, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s1.emplaceBack(mdi);
  }

  for (int j = 0; j < N; ++j) {
    SBG::LIB::Interval i1(j*105+1, 2, (j+1)*105);
    SBG::LIB::Interval i2(j*105+1, 1, (j+1)*105);
    SBG::LIB::Interval i3(j*105+1, 1, (j+1)*105);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s2.emplaceBack(mdi);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.cup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNORDERED UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, UnordComplementThreeDims)
{
  int N = 1000 ;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet();
  for (int j = 0; j < N; ++j) {

    SBG::LIB::Interval i1(j*100+1, 2, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    
    //std::cout << mdi;
    s1.emplaceBack(mdi);

  }



  auto start = std::chrono::high_resolution_clock::now();
  s1.complement();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNORDERED COMPLEMENT TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

TEST(SetPerf, UnordDisjointUnionThreeDims)
{
  int N = 40000;

  SBG::LIB::UnordSetFact fact;
  SBG::LIB::Set s1 = fact.createSet(), s2 = fact.createSet();
  for (int j = 0; j < N; j+=2) {
    SBG::LIB::Interval i1(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i2(j*100+1, 1, (j+1)*100);
    SBG::LIB::Interval i3(j*100+1, 1, (j+1)*100);
    SBG::LIB::MultiDimInter mdi;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i3);
    s1.emplaceBack(mdi);
    SBG::LIB::Interval i4((j+1)*100+1, 1, (j+2)*100);
    SBG::LIB::MultiDimInter mdi2;
    mdi.emplaceBack(i1);
    mdi.emplaceBack(i2);
    mdi.emplaceBack(i4);
    s2.emplaceBack(mdi2);
  }

  auto start = std::chrono::high_resolution_clock::now();
  s1.disjointCup(s2);
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "UNORDERED DISJOINT UNION TEST elapsed time: " << elapsed.count() << "ms\n";

  SUCCEED();
}

