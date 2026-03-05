/** @file natural.hpp

 @brief <b>Naturals (one and multi-dimensional) implementation</b>

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

#ifndef SBGRAPH_SBG_NATURAL_HPP_
#define SBGRAPH_SBG_NATURAL_HPP_

#include <iosfwd>
#include <limits>
#include <vector>

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Naturals implementation -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

using NAT = long long unsigned int;
constexpr NAT Inf = std::numeric_limits<NAT>::max();

////////////////////////////////////////////////////////////////////////////////
// Naturals implementation -----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class MD_NAT {
public:
  using VNAT = std::vector<NAT>;
  using Iterator = VNAT::iterator;
  using ConstIterator = VNAT::const_iterator;

  /**
   * @brief Constructs a zero-dimensional mdnat.
   */
  MD_NAT();

  /**
   * @brief Constructs a one-dimensional mdnat with value x.
   */
  MD_NAT(const NAT x);

  /**
   * @brief Constructs a nmbr_copies-dimensional mdnat with value x in every
   * dimension.
   */
  MD_NAT(const std::size_t k, NAT x);

  /**
   * @brief Range constructor.
   */
  MD_NAT(Iterator b, Iterator e);

  Iterator begin();
  Iterator end();
  ConstIterator begin() const;
  ConstIterator end() const;

  /**
   * @brief Add a dimension to the mdnat, and assign to it the value of x.
   */
  void pushBack(const NAT x);

  NAT& operator[](std::size_t n);
  const NAT& operator[](std::size_t n) const;

  bool operator==(const MD_NAT& other) const;
  bool operator!=(const MD_NAT& other) const;

  /**
   * @brief A mdnat x1 is less than other mdnat x2 iff there is some dimension
   * i for which x1[i] < x2[i] and for every j < i, x1[j] == x2[j]. 
   */ 
  bool operator<(const MD_NAT& other) const;

  bool operator<=(const MD_NAT& other) const;

  MD_NAT operator+(const MD_NAT& other) const;

  /**
   * @brief Number of dimensions of the mdnat, i.e. arity(1, 1, 1) = 3.
   */
  std::size_t arity() const;

private:
  VNAT _value;
};
std::ostream& operator<<(std::ostream& out, const MD_NAT& md);

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_NATURAL_HPP_
