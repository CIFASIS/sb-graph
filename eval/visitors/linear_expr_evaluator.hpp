/** @file le_evaluator.hpp

 @brief <b>Linear expression evaluator</b>

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

#ifndef LE_EVALUATOR 
#define LE_EVALUATOR 

#include "eval/var_env.hpp"
#include "sbg/lexp.hpp"

namespace SBG {

namespace Eval {

class LinearExprEvaluator : public boost::static_visitor<LIB::LExp> {
  public:
  LinearExprEvaluator(VarEnv &venv);

  LIB::LExp operator()(AST::Natural v) const;
  LIB::LExp operator()(AST::Rational v) const;
  LIB::LExp operator()(AST::Name v) const;
  LIB::LExp operator()(AST::UnaryOp v) const;
  LIB::LExp operator()(AST::BinOp v) const;
  LIB::LExp operator()(AST::Call v) const;
  LIB::LExp operator()(AST::Interval v) const;
  LIB::LExp operator()(AST::MultiDimInter v) const;
  LIB::LExp operator()(AST::Set v) const;
  LIB::LExp operator()(AST::LinearExp v) const;
  LIB::LExp operator()(AST::MDLExp v) const;
  LIB::LExp operator()(AST::LinearMap v) const;
  LIB::LExp operator()(AST::PWLMap v) const;
  LIB::LExp operator()(AST::SBG v) const;
  LIB::LExp operator()(AST::DSBG v) const;
  LIB::LExp operator()(AST::ParenExpr v) const;

  private:
  mutable VarEnv venv_;
};

} // namespace Eval

} // namespace SBG

#endif
