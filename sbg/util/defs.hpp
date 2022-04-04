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

#include <boost/config.hpp>
#include <boost/unordered_set.hpp>

namespace SBG {

typedef int INT;
typedef double REAL;

#define Inf std::numeric_limits<INT>::max()

template <typename T, class = std::allocator<T>>
using OrdCT = std::list<T>;  // Ord stands for ordered container

template <typename Value, typename Hash = boost::hash<Value>, typename Pred = std::equal_to<Value>, typename Alloc = std::allocator<Value>>
using UnordCT = boost::unordered_set<Value>;  // Unord stands for unordered container

typedef OrdCT<INT> ORD_INTS;
typedef OrdCT<REAL> ORD_REALS;

std::ostream &operator<<(std::ostream &out, const ORD_INTS &nums);
std::ostream &operator<<(std::ostream &out, const ORD_REALS &nums);

// Helpful macros ---------------------------------------------------------------------------------

/**
 * @brief Define a variable member of a class and the getters and setters.
 *
 * The macro takes two parameters:
 *  + The variable type @c X.
 *  + The name of the variable @c Y.
 *
 * It will define the folowwing:
 *  + A member variable called: @c Y_
 *  + An constant getter method: @c Y()
 *  + A setter method: @c set_Y()
 *  + A getter ref method @c Y_ref()
 *
 */
#define member_class(X, Y) \
  X Y##_;                  \
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

}  // namespace SBG
