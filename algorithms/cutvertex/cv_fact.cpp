/*******************************************************************************

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

#include "algorithms/cutvertex/cv_fact.hpp"
#include "algorithms/cutvertex/maxdeg_cv.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Maximum Degree Cut Vertex AF ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

CutVertex MaxDegCVFact::createCVAlgorithm() const
{
  return CutVertex(std::make_unique<MaxDegCutVertex>());
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

CVFactory::CVFactory() : cv_fact_(std::make_unique<MaxDegCVFact>()) {}

CVFact& CVFactory::cv_fact()
{
  return *cv_fact_;
}

void CVFactory::set_cv_fact(CVFactPtr cv_fact)
{
  cv_fact_ = std::move(cv_fact);
}

} // namespace LIB

} // namespace SBG
