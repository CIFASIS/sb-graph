/** @file af_cv.hpp

 @brief <b>Vertex Cut Set Algorithm Abstract Factory</b>

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

#ifndef SBG_AF_CV_HPP
#define SBG_AF_CV_HPP

#include "cut_vertex.hpp"

namespace SBG {

namespace LIB {

struct CVAF {
  public:
  virtual ~CVAF() = default;
  CVAF() = default;

  virtual CutVertex createCVAlgorithm(const PWMapAF &pw_fact
    , const SCCAF &scc_fact) const = 0;
};

struct MaxDegCVAF : public CVAF {
  public:
  MaxDegCVAF() = default;

  CutVertex createCVAlgorithm(const PWMapAF &pw_fact, const SCCAF &scc_fact)
    const override;
};

} // namespace LIB

}  // namespace SBG

#endif
