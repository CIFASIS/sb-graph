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

#include <eval/defs.hpp>

namespace SBG {

namespace Eval {

// Type definitions ------------------------------------------------------------

Util::INT toInt(ExprBaseType v) 
{ 
  if (std::holds_alternative<Util::INT>(v))
    return std::get<Util::INT>(v);

  if (std::holds_alternative<Util::RATIONAL>(v)) {
    Util::RATIONAL v_rat = std::get<Util::RATIONAL>(v);
    if (v_rat.denominator() == 1)
      return v_rat.numerator();
  }

  Util::ERROR("toInt: intervals are composed by ints");
  return 0; 
}

AST::Expr toExpr(ExprBaseType v) 
{
  if (std::holds_alternative<Util::INT>(v))
    return std::get<Util::INT>(v);

  if (std::holds_alternative<Util::RATIONAL>(v))
    return std::get<Util::RATIONAL>(v);

  if (std::holds_alternative<SBG::Interval>(v)) {
    SBG::Interval i = std::get<SBG::Interval>(v);
    return AST::Interval(i.begin(), i.step(), i.end());
  }

  Util::ERROR("toExpr: compiler error?");
  return AST::Expr();
}

// Environments ----------------------------------------------------------------

VarEnv::VarEnv() {}

FuncEnv::FuncEnv() {
  insert("isEmpty", 0);
  insert("isMember", 1);
}

} // namespace Eval

} // namespace SBG
