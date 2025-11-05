/** @file ord_unidim_dense_set.hpp

 @brief <b>Ordered Unidimensional Dense Set Implementation</b>

 A dense set is composed only by dense intervals, i.e. intervals whose step is
 equal to one.

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

#ifndef SBG_ORD_UNIDIM_DENSE_SET_HPP
#define SBG_ORD_UNIDIM_DENSE_SET_HPP

#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Ordered Unidimensional Dense Set Implementation (concrete strategy) ---------
////////////////////////////////////////////////////////////////////////////////

struct OrdUnidimDenseSet : public SetStrategy {
  using MDIOrdCollection = std::vector<SetPiece>;

  member_class(MDIOrdCollection, pieces);

  ~OrdUnidimDenseSet();
  OrdUnidimDenseSet();
  OrdUnidimDenseSet(MD_NAT x);
  OrdUnidimDenseSet(Interval i);
  OrdUnidimDenseSet(SetPiece mdi);
  OrdUnidimDenseSet(MDIOrdCollection pieces);

  SetStratPtr clone() const override;

  struct Iterator : public SetStrategy::Iterator {
    member_class(MDIOrdCollection::const_iterator, it);

    Iterator(MDIOrdCollection::const_iterator it);
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
   * @brief Performs operation f between a piece of s1 and a piece of s2. At the
   * start begins with both minimum elements, and advances the iterator of the
   * set with the piece that has the minimum end. This is repeated until one of
   * the two collections is consumed.
   */
  MDIOrdCollection boundedTraverse(SetPiece (SetPiece::*f)(const SetPiece &) const
    , const MDIOrdCollection &other) const;

  /**
   * @brief Performs operation f between a piece of s1 and a piece of s2. At the
   * start begins with both minimum elements, and advances the iterator of the
   * set with the piece that has the minimum end. This is repeated until one of
   * the two collections is consumed. Then, all the remaining pieces of the
   * other set are also inserted.
   */
  MDIOrdCollection traverse(SetPiece (SetPiece::*f)(const SetPiece &) const
    , const MDIOrdCollection &other) const;
};

typedef const OrdUnidimDenseSet &OrdUnidimDenseSetCRef;

} // namespace LIB

}  // namespace SBG

#endif
