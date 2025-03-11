/** @file af_map.hpp

 @brief <b>PWMap Abstract Factory</b>

 Currently only one implementation of PWMaps is supported: unordered. In the
 future an ordered implementation might be added.

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

#ifndef SBG_AF_PWMAP_HPP
#define SBG_AF_PWMAP_HPP

#include "pw_map.hpp"

namespace SBG {

namespace LIB {

struct PWMapAF {
  virtual ~PWMapAF() = default;

  virtual Set createSet() const = 0;
  virtual Set createSet(const MD_NAT &x) const = 0;
  virtual Set createSet(const Interval &i) const = 0;
  virtual Set createSet(const SetPiece &mdi) const = 0;

  virtual Map createMap() const = 0;
  virtual Map createMap(MD_NAT x, Exp exp) const = 0;
  virtual Map createMap(Interval i, LExp le) const = 0;
  virtual Map createMap(SetPiece mdi, Exp exp) const = 0;
  virtual Map createMap(Set s, Exp exp) const = 0;

  virtual PWMap createPWMap() const = 0;
  virtual PWMap createPWMap(const Set &s) const = 0;
  virtual PWMap createPWMap(const Map &m) const = 0;
};

struct UnordPWMapAF : public PWMapAF {
  private:
  const MapAF &map_fact_;

  public:
  UnordPWMapAF(const MapAF &map_fact);

  Set createSet() const override;
  Set createSet(const MD_NAT &x) const override;
  Set createSet(const Interval &i) const override;
  Set createSet(const SetPiece &mdi) const override;

  Map createMap() const override;
  Map createMap(MD_NAT x, Exp exp) const override;
  Map createMap(Interval i, LExp le) const override;
  Map createMap(SetPiece mdi, Exp exp) const override;
  Map createMap(Set s, Exp exp) const override;

  PWMap createPWMap() const;
  PWMap createPWMap(const Set &s) const;
  PWMap createPWMap(const Map &m) const;
};

} // namespace LIB

}  // namespace SBG

#endif
