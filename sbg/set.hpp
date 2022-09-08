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
#pragma once

#include <sbg/util/defs.hpp>
#include <sbg/multi_interval.hpp>

namespace SBG {

// Implementation 1: non canonic sets --------------------------------------------------------------

#define SET_TEMPLATE1                                                          \
  template <template<typename T, typename = std::allocator<T>> class ORD_CT,   \
            template <typename Value,                                          \
                      typename Hash = boost::hash<Value>,                      \
                      typename Pred = std::equal_to<Value>,                    \
                      typename Alloc = std::allocator<Value>>                  \
            class UNORD_CT,                                                    \
            typename MI_IMP, typename INT_IMP>

#define SET_TEMP_TYPE1                                        \
  SetImp1<ORD_CT, UNORD_CT, MI_IMP, INT_IMP>

SET_TEMPLATE1
struct SetImp1 {
  typedef UNORD_CT<MI_IMP> AtomSets;
  typedef typename AtomSets::iterator AtomSetsIt;

  member_class(AtomSets, asets);
  member_class(int, ndim);

  SetImp1();
  SetImp1(MI_IMP as);
  SetImp1(AtomSets ss);
  SetImp1(ORD_CT<INT> v);

  void addAtomSet(MI_IMP aset2);
  void addAtomSets(AtomSets sets2);

  bool empty();

  bool isIn(ORD_CT<INT_IMP> elem);
  int card();
  bool subseteq(SetImp1 set2);
  bool subset(SetImp1 set2);
  SetImp1 cap(SetImp1 set2);
  SetImp1 diff(SetImp1 set2);
  SetImp1 cup(SetImp1 set2);

  ORD_CT<INT_IMP> minElem();
  ORD_CT<INT_IMP> maxElem();

  SetImp1 normalize();

  SetImp1 crossProd(SetImp1 set2);

  ORD_CT<SET_TEMP_TYPE1> atomize();

  eq_class(SetImp1);
  neq_class(SetImp1);
  lt_class(SetImp1);

  size_t hash();
};

printable_temp(SET_TEMPLATE1, SET_TEMP_TYPE1);

// Implementation 2: canonic sets ------------------------------------------------------------------

#define SET_TEMPLATE2                                                               \
  template <template<typename T, typename = std::allocator<T>> class ORD_CT,        \
            template<typename T, typename = std::less<T>, typename = std::allocator<T>> class UNIQUE_ORD_CT, \
            template <typename Value,                                          \
                      typename Hash = boost::hash<Value>,                      \
                      typename Pred = std::equal_to<Value>,                    \
                      typename Alloc = std::allocator<Value>>                  \
            class UNORD_CT,                                                    \
            typename MI_IMP, typename INT_IMP>

#define SET_TEMP_TYPE2                                        \
  SetImp2<ORD_CT, UNIQUE_ORD_CT, UNORD_CT, MI_IMP, INT_IMP>

SET_TEMPLATE2
struct SetImp2 {
  typedef UNORD_CT<MI_IMP> UnordAtomSets;
  typedef typename UnordAtomSets::iterator UnordAtomSetsIt;
  typedef UNIQUE_ORD_CT<MI_IMP> AtomSets;
  typedef typename AtomSets::iterator AtomSetsIt;

  member_class(AtomSets, asets);
  member_class(int, ndim);

  SetImp2();
  SetImp2(MI_IMP as);
  SetImp2(AtomSets ss);
  SetImp2(UNORD_CT<MI_IMP> ss);
  SetImp2(ORD_CT<INT> v);

  void addAtomSet(MI_IMP aset2);
  void addLastAtomSet(MI_IMP aset2);
  void addAtomSets(AtomSets sets2);

  bool empty();

  bool isIn(ORD_CT<INT_IMP> elem);
  int card();
  bool subseteq(SetImp2 set2);
  bool subset(SetImp2 set2);
  SetImp2 linearTraverse(SetImp2 set2, AtomSets (*f)(MI_IMP, MI_IMP));
  SetImp2 cap(SetImp2 set2);
  SetImp2 diff(SetImp2 set2);
  SetImp2 cup(SetImp2 set2);

  ORD_CT<INT_IMP> minElem();
  ORD_CT<INT_IMP> nextElem(ORD_CT<INT_IMP> cur);
  ORD_CT<INT_IMP> maxElem();

  SetImp2 normalize();

  SetImp2 crossProd(SetImp2 set2);

  UNIQUE_ORD_CT<SET_TEMP_TYPE2> atomize();

  eq_class(SetImp2);
  neq_class(SetImp2);
  lt_class(SetImp2);
};

printable_temp(SET_TEMPLATE2, SET_TEMP_TYPE2);

// Chosen implementation ---------------------------------------------------------------------------

typedef SetImp2<OrdCT, UniqueOrdCT, UnordCT, MultiInterval, INT> Set;
std::size_t hash_value(const Set &set);

Set createSet(Interval i);
Set createSet(MultiInterval mi);

} // namespace SBG
