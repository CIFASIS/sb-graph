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

#include <boost/foreach.hpp>
#include "sbg/interval.hpp"

namespace SBG {

namespace LIB {

using NAT = Util::NAT;

typedef std::vector<Interval> InterVector;
typedef InterVector::iterator InterVectorIt;
typedef InterVector::const_iterator InterVectorConstIt;

struct MultiDimInter {
  typedef InterVectorIt iterator;
  typedef InterVectorConstIt const_iterator;

  member_class(InterVector, intervals);

  MultiDimInter();
  MultiDimInter(MD_NAT x);
  MultiDimInter(Interval i);
  MultiDimInter(unsigned int nmbr_copies, Interval i);
  MultiDimInter(InterVector iv);

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  std::size_t size() const;  
  void emplaceBack(Interval i);
  Interval &operator[](std::size_t n);
  const Interval &operator[](std::size_t n) const;

  eq_class(MultiDimInter);
  neq_class(MultiDimInter);
  lt_class(MultiDimInter);
};
std::ostream &operator<<(std::ostream &out, const MultiDimInter &mi);

/**
 * @brief Traditional set operations.

 */

unsigned int cardinal(MultiDimInter mdi);
bool isEmpty(MultiDimInter mdi);
bool isMember(MD_NAT x, MultiDimInter mdi); // Useful?
Util::MD_NAT minElem(MultiDimInter mdi);
Util::MD_NAT maxElem(MultiDimInter mdi);
MultiDimInter intersection(MultiDimInter mdi1, MultiDimInter mdi2);

/**
 * @brief Extra operations.
 */

MultiDimInter offset(Util::MD_NAT off, MultiDimInter mdi);
MultiDimInter least(MultiDimInter mdi1, MultiDimInter mdi2);
bool isUnidim(MultiDimInter mdi);

std::size_t hash_value(const MultiDimInter &mdi);

typedef MultiDimInter SetPiece;

} // namespace LIB

}  // namespace SBG

#endif
