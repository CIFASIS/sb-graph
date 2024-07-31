/** @file multidim_inter.hpp

 @brief <b>Multi-dimensional interval implementation</b>

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

#ifndef SBG_MULTIDIM_INTERVAL_HPP
#define SBG_MULTIDIM_INTERVAL_HPP

#include "sbg/interval.hpp"

namespace SBG {

namespace LIB {

using NAT = Util::NAT;

typedef std::vector<Interval> InterVector;
typedef InterVector::iterator InterVectorIt;
typedef InterVector::const_iterator InterVectorConstIt;

struct MultiDimInter {
  using MaybeMDI = std::optional<MultiDimInter>;

  member_class(InterVector, intervals);

  MultiDimInter();
  MultiDimInter(MD_NAT x);
  MultiDimInter(Interval i);
  MultiDimInter(unsigned int nmbr_copies, Interval i);
  MultiDimInter(InterVector iv);

  typedef InterVectorIt iterator;
  typedef InterVectorConstIt const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  void emplaceBack(Interval i);
  Interval &operator[](std::size_t n);
  const Interval &operator[](std::size_t n) const;

  bool operator==(const MultiDimInter &other) const;
  bool operator!=(const MultiDimInter &other) const;
  bool operator<(const MultiDimInter &other) const;

  /**
   * @brief Traditional set operations.
   */
  std::size_t arity() const;
  unsigned int cardinal() const;
  bool isEmpty() const;
  bool isMember(const MD_NAT &x) const; // Useful?
  Util::MD_NAT minElem() const;
  Util::MD_NAT maxElem() const;
  MultiDimInter intersection(const MultiDimInter &other) const;

  /**
   * @brief Extra operations.
   */
  MultiDimInter offset(const MD_NAT &off) const;
  MultiDimInter least(const MultiDimInter &other) const;
  bool isUnidim() const;
  MaybeMDI compact(const MultiDimInter &other) const;
};
std::ostream &operator<<(std::ostream &out, const MultiDimInter &mi);

typedef MultiDimInter SetPiece;

} // namespace LIB

}  // namespace SBG

#endif
