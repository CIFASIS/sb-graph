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

#include <eval/defs.hpp>
#include <eval/visitors/eval_le.hpp>
#include <eval/visitors/eval_interval.hpp>
#include <sbg/map.hpp>

namespace SBG {

namespace Eval {

struct EvalMap : public boost::static_visitor<SBG::SBGMap> {
  public:
  EvalMap();
  EvalMap(VarEnv env);

  SBG::SBGMap operator()(AST::Natural v) const;
  SBG::SBGMap operator()(AST::Rational v) const;
  SBG::SBGMap operator()(AST::Boolean v) const;
  SBG::SBGMap operator()(Util::VariableName v) const;
  SBG::SBGMap operator()(AST::BinOp v) const;
  SBG::SBGMap operator()(AST::Call v) const;
  SBG::SBGMap operator()(AST::Interval v) const;
  SBG::SBGMap operator()(AST::InterUnaryOp v) const;
  SBG::SBGMap operator()(AST::InterBinOp v) const;
  SBG::SBGMap operator()(AST::Set v) const;
  SBG::SBGMap operator()(AST::SetUnaryOp v) const;
  SBG::SBGMap operator()(AST::SetBinOp v) const;
  SBG::SBGMap operator()(AST::LinearExp v) const;
  SBG::SBGMap operator()(AST::LExpBinOp v) const;
  SBG::SBGMap operator()(AST::LinearMap v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
