/** @file eval_map.hpp

 @brief <b>Linear map expression evaluator</b>

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

#ifndef AST_VISITOR_EVALMAP
#define AST_VISITOR_EVALMAP

#include <boost/variant/static_visitor.hpp>

#include "eval/visitors/eval_le.hpp"
#include "eval/visitors/eval_set.hpp"
#include "sbg/map.hpp"

namespace SBG {

namespace Eval {

struct EvalMap : public boost::static_visitor<LIB::SBGMap> {
  public:
  EvalMap();
  EvalMap(VarEnv env);

  LIB::SBGMap operator()(AST::Natural v) const;
  LIB::SBGMap operator()(AST::Rational v) const;
  LIB::SBGMap operator()(AST::Boolean v) const;
  LIB::SBGMap operator()(Util::VariableName v) const;
  LIB::SBGMap operator()(AST::UnaryOp v) const;
  LIB::SBGMap operator()(AST::BinOp v) const;
  LIB::SBGMap operator()(AST::Call v) const;
  LIB::SBGMap operator()(AST::Interval v) const;
  LIB::SBGMap operator()(AST::InterUnaryOp v) const;
  LIB::SBGMap operator()(AST::InterBinOp v) const;
  LIB::SBGMap operator()(AST::Set v) const;
  LIB::SBGMap operator()(AST::SetUnaryOp v) const;
  LIB::SBGMap operator()(AST::SetBinOp v) const;
  LIB::SBGMap operator()(AST::LinearExp v) const;
  LIB::SBGMap operator()(AST::LExpBinOp v) const;
  LIB::SBGMap operator()(AST::LinearMap v) const;
  LIB::SBGMap operator()(AST::PWLMap v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
