/** @file interval.hpp

 @brief <b>Interval implementation</b>

 An interval [lo:st:hi] is the set of integer numbers 
 {x : lo ≤ x ≤ hi ∧ x = lo + st * k, k ∈ ℤ}. Notice that an interval
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

#ifndef SBGRAPH_SBG_INTERVAL_HPP_
#define SBGRAPH_SBG_INTERVAL_HPP_

#include "sbg/integer.hpp"
#include "sbg/perimeter.hpp"

#include "rapidjson/document.h"

#include <iosfwd>
#include <optional>

namespace SBG {

namespace LIB {

namespace detail {

class Interval;

using MaybeInterval = std::optional<Interval>;

class Interval {
public:
  /**
   * @brief Construct an empty interval.
   */
  Interval();

  /**
   * @brief Construct an interval only containing \p x.
   */
  Interval(const Int x);

  /**
   * @brief Construct an interval with \p begin, \p step and \p end.
   */
  Interval(const Int begin, const Int step, const Int end);

  const Int& begin() const;
  const Int& step() const;
  const Int& end() const;

  bool operator<(const Interval& other) const;

  /**
   * @brief Number of elements contained in the interval, i.e.
   * cardinal([1:1:10]) = 10. 
   */
  unsigned int cardinal() const;
  bool isEmpty() const;
  Int minElem() const;
  Int maxElem() const;
  Interval intersection(const Interval& other) const;

  /**
   * @brief Sum a constant value to every element of the interval.
   */
  Interval translate(const Int off) const;

  Perimeter perimeter() const;

  /**
   * @brief Merge two contiguous intervals if possible. If not, then the result
   * is not an interval, so no value is returned.
   */
  MaybeInterval compact(const Interval& other) const;

private:
  Int _begin;
  Int _step;
  Int _end;
};

bool operator==(const Interval& lhs, const Interval& rhs);

bool operator!=(const Interval& lhs, const Interval& rhs);

std::ostream& operator<<(std::ostream& out, const Interval& i);

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(Interval i, rapidjson::Document::AllocatorType& alloc);

// Interval constans -----------------------------------------------------------

const Interval kOneDimUniverse = Interval{kNegInf, 1, kPosInf};

} // namespace detail

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_INTERVAL_HPP_
