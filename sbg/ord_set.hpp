/** @file ord_set.hpp

 @brief <b>Ordered Set Implementation</b>

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

#ifndef SBG_ORD_SET_HPP
#define SBG_ORD_SET_HPP

#include "sbg/multidim_inter.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Ordered Set Implementation (concrete strategy) ------------------------------
////////////////////////////////////////////////////////////////////////////////

struct OrderedSet : public SetStrategy {
  using MDIOrdCollection = std::vector<SetPiece>;

  member_class(MDIOrdCollection, pieces);

  ~OrderedSet();
  OrderedSet();
  OrderedSet(MD_NAT x);
  OrderedSet(Interval i);
  OrderedSet(SetPiece mdi);
  OrderedSet(MDIOrdCollection pieces);

  SetStratPtr clone() const override;

  struct Iterator : public SetStrategy::Iterator {
    member_class(MDIOrdCollection::const_iterator, it);

    Iterator(MDIOrdCollection::const_iterator it);
    void operator++() override;
    bool operator!=(const SetStrategy::Iterator& other) const override;
    bool operator==(const SetStrategy::Iterator& other) const override;
    bool operator<(const SetStrategy::Iterator& other) const override;
    const SetPiece& operator*() const override;
  };

  std::shared_ptr<SetStrategy::Iterator> begin() const override;
  std::shared_ptr<SetStrategy::Iterator> end() const override;

  std::size_t size() const override;
  void emplace(const SetPiece& mdi) override;
  void emplaceBack(const SetPiece& mdi) override;

  bool operator==(const SetStrategy& other) const override;
  bool operator!=(const SetStrategy& other) const override;
  std::ostream& print(std::ostream& out) const override;

  // Traditional set operations ------------------------------------------------

  unsigned int cardinal() const override;
  bool isEmpty() const override;
  MD_NAT minElem() const override;
  MD_NAT maxElem() const override;
  SetStratPtr intersection(const SetStrategy& other) const override;
  SetStratPtr cup(const SetStrategy& other) const & override;
  SetStratPtr cup(SetStrategy&& other) && override;
  SetStratPtr complement() const;
  SetStratPtr difference(const SetStrategy& other) const override;

  // Extra operations ----------------------------------------------------------

  std::size_t arity() const override;
  SetStratPtr disjointCup(const SetStrategy& other) const & override;
  SetStratPtr disjointCup(SetStrategy&& other) && override;
  SetStratPtr filterSet(bool (*f)(const SetPiece& mdi)) const override;
  SetStratPtr offset(const MD_NAT& off) const override;
  SetStratPtr compact() const override;

  private:
  void emplaceHint(NAT hint, const SetPiece& mdi);

  NAT advanceHint(NAT hint, const SetPiece& mdi);

  /**
   * @brief Calculates the complement of an ordered set with a single piece.
   */
  SetStratPtr complementAtom() const;
  
  /**
   * @brief Computes the accumulated complement between an ordered set (this), 
   * which represents the complement of an ordered set, and an ordered set
   * (other), which represents the complement of an atomic ordered set.
   */
  void intersectionComp(const SetStrategy& other, const SetPiece& mdi
    , SetStrategy& rem);
};

typedef const OrderedSet& OrdSetCRef;
typedef OrderedSet& OrdSetRef;

} // namespace LIB

}  // namespace SBG

#endif
