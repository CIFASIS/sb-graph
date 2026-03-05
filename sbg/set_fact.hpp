/** @file set_fact.hpp

 @brief <b>Set Factory</b>

 As other structures will have to create SBG Sets (i.e., Maps and PWMaps), it
 is necessary to have a way to create SBG Sets objects with the desired
 implementation. This module provides a factory to such purpose.

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

#ifndef SBGRAPH_SBG_SET_FACT_HPP_
#define SBGRAPH_SBG_SET_FACT_HPP_

#include "sbg/interval.hpp"
#include "sbg/multidim_inter.hpp"
#include "sbg/natural.hpp"
#include "sbg/set.hpp"

#include <string>
#include <variant>

namespace SBG {

namespace LIB {

#define SET_FACT SetFactory::instance()

class UnordSetFact {
public:
  UnordSetFact();

  static Set createSet();
  static Set createSet(const MD_NAT& x);
  static Set createSet(const NAT lo, const NAT step, const NAT hi);
  static Set createSet(const FixedPointsInfo& info);
};

//class OrdSetFact {
//public:
//  Set createSet() const;
//  Set createSet(const MD_NAT& x) const;
//  Set createSet(const NAT lo, const NAT step, const NAT hi) const;
//  Set createSet(const FixedPointsInfo& mdi) const;
//};

class OrdUnidimDenseSetFact {
public:
  Set createSet() const;
  Set createSet(const MD_NAT& x) const;
  Set createSet(const NAT lo, const NAT step, const NAT hi) const;
  Set createSet(const FixedPointsInfo& info) const;
};

/**
 * @brief Single instance of set factory to be used by clients in need of
 * creating sets. A client includes this file and calls
 * SET_FACT.createSet(args).
 */
class SetFactory {
public:
  ~SetFactory() = default;

  static SetFactory& instance();

  void set_set_fact(SetKind kind);
  const SetKind& kind() const;

  Set createSet() const;
  Set createSet(const MD_NAT& x) const;
  Set createSet(const NAT lo, const NAT step, const NAT hi) const;
  Set createSet(const FixedPointsInfo& mdi) const;

private:
  using FactImpl = std::variant<UnordSetFact, OrdUnidimDenseSetFact>;

  SetFactory();

  SetKind _kind;
  FactImpl _impl;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_SET_FACT_HPP_ 
