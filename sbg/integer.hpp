/** @file integer.hpp

 @brief <b>Multi-dimensional integer implementation</b>

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

#ifndef SBGRAPH_SBG_INTEGER_HPP_
#define SBGRAPH_SBG_INTEGER_HPP_

#include <iosfwd>
#include <limits>
#include <vector>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Integers implementation -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

using Int = long long int;
constexpr Int kPosInf = std::numeric_limits<Int>::max();
constexpr Int kNegInf = std::numeric_limits<Int>::min();

////////////////////////////////////////////////////////////////////////////////
// Multi-dimensional integers implementation -----------------------------------
////////////////////////////////////////////////////////////////////////////////

class IntTuple {
public:
  using Iterator = std::vector<Int>::iterator;
  using ConstIterator = std::vector<Int>::const_iterator;

  /**
   * @brief Constructs a zero-dimensional integer tuple.
   */
  IntTuple();

  /**
   * @brief Constructs a one-dimensional integer tuple with value x.
   */
  IntTuple(const Int x);

  /**
   * @brief Constructs the tuple (x, ..., x) repeated \p k times.
   */
  IntTuple(const std::size_t k, Int x);

  /**
   * @brief Range constructor.
   */
  IntTuple(Iterator b, Iterator e);

  Iterator begin();
  Iterator end();
  ConstIterator begin() const;
  ConstIterator end() const;

  /**
   * @brief Add a dimension to the integer tuple, and assign to it the value of
   * x.
   */
  void pushBack(const Int x);

  Int& operator[](std::size_t n);
  const Int& operator[](std::size_t n) const;

  bool operator==(const IntTuple& other) const;
  bool operator!=(const IntTuple& other) const;

  /**
   * @brief An integer tuple x1 is less than other integer tuple x2 iff there
   * is some dimension i for which x1[i] < x2[i] and for every j < i,
   * x1[j] == x2[j]. 
   */ 
  bool operator<(const IntTuple& other) const;

  bool operator<=(const IntTuple& other) const;

  IntTuple operator+(const IntTuple& other) const;

  /**
   * @brief Number of dimensions of the integer tuple, i.e. arity(1, 1, 1) = 3.
   */
  std::size_t arity() const;

private:
  std::vector<Int> _value;
};
std::ostream& operator<<(std::ostream& out, const IntTuple& md);

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_INTEGER_HPP_
