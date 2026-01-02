/** @file unord_pwmap.hpp

 @brief <b>Unordered PWMap Implementation</b>

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

#ifndef SBG_UNORD_PWMAP_HPP
#define SBG_UNORD_PWMAP_HPP

#include "sbg/pw_map.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Unordered PWMap Implementation (concrete strategy) --------------------------
////////////////////////////////////////////////////////////////////////////////

class UnordPWMap : public PWMapStrategy {
  public:
  using UnordMapCollection = std::vector<Map>;

  member_class(UnordMapCollection, pieces);

  ~UnordPWMap() = default;
  UnordPWMap();
  UnordPWMap(const Set &s);
  UnordPWMap(const Map &m);
  UnordPWMap(const UnordMapCollection& pieces);

  class Iterator : public PWMapStrategy::Iterator {
    member_class(UnordMapCollection::const_iterator, it);

    Iterator(UnordMapCollection::const_iterator it);
    void operator++() override;
    bool operator!=(const PWMapStrategy::Iterator &other) const override;
    const Map &operator*() const override;
  };

  std::shared_ptr<PWMapStrategy::Iterator> begin() const override;
  std::shared_ptr<PWMapStrategy::Iterator> end() const override;

  void emplaceBack(const Map &m) override;

  bool operator==(const PWMapStrategy &other) const override;
  bool operator!=(const PWMapStrategy &other) const override;
  UnordPWMap &operator=(UnordPWMap &&other);
  std::ostream &print(std::ostream &out) const override;

  PWMapStratPtr operator+(const PWMapStrategy &other) const override;

  PWMapStratPtr clone() const override;

  // Traditional map operations ------------------------------------------------

  std::size_t arity() const override;
  bool isEmpty() const override;
  Set dom() const override;
  PWMapStratPtr restrict(const Set &subdom) const override;
  Set image() const override;
  Set image(const Set &subdom) const override;
  Set preImage(const Set &subcodom) const override;
  PWMapStratPtr inverse() const override;
  PWMapStratPtr composition(const PWMapStrategy &pw2) const override;

  PWMapStratPtr mapInf(unsigned int n) const override;
  PWMapStratPtr mapInf() const override;
  Set fixedPoints() const override;

  // Extra operations ----------------------------------------------------------

  PWMapStratPtr concatenation(const PWMapStrategy &other) const override;
  PWMapStratPtr combine(const PWMapStrategy &other) const override;
  PWMapStratPtr reduce() const override;

  PWMapStratPtr minMap(const PWMapStrategy &other) const override;
  PWMapStratPtr minAdjMap(const PWMapStrategy &other) const override;

  PWMapStratPtr firstInv(const Set &subdom) const override;
  PWMapStratPtr firstInv() const override;

  PWMapStratPtr filterMap(bool (*f)(const Map &)) const override;

  Set equalImage(const PWMapStrategy &other) const override;
  Set lessImage(const PWMapStrategy& other) const override;
  Set sharedImage() const override;

  PWMapStratPtr offsetDom(const MD_NAT &off) const override;
  PWMapStratPtr offsetDom(const PWMapStrategy &off) const override;
  PWMapStratPtr offsetImage(const MD_NAT &off) const override;
  PWMapStratPtr offsetImage(const Exp &off) const override;

  PWMapStratPtr compact() const override;
};

typedef const UnordPWMap &UnordPWMapCRef;
typedef std::unique_ptr<UnordPWMap> UnordPWMapPtr;

} // namespace LIB

} // namespace SBG

#endif
