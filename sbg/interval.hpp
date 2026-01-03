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

#include <optional>

#include "sbg/natural.hpp"

namespace SBG {

namespace LIB {

class Interval;

typedef std::optional<Interval> MaybeInterval;

class Interval {
  member_class(NAT, begin);
  member_class(NAT, step);
  member_class(NAT, end);

  /**
   * @brief Construct an empty interval.
   */
  Interval();

  /**
   * @brief Construct an interval only containing \p x.
   */
  Interval(NAT x);

  /**
   * @brief Construct an interval with \p begin, \p step and \p end.
   */
  Interval(NAT begin, NAT step, NAT end);

  bool operator==(const Interval &i) const;
  bool operator!=(const Interval &i) const;

  /**
   * @brief An interval i1 is less than another interval i2 iff
   * min(i1) < min(i2). This operation is later needed to implement ordered
   * sets.
   */ 
  bool operator<(const Interval &i) const;

  // Traditional set operations ------------------------------------------------

  /**
   * @brief Number of elements contained in the interval, i.e.
   * cardinal([1:1:10]) = 10. 
   */
  unsigned int cardinal() const;
  bool isEmpty() const;
  bool isMember(NAT x) const;
  Interval intersection(const Interval &i2) const;

  // Extra operations ----------------------------------------------------------

  /**
   * @brief Sum a constant value to every element of the interval.
   */
  Interval offset(NAT off) const;

  /**
   * @brief Operation that given two disjoint intervals returns the lesser one.
   * It will be used by ordered sets operations.
   */
  Interval least(const Interval &i2) const;

  /**
   * @brief Merge two contiguous intervals if possible. If not, then the result
   * is not an interval, so no value is returned.
   */
  MaybeInterval compact(const Interval &i2) const;
};
std::ostream &operator<<(std::ostream &out, const Interval &i);

} // namespace LIB

}  // namespace SBG

#endif
