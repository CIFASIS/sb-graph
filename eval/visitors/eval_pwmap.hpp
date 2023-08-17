/** @file eval_pwmap.hpp

 @brief <b>Piecewise linear map expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_PWMAP
#define AST_VISITOR_EVAL_PWMAP

#include <boost/variant/static_visitor.hpp>

#include "eval/visitors/eval_map.hpp"
#include "sbg/pw_map.hpp"

namespace SBG {

namespace Eval {

struct EvalPWMap : public boost::static_visitor<LIB::PWMap> {
  public:
  EvalPWMap();
  EvalPWMap(VarEnv env);

  LIB::PWMap operator()(AST::Natural v) const;
  LIB::PWMap operator()(AST::Rational v) const;
  LIB::PWMap operator()(AST::Boolean v) const;
  LIB::PWMap operator()(Util::VariableName v) const;
  LIB::PWMap operator()(AST::UnaryOp v) const;
  LIB::PWMap operator()(AST::BinOp v) const;
  LIB::PWMap operator()(AST::Call v) const;
  LIB::PWMap operator()(AST::Interval v) const;
  LIB::PWMap operator()(AST::InterUnaryOp v) const;
  LIB::PWMap operator()(AST::InterBinOp v) const;
  LIB::PWMap operator()(AST::Set v) const;
  LIB::PWMap operator()(AST::SetUnaryOp v) const;
  LIB::PWMap operator()(AST::SetBinOp v) const;
  LIB::PWMap operator()(AST::LinearExp v) const;
  LIB::PWMap operator()(AST::LExpBinOp v) const;
  LIB::PWMap operator()(AST::LinearMap v) const;
  LIB::PWMap operator()(AST::PWLMap v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
