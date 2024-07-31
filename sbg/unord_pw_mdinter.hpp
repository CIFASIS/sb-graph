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

#include <functional>
#include <unordered_set>

#include "sbg/multidim_inter.hpp"

namespace std {
  template <> struct hash<SBG::LIB::Interval>
  {
    size_t operator()(const SBG::LIB::Interval &i) const
    {
      return std::hash<SBG::Util::NAT>()(i.begin())
        ^ std::hash<SBG::Util::NAT>()(i.step())
        ^ std::hash<SBG::Util::NAT>()(i.end());
    }

  };

  template <> struct hash<SBG::LIB::SetPiece>
  {
    size_t operator()(const SBG::LIB::SetPiece &mdi) const
    {
      size_t res = 0;

      for (const SBG::LIB::Interval &i : mdi)
        res = res ^ std::hash<SBG::LIB::Interval>()(i);

      return res;
    }
  };
}

namespace SBG {

namespace LIB {

// Container -------------------------------------------------------------------

typedef std::unordered_set<SetPiece> MDInterUnordSet;
typedef MDInterUnordSet::iterator MDInterUnordSetIt;
typedef MDInterUnordSet::const_iterator MDInterUnordSetConstIt;
std::ostream &operator<<(std::ostream &out, const MDInterUnordSet &ii);

// Non-optimized implementation ------------------------------------------------

struct UnordPWMDInter {
  member_class(MDInterUnordSet, pieces);

  UnordPWMDInter();
  UnordPWMDInter(Util::MD_NAT x);
  UnordPWMDInter(Interval i);
  UnordPWMDInter(SetPiece mdi);
  UnordPWMDInter(MDInterUnordSet c);

  typedef MDInterUnordSetIt iterator;
  typedef MDInterUnordSetConstIt const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  std::size_t size() const;
  void emplace(SetPiece mdi);
  void emplaceBack(SetPiece mdi);

  bool operator==(const UnordPWMDInter &other) const;
  bool operator!=(const UnordPWMDInter &other) const;
  bool operator<(const UnordPWMDInter &other) const;

  /**
   * @brief Traditional set operations.
   */
  unsigned int cardinal() const;
  bool isEmpty() const;
  Util::MD_NAT minElem() const;
  Util::MD_NAT maxElem() const;
  UnordPWMDInter intersection(const UnordPWMDInter &other) const;
  UnordPWMDInter cup(const UnordPWMDInter &other) const;
  UnordPWMDInter difference(const UnordPWMDInter &other) const;

  /**
   * @brief Extra operations.
   */

  std::size_t arity() const;
  /** @function concatenation
   *
   * @brief Function useful to unite two pwis in the case these are known to be
   * disjoint.
   */
  UnordPWMDInter concatenation(const UnordPWMDInter &other) const;
  UnordPWMDInter filterSet(bool (*f)(const SetPiece &mdi)) const;
  UnordPWMDInter offset(const Util::MD_NAT &off) const;
  UnordPWMDInter compact() const;

  private:
  /** @function complementAtom
   *
   * @brief This function calculates the complement for an UnordPWMDInter with only
   * one piece.
   *
   * !!! Both complementAtom and complement return a collection of non-disjoint
   * pieces, because they are only used by difference. These functions should
   * NOT be called by any other function.
   */
  UnordPWMDInter complementAtom() const;
  UnordPWMDInter complement() const;
};
std::ostream &operator<<(std::ostream &out, const UnordPWMDInter &pwi);

typedef UnordPWMDInter UnordSet;

} // namespace LIB

}  // namespace SBG

#endif
