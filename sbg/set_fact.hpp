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

#ifndef SBG_SET_FACT_HPP
#define SBG_SET_FACT_HPP

#include "set.hpp"

namespace SBG {

namespace LIB {

struct SetFact {
  virtual ~SetFact() = default;

  virtual Set createSet() const = 0;
  virtual Set createSet(const MD_NAT &x) const = 0;
  virtual Set createSet(const Interval &i) const = 0;
  virtual Set createSet(const SetPiece &mdi) const = 0;
};

struct UnordSetFact : public SetFact {
  Set createSet() const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;
};

struct OrdUnidimDenseSetFact : public SetFact {
  Set createSet()  const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;
};

struct OrdSetFact : public SetFact {
  Set createSet()  const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;
};

} // namespace LIB

}  // namespace SBG

#endif
