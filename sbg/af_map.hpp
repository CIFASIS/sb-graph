/** @file af_map.hpp

 @brief <b>Map Abstract Factory</b>

 It was proposed to try implementing SBG maps with "atomic" domains (i.e.,
 the domain is a described with a mdi). For this reason, it was decided that
 delegation should be used to provide different implementations, and also an
 abstract factory was needed to create objects with the desired implementation.

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

#ifndef SBG_AF_MAP_HPP
#define SBG_AF_MAP_HPP

#include "map.hpp"

namespace SBG {

namespace LIB {

struct MapAF {
  virtual ~MapAF() = default;

  virtual Map createMap() const = 0;
  virtual Map createMap(Util::MD_NAT x, Exp exp) const = 0;
  virtual Map createMap(Interval i, LExp le) const = 0;
  virtual Map createMap(SetPiece mdi, Exp exp) const = 0;
  virtual Map createMap(Set s, Exp exp) const = 0;
};

struct MapSetAF : public MapAF {
  private:
  const SetAF &set_fact_;

  public:
  MapSetAF(const SetAF &set_fact);

  Map createMap() const override;
  Map createMap(Util::MD_NAT x, Exp exp) const override;
  Map createMap(Interval i, LExp le) const override;
  Map createMap(SetPiece mdi, Exp exp) const override;
  Map createMap(Set s, Exp exp) const override;
};

} // namespace LIB

}  // namespace SBG

#endif
