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

#define MAP_FACT MapFactory::instance().getMapFactory() 

struct MapAF {
  public:
  MapAF();

  Map createMap() const;
  Map createMap(MD_NAT x, Exp exp) const;
  Map createMap(Interval i, LExp le) const;
  Map createMap(SetPiece mdi, Exp exp) const;
  Map createMap(Set s, Exp exp) const;
};

/**
 * @brief Single instance of set factory to be used by clients in need of
 * creating maps. A client includes this file and calls
 * MAP_FACT.createMap(args).
 */
class MapFactory {
  public:
  ~MapFactory() = default;
  static MapFactory& instance() {
    static MapFactory instance_;
    return instance_;
  }

  MapAF& getMapFactory();
  void setMapFactory(std::unique_ptr<MapAF> map_fact);

  private:
  MapFactory();
  std::unique_ptr<MapAF> map_fact_;
};

} // namespace LIB

}  // namespace SBG

#endif
