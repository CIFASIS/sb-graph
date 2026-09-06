/** @file base_type.hpp

 @brief <b>Expression evaluator base type</b>
 
 This module defines the possible types that a SBG expression can evaluate to.

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

#ifndef SBGRAPH_EVAL_BASE_TYPE_HPP_
#define SBGRAPH_EVAL_BASE_TYPE_HPP_

#include "algorithms/matching/match_data.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "sbg/directed_sbg.hpp"
#include "sbg/expression.hpp"
#include "sbg/integer.hpp"
#include "sbg/map.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/rational.hpp"
#include "sbg/sbg.hpp"
#include "sbg/set.hpp"

#include <optional>
#include <variant>

namespace SBG {

namespace Eval {

using ExprBaseType = std::variant<bool
  , LIB::Int
  , LIB::IntTuple
  , LIB::Rational
  , LIB::Set
  , LIB::Expression
  , LIB::Map
  , LIB::PWMap
  , LIB::SBG
  , LIB::BipartiteSBG
  , LIB::DirectedSBG
  , LIB::MatchData>;

using MaybeEBT = std::optional<ExprBaseType>;

using EBTList = std::vector<ExprBaseType>;

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_BASE_TYPE_HPP_
