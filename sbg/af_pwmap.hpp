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

struct PWMapAF : public MapAF {
  protected:
  const MapAF &map_fact_;

  public:
  virtual ~PWMapAF() = default;
  PWMapAF(const MapAF &map_fact);

  virtual PWMap createPWMap() const = 0;
  virtual PWMap createPWMap(const Set &s) const = 0;
  virtual PWMap createPWMap(const Map &m) const = 0;
};

struct UnordPWMapAF : public PWMapAF {
  public:
  UnordPWMapAF(const MapAF &map_fact);

  PWMap createPWMap() const override;
  PWMap createPWMap(const Set &s) const override;
  PWMap createPWMap(const Map &m) const override;
};

struct OrdPWMapAF : public PWMapAF {
  public:
  OrdPWMapAF(const MapAF &map_fact);

  PWMap createPWMap() const override;
  PWMap createPWMap(const Set &s) const override;
  PWMap createPWMap(const Map &m) const override;
};



} // namespace LIB

}  // namespace SBG

#endif
