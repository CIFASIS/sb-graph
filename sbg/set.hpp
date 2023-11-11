/** @file set.hpp

 @brief <b>Set</b>

 Set is a wrapper for all possible pwi implementations

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

#ifndef SBG_SET_HPP
#define SBG_SET_HPP

#include "sbg/ord_pw_mdinter.hpp"
#include "sbg/unord_pw_mdinter.hpp"

namespace SBG {

namespace LIB {

typedef std::variant<UnordSet, OrdSet> SetType;

struct Set {
  member_class(SetType, pieces);

  Set(UnordSet c);
  Set(OrdSet c);

  std::size_t size();
  void emplace(SetPiece mdi);
  SetPiece operator[](std::size_t n);

  eq_class(Set);
  lt_class(Set);
};
std::ostream &operator<<(std::ostream &out, const Set &s);
void foreach(std::function<void (SetPiece)>, Set s);

/**
 * @brief Traditional set operations.
 */

unsigned int cardinal(Set s);
bool isEmpty(Set s);
bool isMember(MD_NAT x, Set s);
Util::MD_NAT minElem(Set s);
Util::MD_NAT maxElem(Set s);
Set intersection(Set s1, Set s2);
Set cup(Set s1, Set s2);

Set difference(Set s1, Set s2);

/**
 * @brief Extra operations.
 */

/** @function concatenation
 *
 * @brief Function useful to unite two s in the case these are known to be
 * disjoint.
 */
Set concatenation(Set s1, Set s2);
Set filterSet(bool (*f)(SetPiece), Set s);

Set offset(Util::MD_NAT off, Set);

std::size_t hash_value(const Set &s);

} // namespace LIB

}  // namespace SBG

#endif
