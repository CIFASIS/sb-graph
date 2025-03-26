/** @file af_set.hpp

 @brief <b>Set Abstract Factory</b>

 While developing the SBG library several implementations were proposed. First,
 two implementations were proposed for sets: unordered and ordered sets. For
 this reason, it was decided that delegation should be used to provide
 different implementations, and also an abstract factory was needed to create
 objects with the desired implementation.

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

#ifndef SBG_AF_SET_HPP
#define SBG_AF_SET_HPP

#include "set.hpp"

namespace SBG {

namespace LIB {

struct SetAF {
  virtual ~SetAF() = default;

  virtual Set createSet() const = 0;
  virtual Set createSet(const MD_NAT &x) const = 0;
  virtual Set createSet(const Interval &i) const = 0;
  virtual Set createSet(const SetPiece &mdi) const = 0;
};

struct UnordAF : public SetAF {
  Set createSet() const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;
};

struct OrdDenseAF : public SetAF {
  Set createSet()  const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;
};

struct OrdAF : public SetAF {
  Set createSet()  const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;
};

} // namespace LIB

}  // namespace SBG

#endif
