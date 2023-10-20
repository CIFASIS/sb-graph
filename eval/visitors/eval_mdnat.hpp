/** @file eval_mdnat.hpp

 @brief <b>Multi-dimensional natural expression evaluator</b>

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

#ifndef AST_VISITOR_EVAL_MDNAT
#define AST_VISITOR_EVAL_MDNAT

#include "eval/visitors/eval_nat.hpp"

namespace SBG {

namespace Eval {

struct EvalMDNat : public boost::static_visitor<Util::MD_NAT> {
  public:
  EvalMDNat();
  EvalMDNat(VarEnv env);

  Util::MD_NAT operator()(AST::Natural v) const;
  Util::MD_NAT operator()(AST::MDNatural v) const;
  Util::MD_NAT operator()(AST::Rational v) const;
  Util::MD_NAT operator()(AST::Boolean v) const;
  Util::MD_NAT operator()(Util::VariableName v) const;
  Util::MD_NAT operator()(AST::UnaryOp v) const;
  Util::MD_NAT operator()(AST::BinOp v) const;
  Util::MD_NAT operator()(AST::Call v) const;
  Util::MD_NAT operator()(AST::Interval v) const;
  Util::MD_NAT operator()(AST::InterUnaryOp v) const;
  Util::MD_NAT operator()(AST::InterBinOp v) const;
  Util::MD_NAT operator()(AST::MultiDimInter v) const;
  Util::MD_NAT operator()(AST::MDInterUnaryOp v) const;
  Util::MD_NAT operator()(AST::MDInterBinOp v) const;
  Util::MD_NAT operator()(AST::Set v) const;
  Util::MD_NAT operator()(AST::SetUnaryOp v) const;
  Util::MD_NAT operator()(AST::SetBinOp v) const;
  Util::MD_NAT operator()(AST::LinearExp v) const;
  Util::MD_NAT operator()(AST::LExpBinOp v) const;
  Util::MD_NAT operator()(AST::MDLExp v) const;
  Util::MD_NAT operator()(AST::MDLExpBinOp v) const;
  Util::MD_NAT operator()(AST::LinearMap v) const;
  Util::MD_NAT operator()(AST::PWLMap v) const;
  Util::MD_NAT operator()(AST::SBG v) const;
  Util::MD_NAT operator()(AST::DSBG v) const;

  private:
  mutable VarEnv env_;
};

} // namespace Eval

} // namespace SBG

#endif
