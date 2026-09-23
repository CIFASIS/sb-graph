/** @file autom_impl_visitor.hpp

 @brief <b>Implementation Visitor</b>

 This visitor reads the input AST and decides the optimal implementation that
 can be used for that instance. For example, to use ordered dense sets it checks
 that all intervals have step=1 and maps return dense intervals.

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

#ifndef SBGRAPH_EVAL_VISITORS_AUTOM_IMPL_VISITOR_HPP_ 
#define SBGRAPH_EVAL_VISITORS_AUTOM_IMPL_VISITOR_HPP_

#include "ast/sbg_program.hpp"
#include "eval/user_input.hpp"

namespace SBG {

namespace Eval {

namespace detail {

class AutomImplVisitor {
public:
  AutomImplVisitor();

  EvalUserInput visit(AST::SBGProgram p) const;
};

} // namespace detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_VISITORS_AUTOM_IMPL_VISITOR_HPP_
