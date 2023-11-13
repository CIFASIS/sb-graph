/** @file interval.hpp

 @brief <b>Interval implementation</b>

 An interval [lo:st:hi] is the set of natural numbers 
 {x : lo ≤ x ≤ hi ∧ x = lo + st * k, k ∈ ℕ}. Notice that an interval
 represents an unique set, but a set can have multiple representants.
 As such, in the implementation we will use the interval with the minimum
 hi existing. To ensure this, operations will be defined to create a new
 interval, and not to modify the existing ones.

 <hr>

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

#ifndef SBG_INTERVAL_HPP
#define SBG_INTERVAL_HPP

#include <cmath>
#include <numeric>
#include <optional>

#include <iostream>

#include "util/defs.hpp"

namespace SBG {

namespace LIB {

using NAT = Util::NAT;
using MD_NAT = Util::MD_NAT;

struct Interval {
  member_class(NAT, begin);
  member_class(NAT, step);
  member_class(NAT, end);

  Interval();
  Interval(NAT x);
  Interval(NAT begin, NAT step, NAT end);

  eq_class(Interval);
  neq_class(Interval);
  lt_class(Interval);
};
std::ostream &operator<<(std::ostream &out, const Interval &i);

/**
 * @brief Traditional set operations.
 */

unsigned int cardinal(Interval i);
bool isEmpty(Interval i);
bool isMember(NAT x, Interval i);
Util::NAT minElem(Interval i);
Util::NAT maxElem(Interval i);
Interval intersection(Interval i1, Interval i2);

/**
 * @brief Extra operations.
 */

Interval offset(Util::NAT off, Interval i);
Interval least(Interval i1, Interval i2);
typedef std::optional<Interval> MaybeInterval;
MaybeInterval canonize(Interval i1, Interval i2);

std::size_t hash_value(const Interval &i);

} // namespace LIB

}  // namespace SBG

#endif
