/** @file af_map.hpp

 @brief <b>Map Factory</b>

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

struct MapAF : public SetAF {
  private:
  const SetAF &set_fact_;

  public:
  MapAF(const SetAF &set_fact);

  Set createSet() const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;

  Map createMap() const;
  Map createMap(MD_NAT x, Exp exp) const;
  Map createMap(Interval i, LExp le) const;
  Map createMap(SetPiece mdi, Exp exp) const;
  Map createMap(Set s, Exp exp) const;
};

} // namespace LIB

}  // namespace SBG

#endif
