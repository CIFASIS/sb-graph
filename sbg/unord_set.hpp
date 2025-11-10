/** @file unord_set.hpp

 @brief <b>Unordered Set Implementation</b>

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

#ifndef SBG_UNORD_SET_HPP
#define SBG_UNORD_SET_HPP

#include "sbg/multidim_inter.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Unordered Set Implementation (concrete strategy) ----------------------------
////////////////////////////////////////////////////////////////////////////////

struct UnorderedSet : public SetStrategy {
  using MDIUnordCollection = std::vector<SetPiece>;

  member_class(MDIUnordCollection, pieces);

  ~UnorderedSet();
  UnorderedSet();
  UnorderedSet(const MD_NAT &x);
  UnorderedSet(const Interval &i);
  UnorderedSet(const SetPiece &mdi);
  UnorderedSet(const MDIUnordCollection &pieces);

  SetStratPtr clone() const override;

  struct Iterator : public SetStrategy::Iterator {
    member_class(MDIUnordCollection::const_iterator, it);

    Iterator(MDIUnordCollection::const_iterator it);
    void operator++() override;
    bool operator!=(const SetStrategy::Iterator &other) const override;
    bool operator==(const SetStrategy::Iterator &other) const override;
    bool operator<(const SetStrategy::Iterator &other) const override;
    const SetPiece &operator*() const override;
  };

  std::shared_ptr<SetStrategy::Iterator> begin() const override;
  std::shared_ptr<SetStrategy::Iterator> end() const override;

  std::size_t size() const override;
  void emplace(const SetPiece &mdi) override;
  void emplaceBack(const SetPiece &mdi) override;

  bool operator==(const SetStrategy &other) const override;
  bool operator!=(const SetStrategy &other) const override;
  std::ostream &print(std::ostream &out) const override;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const override;
  bool isEmpty() const override;
  MD_NAT minElem() const override;
  MD_NAT maxElem() const override;
  SetStratPtr intersection(const SetStrategy &other) const override;
  SetStratPtr cup(const SetStrategy &other) const override;
  SetStratPtr complement() const;
  SetStratPtr difference(const SetStrategy &other) const override;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const override;
  SetStratPtr disjointCup(const SetStrategy &other) const override;
  SetStratPtr filterSet(bool (*f)(const SetPiece &mdi)) const override;
  SetStratPtr offset(const MD_NAT &off) const override;
  SetStratPtr compact() const override;

  private:
  /**
   * @brief Calculate the complement of an unordered set with a single piece.
   */
  SetStratPtr complementAtom() const;
};

typedef const UnorderedSet &UnordSetCRef;

} // namespace LIB

}  // namespace SBG

#endif
