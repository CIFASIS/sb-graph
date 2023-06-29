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

Util::INT toInt(ExprBaseType v) { 
  if (is<Util::INT>(v))
    return boost::get<Util::INT>(v);

  if (is<Util::RATIONAL>(v)) {
    Util::RATIONAL v_rat = boost::get<Util::RATIONAL>(v);
    if (v_rat.denominator() == 1)
      return v_rat.numerator();
  }

  Util::ERROR("toInt: intervals are composed by ints");
  return 0; 
}

AST::Expr toExpr(ExprBaseType v) {
  if (is<Util::INT>(v))
    return boost::get<Util::INT>(v);

  if (is<Util::RATIONAL>(v))
    return boost::get<Util::RATIONAL>(v);

  if (is<SBG::Interval>(v)) {
    SBG::Interval i = boost::get<SBG::Interval>(v);
    return AST::Interval(i.begin(), i.step(), i.end());
  }

  Util::ERROR("toExpr: compiler error?");
  return AST::Expr();
}

VarEnv::VarEnv() {}

FuncEnv::FuncEnv() {
  insert("cardinal", 0);
  insert("isEmpty", 1);
  insert("isMember", 2);
  insert("intersection", 3);
  insert("difference", 4);
}

} // namespace Eval

} // namespace SBG
