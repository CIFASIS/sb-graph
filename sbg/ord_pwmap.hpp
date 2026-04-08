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

#ifndef SBGRAPH_SBG_ORD_PWMAP_HPP_
#define SBGRAPH_SBG_ORD_PWMAP_HPP_

#include "sbg/map.hpp"
#include "sbg/map_entry.hpp"
#include "sbg/set.hpp"

#include <vector>
#include <iosfwd>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Ordered PWMap Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class OrdPWMap {
public:
  using OrdMapCollection = std::vector<MapEntry>;
  using ConstIt = OrdMapCollection::const_iterator;

  OrdPWMap();
  OrdPWMap(const Set& s);
  OrdPWMap(const Map& m);
  OrdPWMap(const OrdMapCollection& pieces);
  OrdPWMap(OrdMapCollection&& pieces);

  ConstIt begin() const;
  ConstIt end() const;

  template<typename... Args>
  void emplace(Args&&... args);
  void insert(const Map& m);
  void insert(Map&& m);

  bool operator==(const OrdPWMap& other) const;
  bool operator!=(const OrdPWMap& other) const;
  OrdPWMap operator+(const OrdPWMap& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional map operations ------------------------------------------------

  std::size_t arity() const;
  bool isEmpty() const;
  Set domain() const &;
  Set domain() &&;
  OrdPWMap restrict(const Set& subdom) const;
  Set image() const;
  Set image(const Set& subdom) const;
  Set preImage(const Set& subcodom) const;
  OrdPWMap inverse() const;
  OrdPWMap composition(const OrdPWMap& other) const;

  OrdPWMap mapInf() const;
  Set fixedPoints() const;

  // Extra operations ----------------------------------------------------------

  OrdPWMap concatenation(const OrdPWMap& other) const &;
  OrdPWMap concatenation(const OrdPWMap& other) &&;
  OrdPWMap concatenation(OrdPWMap&& other) const &;
  OrdPWMap concatenation(OrdPWMap&& other) &&;
  OrdPWMap combine(const OrdPWMap& other) const &;
  OrdPWMap combine(const OrdPWMap& other) &&;
  OrdPWMap combine(OrdPWMap&& other) const &;
  OrdPWMap combine(OrdPWMap&& other) &&;

  OrdPWMap min(const OrdPWMap& other) const;
  OrdPWMap minAdj(const OrdPWMap& other) const;

  Set sharedImage() const;
  Set equalImage(const OrdPWMap& other) const;
  Set lessImage(const OrdPWMap& other) const;

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
  OrdPWMap reduce() const;

  /*
   * @brief First compose the pw with itself \p n times, obtaining pw'. Then,
   * compose pw' with itself up to convergence.
   */
  OrdPWMap mapInf(unsigned int n) const;

  template<typename Core>
  Core traverse(const OrdPWMap& other, Core op) const;

  OrdMapCollection _pieces;

  friend class AddCore;
  friend class MinAdjCore;
  friend class PWMapAccessKey;
};

template<typename... Args>
inline void OrdPWMap::emplace(Args&&... args)
{
  insert(Map{std::forward<Args>(args)...});
}

template<typename... Args>
inline void OrdPWMap::emplaceBack(Args&&... args)
{
  _pieces.emplace_back(std::forward<Args>(args)...);
}

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_ORD_PWMAP_HPP_
