/** @file ts_fact.hpp

 @brief <b>Topological Sort Algorithm Factory</b>

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

#ifndef SBG_TS_FACT_HPP
#define SBG_TS_FACT_HPP

#include "topo_sort.hpp"

namespace SBG {

namespace LIB {

struct TSFact {
  public:
  virtual ~TSFact() = default;
  TSFact() = default;

  virtual TopoSort createTSAlgorithm(const PWMapAF& fact) const = 0;
};

struct MinVertexTSFact : public TSFact {
  public:
  MinVertexTSFact() = default;

  TopoSort createTSAlgorithm(const PWMapAF& fact) const override;
};

} // namespace LIB

}  // namespace SBG

#endif
