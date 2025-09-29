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

#ifndef AUTOM_IMPL_VISITOR 
#define AUTOM_IMPL_VISITOR 

#include <boost/variant.hpp>

#include "ast/sbg_program.hpp"
#include "eval/impl_context.hpp"

namespace SBG {

namespace Eval {

class AutomImplVisitor {
  public:
  AutomImplVisitor();

  void visit(AST::SBGProgram p) const;
};

} // namespace Eval

} // namespace SBG

#endif
