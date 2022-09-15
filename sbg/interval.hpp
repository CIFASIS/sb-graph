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

namespace SBG {

// Intervals --------------------------------------------------------------------------------------

#define INTER_TEMPLATE                                                                                          \
  template <template<typename T, typename = std::less<T>, typename = std::allocator<T>> class UNIQUE_ORD_CT>

#define INTER_TEMP_TYPE IntervalImp1<UNIQUE_ORD_CT>

INTER_TEMPLATE
struct IntervalImp1 {
  member_class(INT, lo);
  member_class(INT, step);
  member_class(INT, hi);
  member_class(bool, empty);

  IntervalImp1();
  IntervalImp1(bool isEmpty);
  IntervalImp1(INT vlo, INT vstep, INT vhi);

  INT gcd(INT a, INT b);
  INT lcm(INT a, INT b);

  bool isIn(INT x);
  int card();
  IntervalImp1 cap(IntervalImp1 i2);
  UNIQUE_ORD_CT<IntervalImp1> complement();
  UNIQUE_ORD_CT<IntervalImp1> diff(IntervalImp1 i2);

  IntervalImp1 offset(int off);

  INT minElem();
  INT nextElem(INT cur);
  INT maxElem();

  IntervalImp1 normalize(IntervalImp1 i2);

  eq_class(IntervalImp1);
  neq_class(IntervalImp1);
  lt_class(IntervalImp1);
};

// >>>>> To add new implementation, add:
// struct IntervalImp2 { ... }
//
typedef IntervalImp1<UniqueOrdCT> Interval;
std::size_t hash_value(const Interval &inter);

// >>>>> To change implementation of Interval:
// typedef IntervalImp2 Interval;

printable_temp(INTER_TEMPLATE, INTER_TEMP_TYPE);

typedef OrdCT<Interval> ORD_INTERS;
typedef UniqueOrdCT<Interval> UNIQUE_ORD_INTERS;
typedef UnordCT<Interval> UNORD_INTERS;

std::ostream &operator<<(std::ostream &out, const ORD_INTERS &inters);
std::ostream &operator<<(std::ostream &out, const UNIQUE_ORD_INTERS &inters);
std::ostream &operator<<(std::ostream &out, const UNORD_INTERS &inters);

}  // namespace SBG
