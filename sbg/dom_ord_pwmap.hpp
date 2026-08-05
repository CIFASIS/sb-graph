/** @file dom_ord_pwmap.hpp

 @brief <b>Domain Ordered PWMap Implementation</b>

 Ordered piecewise implementation, that assumes an ordered set implementation.
 The evaluator will be in charge of checking that the choosen set
 implementation is ordered when this structure is used.

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

#ifndef SBGRAPH_SBG_DOM_ORD_PWMAP_HPP_
#define SBGRAPH_SBG_DOM_ORD_PWMAP_HPP_

#include "sbg/map.hpp"
#include "sbg/map_entry.hpp"
#include "sbg/set.hpp"

#include "rapidjson/document.h"

#include <vector>
#include <iosfwd>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Domain ordered PWMap Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

class DomOrdPWMap {
public:
  using OrdMapCollection = std::vector<MapEntry>;
  using ConstIt = OrdMapCollection::const_iterator;

  DomOrdPWMap();
  DomOrdPWMap(const Set& s);
  DomOrdPWMap(const Map& m);
  DomOrdPWMap(const std::vector<Map>& pieces);
  DomOrdPWMap(const OrdMapCollection& pieces);
  DomOrdPWMap(OrdMapCollection&& pieces);

  ConstIt begin() const;
  ConstIt end() const;

  template<typename... Args>
  void emplace(Args&&... args);
  void insert(const Map& m);
  void insert(Map&& m);

  bool operator==(const DomOrdPWMap& other) const;
  bool operator!=(const DomOrdPWMap& other) const;
  DomOrdPWMap operator+(const DomOrdPWMap& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional map operations ------------------------------------------------

  std::size_t arity() const;
  bool isEmpty() const;
  Set domain() const &;
  Set domain() &&;
  DomOrdPWMap restrict(const Set& subdom) const;
  Set image() const;
  Set image(const Set& subdom) const;
  Set preImage(const Set& subcodom) const;
  DomOrdPWMap inverse() const;
  DomOrdPWMap composition(const DomOrdPWMap& other) const;

  DomOrdPWMap mapInf() const;
  Set fixedPoints() const;

  // Extra operations ----------------------------------------------------------

  DomOrdPWMap concatenation(const DomOrdPWMap& other) const &;
  DomOrdPWMap concatenation(const DomOrdPWMap& other) &&;
  DomOrdPWMap concatenation(DomOrdPWMap&& other) const &;
  DomOrdPWMap concatenation(DomOrdPWMap&& other) &&;
  DomOrdPWMap combine(const DomOrdPWMap& other) const &;
  DomOrdPWMap combine(const DomOrdPWMap& other) &&;
  DomOrdPWMap combine(DomOrdPWMap&& other) const &;
  DomOrdPWMap combine(DomOrdPWMap&& other) &&;

  DomOrdPWMap min(const DomOrdPWMap& other) const;
  DomOrdPWMap minAdj(const DomOrdPWMap& other) const;

  Set sharedImage() const;
  Set equalImage(const DomOrdPWMap& other) const;
  Set lessImage(const DomOrdPWMap& other) const;

  DomOrdPWMap imageMultiplicity() const;

  void compact();

private:
  template<typename... Args>
  void emplaceBack(Args&&... args);
  void pushBack(const Map& m);
  void pushBack(Map&& m);
  void pushBack(const MapEntry& entry);
  void pushBack(MapEntry&& entry);

  void insertHint(std::size_t hint, const Map& m);
  void insertHint(std::size_t hint, Map&& m);

  /**
   * @brief Advances the hint to point the next element that has jth_entry as
   * its minimum in its perimeter.
   */
  std::size_t advanceHint(std::size_t hint, const MapEntry& jth_entry);

  /**
   * @brief Calculates (if possible) compactly the result of mapInf.\n
   *
   * Currently, the only expressions that can be efficiently reduced are:
   *   - x+h
   *   - x-h
   */
  DomOrdPWMap reduce() const;

  /*
   * @brief First compose the pw with itself \p n times, obtaining pw'. Then,
   * compose pw' with itself up to convergence.
   */
  DomOrdPWMap mapInf(unsigned int n) const;

  OrdMapCollection _pieces;

  friend class AddCore;
  friend class MinAdjCore;
  template<typename T, typename SetImplT>
  friend class RestrictCore;
  friend class PWMapAccessKey;
};

// Template definitions --------------------------------------------------------

template<typename... Args>
inline void DomOrdPWMap::emplace(Args&&... args)
{
  auto&& domain = std::get<0>(std::forward_as_tuple(args...));
  if (!domain.isEmpty()) {
    insert(Map{std::forward<Args>(args)...});
  }
}

template<typename... Args>
inline void DomOrdPWMap::emplaceBack(Args&&... args)
{
  auto&& domain = std::get<0>(std::forward_as_tuple(args...));
  if (!domain.isEmpty()) {
    _pieces.emplace_back(std::forward<Args>(args)...);
  }
}

template<typename OrdCollection1, typename OrdCollection2, typename Core>
Core traverse(const OrdCollection1& lhs, const OrdCollection2& rhs, Core op);

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(DomOrdPWMap pw
  , rapidjson::Document::AllocatorType& alloc);

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_DOM_ORD_PWMAP_HPP_
