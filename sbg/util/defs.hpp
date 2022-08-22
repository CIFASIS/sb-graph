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

#include <boost/range/combine.hpp>
#include <boost/config.hpp>
#include <boost/unordered_set.hpp>

namespace SBG {

typedef int INT;
typedef double REAL;

#define Inf std::numeric_limits<INT>::max()

template <typename T, class = std::allocator<T>>
using OrdCT = std::list<T>;  // Ordered container, with duplicates

template <class T, class Compare = std::less<T>, class Alloc = std::allocator<T>>
using UniqueOrdCT = std::set<T>; // Ordered container, without duplicates

template <typename Value, typename Hash = boost::hash<Value>, typename Pred = std::equal_to<Value>, typename Alloc = std::allocator<Value>>
using UnordCT = boost::unordered_set<Value>;  // Unordered container without duplicates

typedef OrdCT<INT> ORD_INTS;
typedef OrdCT<REAL> ORD_REALS;

std::ostream &operator<<(std::ostream &out, const ORD_INTS &nums);
std::ostream &operator<<(std::ostream &out, const ORD_REALS &nums);

// Helpful macros ---------------------------------------------------------------------------------

#define member_class(X, Y) \
private:                   \
  X Y##_;                  \
public:                    \
  X Y() const;             \
  void set_##Y(X x);       \
  X &Y##_ref();

#define member_imp(C, X, Y)          \
  X C::Y() const { return Y##_; }    \
  void C::set_##Y(X x) { Y##_ = x; } \
  X &C::Y##_ref() { return Y##_; }

#define member_imp_temp(T, C, X, Y)    \
  T X C::Y() const { return Y##_; }    \
  T void C::set_##Y(X x) { Y##_ = x; } \
  T X &C::Y##_ref() { return Y##_; }

#define eq_class(X) bool operator==(const X &other) const;
#define neq_class(X) bool operator!=(const X &other) const;
#define lt_class(X) bool operator<(const X &other) const;
#define gt_class(X) bool operator>(const X &other) const;

#define printable_temp(T, X) T std::ostream &operator<<(std::ostream &out, const X &);

// Helpful functions ------------------------------------------------------------------------------

#define parallel_foreach2(X, Y) for (auto&& items : boost::combine(X, Y))
#define parallel_foreach3(X, Y) for (auto&& items : boost::combine(X, Y))

}  // namespace SBG
