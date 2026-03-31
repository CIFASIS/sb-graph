/** @file pwmap_fact.hpp

 @brief <b>PWMap Factory</b>

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

#ifndef SBGRAPH_SBG_PWMAP_FACT_HPP_
#define SBGRAPH_SBG_PWMAP_FACT_HPP_

#include "sbg/map.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

#define PWMAP_FACT PWMapFactory::instance()

class UnordPWMapFact {
public:
  UnordPWMapFact();

  PWMap createPWMap() const;
  PWMap createPWMap(const Set& s) const;
  PWMap createPWMap(const Map& m) const;
};

class OrdPWMapFact {
public:
  OrdPWMapFact();

  PWMap createPWMap() const;
  PWMap createPWMap(const Set& s) const;
  PWMap createPWMap(const Map& m) const;
};

//class DomOrdPWMapFact {
//  public:
//  DomOrdPWMapFact();
//
//  PWMap createPWMap() const override;
//  PWMap createPWMap(const Set& s) const override;
//  PWMap createPWMap(const Map& m) const override;
//};

/**
 * @brief Single instance of pw factory to be used by clients in need of
 * creating pws. A client includes this file and calls
 * PW_FACT.createPWMap(args).
 */
class PWMapFactory {
public:
  ~PWMapFactory() = default;

  static PWMapFactory& instance();
  const PWMapKind& kind() const;  

  void set_pwmap_fact(PWMapKind kind);

  PWMap createPWMap() const;
  PWMap createPWMap(Set s) const;
  PWMap createPWMap(Map m) const;

private:
  using FactImpl = std::variant<UnordPWMapFact, OrdPWMapFact>;

  PWMapFactory();

  PWMapKind _kind;
  FactImpl _impl;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_PWMAP_FACT_HPP_
