/** @file dom_ord_pwmap.hpp

 @brief <b>Domain Ordered PWMap Implementation</b>

 Domain ordered and map ordered piecewise map implementation. The evaluator will
 be in charge of checking that the choosen set implementation is ordered when
 this structure is used.

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

#ifndef SBG_DOM_ORD_PWMAP_HPP
#define SBG_DOM_ORD_PWMAP_HPP

#include "sbg/ord_pwmap.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Domain Ordered PWMap Implementation (concrete strategy) ---------------------
////////////////////////////////////////////////////////////////////////////////

struct DomOrdPWMap : public OrdPWMap {
  using SetPerimeter = OrdPWMap::SetPerimeter;
  using MapEntry = OrdPWMap::MapEntry;
  using OrdMapCollection = OrdPWMap::OrdMapCollection;

  member_class(OrdMapCollection, pieces);

  ~DomOrdPWMap() = default;
  DomOrdPWMap();
  DomOrdPWMap(const Set& s);
  DomOrdPWMap(const Map& m);
  DomOrdPWMap(const OrdMapCollection& pieces);
  DomOrdPWMap(const DomOrdPWMap& pw);

  struct Iterator : public PWMapStrategy::Iterator {
    member_class(OrdMapCollection::const_iterator, it);

    Iterator(OrdMapCollection::const_iterator it);
    void operator++() override;
    bool operator!=(const PWMapStrategy::Iterator& other) const override;
    const Map& operator*() const override;
  };

  std::shared_ptr<PWMapStrategy::Iterator> begin() const override;
  std::shared_ptr<PWMapStrategy::Iterator> end() const override;

  void emplaceBack(const Map& m) override;

  bool operator==(const PWMapStrategy& other) const override;
  bool operator!=(const PWMapStrategy& other) const override;
  DomOrdPWMap& operator=(DomOrdPWMap&& other);
  std::ostream& print(std::ostream& out) const override;

  PWMapStratPtr operator+(const PWMapStrategy& other) const override;

  PWMapStratPtr clone() const override;

  // Traditional map operations ------------------------------------------------

  std::size_t arity() const override;
  bool isEmpty() const override;
  Set dom() const override;
  PWMapStratPtr restrict(const Set& subdom) const override;
  Set image() const override;
  Set image(const Set& subdom) const override;
  Set preImage(const Set& subcodom) const override;
  PWMapStratPtr inverse() const override;
  PWMapStratPtr composition(const PWMapStrategy& pw2) const override;

  PWMapStratPtr mapInf(unsigned int n) const override;
  PWMapStratPtr mapInf() const override;
  Set fixedPoints() const override;

  // Extra operations ----------------------------------------------------------

  PWMapStratPtr concatenation(const PWMapStrategy& other) const override;
  PWMapStratPtr combine(const PWMapStrategy& other) const override;
  PWMapStratPtr reduce(const Interval& i, const LExp& e) const override;
  PWMapStratPtr reduce(const Map& sbgmap) const override;
  PWMapStratPtr reduce() const override;

  PWMapStratPtr minMap(const PWMapStrategy& other) const override;
  PWMapStratPtr minAdjMap(const PWMapStrategy& other) const override;

  PWMapStratPtr firstInv(const Set& subdom) const override;
  PWMapStratPtr firstInv() const override;

  PWMapStratPtr filterMap(bool (*f)(const Map& )) const override;

  Set equalImage(const PWMapStrategy& other) const override;
  Set lessImage(const PWMapStrategy& other) const override;
  Set sharedImage() const override;

  PWMapStratPtr offsetDom(const MD_NAT& off) const override;
  PWMapStratPtr offsetDom(const PWMapStrategy& off) const override;
  PWMapStratPtr offsetImage(const MD_NAT& off) const override;
  PWMapStratPtr offsetImage(const Exp& off) const override;

  PWMapStratPtr compact() const override;
  
  private:
  /**
   * @brief Calculates the minAdjMap core, which contains the entire main process
   * of the function.
   */
  void processMinAdjMap(
  const Map& m1, 
  const Map& m2, 
  Set& set_in,
  Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT global_pos) const;
  
  /**
   * @brief Calculates the minus core, which contains the entire main process of
   * the function.
   */
  void processMinus(
  const Map& m1, 
  const Map& m2, 
  Set& set_in,
  Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT global_pos) const; 
  
  /**
   * @brief Calculates the add core, which contains the entire main process of
   * the function.
   */
  void processAdd(
  const Map& m1, 
  const Map& m2, 
  Set& set_in,
  Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT global_pos) const; 
  
  /**
   * @brief Calculates the equalImage core, which contains the entire main
   * process of the function.
   */
  void processEqualImage(
  const Map& m1, 
  const Map& m2, 
  Set& set_in,
  Set& set_out,
  OrdMapCollection& ord_pwmap,
  NAT global_pos) const; 
  
  /**
   * @brief Type used in the 'processMapsOrd' declaration to reduce its size.
   * This type is the same as the process functions above.
   */
  using ProcessFunc = void (DomOrdPWMap::*)(
  const Map& , const Map& , 
  Set& , Set& , OrdMapCollection&, 
  NAT
  ) const;
  
  /**
   * @brief Provides an efficient method for processing two ordered piecewise
   * maps.
   */
  void processMapsOrd(
    const PWMapStrategy& other,
    Set& set_in,
    Set& set_out,
    OrdMapCollection& ord_pwmap,
    ProcessFunc process,
    bool order_mts
  ) const;
};

typedef const DomOrdPWMap& DomOrdPWMapCRef;
typedef DomOrdPWMap& DomOrdPWMapRef;
typedef std::unique_ptr<DomOrdPWMap> DomOrdPWMapPtr;

} // namespace LIB

} // namespace SBG

#endif
