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

#include "util/defs.hpp"

namespace SBG {

namespace LIB {

using NAT = Util::NAT;
using MD_NAT = Util::MD_NAT;

struct Interval {
  using MaybeInterval = std::optional<Interval>;

  NAT begin() const;
  NAT &begin_ref();
  NAT step() const;
  NAT &step_ref();
  NAT end() const;
  NAT &end_ref();

  Interval();
  Interval(const NAT &x);
  Interval(const NAT &begin, const NAT &step, const NAT &end);
 
  bool operator==(const Interval &i) const;
  bool operator!=(const Interval &i) const;
  bool operator<(const Interval &i) const;

  /**
   * @brief Traditional set operations.
   */
  unsigned int cardinal() const;
  bool isEmpty() const;
  bool isMember(const NAT &x) const;
  Interval intersection(const Interval &i2) const;

  /**
   * @brief Extra operations.
   */
  Interval offset(const Util::NAT &off) const;
  Interval least(const Interval &i2) const;
  MaybeInterval compact(const Interval &i2) const;

  private:
  NAT begin_;
  NAT step_;
  NAT end_;
};
std::ostream &operator<<(std::ostream &out, const Interval &i);

} // namespace LIB

}  // namespace SBG

#endif
