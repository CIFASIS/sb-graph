/** @file unord_pw_mdinter.hpp

 @brief <b>Piecewise  multi-dimensional interval</b>

 A piecewise multi-dimensional interval is a collection of non-empty disjoint
 intervals, and its corresponding set is the union of all composing mdis.
 The constructors aren't safe as they don't check if intervals are disjoint and
 non-empty.

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

#ifndef SBG_UNORD_PW_MDINTERVAL_HPP
#define SBG_UNORD_PW_MDINTERVAL_HPP

#include <boost/range.hpp>
#include <boost/unordered/unordered_flat_set.hpp>

#include "sbg/multidim_inter.hpp"

namespace SBG {

namespace LIB {

// Container -------------------------------------------------------------------

typedef boost::unordered::unordered_flat_set<SetPiece> MDInterUnordSet;
typedef MDInterUnordSet::iterator MDInterUnordSetIt;
typedef MDInterUnordSet::const_iterator MDInterUnordSetConstIt;
std::ostream &operator<<(std::ostream &out, const MDInterUnordSet &ii);

// Non-optimized implementation ------------------------------------------------

struct UnordPWMDInter {
  typedef MDInterUnordSetIt iterator;
  typedef MDInterUnordSetConstIt const_iterator;

  member_class(MDInterUnordSet, pieces);

  UnordPWMDInter();
  UnordPWMDInter(SetPiece mdi);
  UnordPWMDInter(MDInterUnordSet c);

  std::size_t size();
  void emplace(SetPiece mdi);
  void emplace_hint(MDInterUnordSetIt it, SetPiece mdi);
  iterator begin();
  const_iterator end();
  SetPiece operator[](std::size_t n);

  eq_class(UnordPWMDInter);
  lt_class(UnordPWMDInter);
};
std::ostream &operator<<(std::ostream &out, const UnordPWMDInter &pwi);

/**
 * @brief Traditional set operations.
 */

unsigned int cardinal(UnordPWMDInter pwi);
bool isEmpty(UnordPWMDInter pwi);
bool isMember(MD_NAT x, UnordPWMDInter pwi);
Util::MD_NAT minElem(UnordPWMDInter pwi);
Util::MD_NAT maxElem(UnordPWMDInter pwi);
UnordPWMDInter intersection(UnordPWMDInter pwi1, UnordPWMDInter pwi2);
UnordPWMDInter cup(UnordPWMDInter pwi1, UnordPWMDInter pwi2);

/** @function complementAtom
 *
 * @brief This function calculates the complement for an UnordPWMDInter with only
 * one piece.
 *
 * !!! Both complementAtom and complement return a collection of non-disjoint
 * pieces, because they are only used by difference. These functions should
 * NOT be called by any other function.
 */

UnordPWMDInter complementAtom(UnordPWMDInter mdi);
UnordPWMDInter complement(UnordPWMDInter pwi);

UnordPWMDInter difference(UnordPWMDInter pwi1, UnordPWMDInter pwi2);

/**
 * @brief Extra operations.
 */

/** @function concatenation
 *
 * @brief Function useful to unite two pwis in the case these are known to be
 * disjoint.
 */
UnordPWMDInter concatenation(UnordPWMDInter pwi1, UnordPWMDInter pwi2);
UnordPWMDInter filterSet(bool (*f)(SetPiece), UnordPWMDInter pwi);

UnordPWMDInter offset(Util::MD_NAT off, UnordPWMDInter);

std::size_t hash_value(const UnordPWMDInter &pwi);

typedef UnordPWMDInter UnordSet;

} // namespace LIB

}  // namespace SBG

#endif
