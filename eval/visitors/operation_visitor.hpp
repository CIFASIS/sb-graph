/** @file operation_visitor.hpp

 @brief <b>SBG operations visitors</b>

 For each operation defined in the SBG module we define a visitor to evaluate
 its result in the eval_expr module.

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

#ifndef AST_VISITOR_OPERATION
#define AST_VISITOR_OPERATION 

#include <ast/statement.hpp>
#include <eval/visitors/eval_expr.hpp>
#include <sbg/interval.hpp>

namespace SBG {

namespace Eval {

class CardinalVisitor : public boost::static_visitor<unsigned int> {
  public:
  CardinalVisitor();
 
  unsigned int operator()(Util::INT v) const; 
  unsigned int operator()(Util::RATIONAL v) const;
  unsigned int operator()(SBG::Interval i) const;
};

} // namespace Eval

} // namespace SBG

#endif
