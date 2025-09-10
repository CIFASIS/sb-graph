/** @file ord_pwmap.hpp

 @brief <b>Ordered PWMap Implementation</b>

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

#ifndef SBG_ORD_PWMAP_HPP
#define SBG_ORD_PWMAP_HPP

#include "sbg/pw_map.hpp"

namespace SBG {

namespace LIB {


////////////////////////////////////////////////////////////////////////////////
// Ordered PWMap Implementation (concrete strategy) ----------------------------
////////////////////////////////////////////////////////////////////////////////


struct OrdPWMap : public PWMapStrategy {
  using SetPerimeter = std::pair<MD_NAT, MD_NAT>;
  using MapEntry = std::pair<Map, SetPerimeter>;
  using OrdMapCollection = std::vector<MapEntry>;

  member_class(OrdMapCollection, pieces);

  ~OrdPWMap() = default;
  OrdPWMap(const MapFact &fact);
  OrdPWMap(const MapFact &fact, const Set &s);
  OrdPWMap(const MapFact &fact, const Map &m);
  OrdPWMap(const MapFact &fact, const OrdMapCollection &pieces);
  OrdPWMap(const OrdPWMap &pw);

  PWMapStratPtr clone() const override;

  struct Iterator : public PWMapStrategy::Iterator {
    member_class(OrdMapCollection::const_iterator, it);

    Iterator(OrdMapCollection::const_iterator it);
    void operator++() override;
    bool operator!=(const PWMapStrategy::Iterator &other) const override;
    const Map &operator*() const override;
  };

  std::shared_ptr<PWMapStrategy::Iterator> begin() const override;
  std::shared_ptr<PWMapStrategy::Iterator> end() const override;

  void emplaceBack(const Map &m) override;

  bool operator==(const PWMapStrategy &other) const override;
  bool operator!=(const PWMapStrategy &other) const override;
  OrdPWMap &operator=(OrdPWMap &&other);
  std::ostream &print(std::ostream &out) const override;

  PWMapStratPtr operator+(const PWMapStrategy &other) const override;
  PWMapStratPtr operator-(const PWMapStrategy &other) const override;

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

  // Extra operations ----------------------------------------------------------

  PWMapStratPtr concatenation(const PWMapStrategy &other) const override;
  PWMapStratPtr combine(const PWMapStrategy &other) const override;
  PWMapStratPtr reduce(const Interval &i, const LExp &e) const override;
  PWMapStratPtr reduce(const Map &sbgmap) const override;
  PWMapStratPtr reduce() const override;

  PWMapStratPtr minMap(const PWMapStrategy &other) const override;
  PWMapStratPtr minAdjMap(const PWMapStrategy &other) const override;

  PWMapStratPtr firstInv(const Set &subdom) const override;
  PWMapStratPtr firstInv() const override;

  PWMapStratPtr filterMap(bool (*f)(const Map &)) const override;

  Set equalImage(const PWMapStrategy &other) const override;
  Set sharedImage() const override;

  PWMapStratPtr offsetDom(const MD_NAT &off) const override;
  PWMapStratPtr offsetDom(const PWMapStrategy &off) const override;
  PWMapStratPtr offsetImage(const MD_NAT &off) const override;
  PWMapStratPtr offsetImage(const Exp &off) const override;

  PWMapStratPtr compact() const override;
  
  private:
  void processMinAdjMap(
  const Map &m1, 
  const Map &m2, 
  Set &set_in,
  Set &set_out, 
  OrdMapCollection  &ord_pwmap,
  NAT &global_pos) const;
  
  void processMinus(
  const Map &m1, 
  const Map &m2, 
  Set &set_in,
  Set &set_out, 
  OrdMapCollection  &ord_pwmap,
  NAT &global_pos) const; 
  
  void processAdd(
  const Map &m1, 
  const Map &m2, 
  Set &set_in,
  Set &set_out, 
  OrdMapCollection  &ord_pwmap,
  NAT &global_pos) const; 
  
  void processEqualImage(
  const Map &m1, 
  const Map &m2, 
  Set &set_in,
  Set &set_out,
  OrdMapCollection  &ord_pwmap,
  NAT &global_pos) const; 
  
  using ProcessFunc = void (OrdPWMap::*)(
  const Map &, const Map &, 
  Set &, Set &, OrdMapCollection &, 
  NAT&
  ) const;
  
  void processMapsOrd(
    const PWMapStrategy &other,
    Set &set_in,
    Set &set_out,
    OrdMapCollection  &ord_pwmap,
    ProcessFunc process,
    bool order_mts
  ) const;
};


typedef const OrdPWMap &OrdPWMapCRef;
typedef OrdPWMap &OrdPWMapRef;
typedef std::unique_ptr<OrdPWMap> OrdPWMapPtr;

} // namespace LIB

}  // namespace SBG

#endif
