/** @file matching_fact.hpp

 @brief <b>Matching Algorithm Factory</b>

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

#ifndef SBG_MATCHING_FACT_HPP
#define SBG_MATCHING_FACT_HPP

#include "algorithms/matching/matching.hpp"

namespace SBG {

namespace LIB {

class MatchingFact {
  public:
  virtual ~MatchingFact() = default;
  MatchingFact() = default;

  virtual Matching createMatchAlgorithm(const PWMapAF& fact) const = 0;
};

class BFSMatchingFact : public MatchingFact {
  public:
  BFSMatchingFact() = default;

  Matching createMatchAlgorithm(const PWMapAF& fact) const override;
};

} // namespace LIB

}  // namespace SBG

#endif
