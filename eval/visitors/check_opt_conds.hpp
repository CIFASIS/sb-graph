/** @file check_opt_conds.hpp

 @brief <b>Optimization conditions visitor</b>

 Check if the conditions are met to use the optimal implementation of sets.

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

#ifndef AST_VISITOR_OPTCONDS
#define AST_VISITOR_OPTCONDS

#include "eval/visitors/eval_interval.hpp"

namespace SBG {

namespace Eval {

struct OptConds : public boost::static_visitor<bool> {
  public:
  OptConds();
  OptConds(VarEnv env);

  bool operator()(AST::Natural v) const;
  bool operator()(AST::MDNatural v) const;
  bool operator()(AST::Rational v) const;
  bool operator()(AST::Boolean v) const;
  bool operator()(Util::VariableName v) const;
  bool operator()(AST::UnaryOp v) const;
  bool operator()(AST::BinOp v) const;
  bool operator()(AST::Call v) const;
  bool operator()(AST::Interval v) const;
  bool operator()(AST::InterUnaryOp v) const;
  bool operator()(AST::InterBinOp v) const;
  bool operator()(AST::MultiDimInter v) const;
  bool operator()(AST::MDInterUnaryOp v) const;
  bool operator()(AST::MDInterBinOp v) const;
  bool operator()(AST::Set v) const;
  bool operator()(AST::SetUnaryOp v) const;
  bool operator()(AST::SetBinOp v) const;
  bool operator()(AST::LinearExp v) const;
  bool operator()(AST::LExpBinOp v) const;
  bool operator()(AST::MDLExp v) const;
  bool operator()(AST::MDLExpBinOp v) const;
  bool operator()(AST::LinearMap v) const;
  bool operator()(AST::PWLMap v) const;
  bool operator()(AST::SBG v) const;
  bool operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
