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

#ifndef SBGRAPH_SBG_UNORD_PWMAP_HPP_
#define SBGRAPH_SBG_UNORD_PWMAP_HPP_

#include "sbg/map.hpp"
#include "sbg/set.hpp"

#include "rapidjson/document.h"

#include <iosfwd>
#include <vector>
#include <tuple>

namespace SBG {

namespace LIB {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Unordered PWMap Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class UnordPWMap {
public:
  using UnordMapCollection = std::vector<Map>;
  using ConstIt = UnordMapCollection::const_iterator;

  UnordPWMap();
  UnordPWMap(const Set& s);
  UnordPWMap(const Map& m);
  UnordPWMap(const UnordMapCollection& pieces);
  UnordPWMap(UnordMapCollection&& pieces);

  ConstIt begin() const;
  ConstIt end() const;

  template<typename... Args>
  void emplace(Args&&... args);
  void insert(const Map& m);
  void insert(Map&& m);

  bool operator==(const UnordPWMap& other) const;
  bool operator!=(const UnordPWMap& other) const;
  UnordPWMap operator+(const UnordPWMap& other) const;
  std::ostream& print(std::ostream& out) const;

  // Traditional map operations ------------------------------------------------

  std::size_t arity() const;
  bool isEmpty() const;
  Set domain() const &;
  Set domain() &&;
  UnordPWMap restrict(const Set& subdom) const;
  Set image() const;
  Set image(const Set& subdom) const;
  Set preImage(const Set& subcodom) const;
  UnordPWMap inverse() const;
  UnordPWMap composition(const UnordPWMap& other) const;

  UnordPWMap mapInf() const;
  Set fixedPoints() const;

  // Extra operations ----------------------------------------------------------

  UnordPWMap concatenation(const UnordPWMap& other) const &;
  UnordPWMap concatenation(const UnordPWMap& other) &&;
  UnordPWMap concatenation(UnordPWMap&& other) const &;
  UnordPWMap concatenation(UnordPWMap&& other) &&;
  UnordPWMap combine(const UnordPWMap& other) const &;
  UnordPWMap combine(const UnordPWMap& other) &&;
  UnordPWMap combine(UnordPWMap&& other) const &;
  UnordPWMap combine(UnordPWMap&& other) &&;

  UnordPWMap min(const UnordPWMap& other) const;
  UnordPWMap minAdj(const UnordPWMap& other) const;

  Set sharedImage() const;
  Set equalImage(const UnordPWMap& other) const;
  Set lessImage(const UnordPWMap& other) const;

  UnordPWMap imageMultiplicity() const;

  void compact();

private:
  template<typename... Args>
  void emplaceBack(Args&&... args);
  void pushBack(const Map& m);
  void pushBack(Map&& m);

  /**
   * @brief Calculates (if possible) compactly the result of mapInf.\n
   *
   * Currently, the only expressions that can be efficiently reduced are:
   *   - x+h
   *   - x-h
   */
  UnordPWMap reduce() const;

  UnordMapCollection _pieces;

  friend class PWMapAccessKey;
};

// Template definitions --------------------------------------------------------

template<typename... Args>
inline void UnordPWMap::emplace(Args&&... args)
{
  auto&& domain = std::get<0>(std::forward_as_tuple(args...));
  if (!domain.isEmpty()) {
    _pieces.emplace_back(std::forward<Args>(args)...);
  }
}

template<typename... Args>
inline void UnordPWMap::emplaceBack(Args&&... args)
{
  auto&& domain = std::get<0>(std::forward_as_tuple(args...));
  if (!domain.isEmpty()) {
    _pieces.emplace_back(std::forward<Args>(args)...);
  }
}

// Non-member functions --------------------------------------------------------

rapidjson::Value toJSON(UnordPWMap pw
  , rapidjson::Document::AllocatorType& alloc);

} // namespace detail

} // namespace LIB

} // namespace SBG

#endif // SBGRAPH_SBG_UNORD_PWMAP_HPP_
