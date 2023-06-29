/** @file defs.hpp

 @brief <b>Useful macros</b>
 
 This file provides helpful definitions that will be used by the Eval module.

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

#ifndef EVAL_DEFS_HPP
#define EVAL_DEFS_HPP

#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

#include <ast/expr.hpp>
#include <sbg/interval.hpp>
#include <util/debug.hpp>
#include <util/defs.hpp>
#include <util/env.hpp>

namespace SBG {

namespace Eval {

typedef boost::variant<Util::INT, Util::RATIONAL, SBG::Interval> ExprBaseType;

template <typename T>
inline bool is(ExprBaseType e) { return e.type() == typeid(T); }

/** @function toInt
*
* @brief Converts a rational n/1 to an integer n (if needed).
*/
Util::INT toInt(ExprBaseType t);
AST::Expr toExpr(ExprBaseType t);

/** @struct VarEnv
*
* @brief Variable environment (with expressions already evaluated). This env
* will be populated by StmVisitor, and used by EvalExpression.
*/
struct VarEnv : public Util::SymbolTable<Util::VariableName, ExprBaseType> {
  VarEnv();
};

/** @struct FuncEnv
*
* @brief Function environment. Statically defined: SBG programs don't allow
* the definition of new functions. This table should be updated manually each
* time a new operation for SBG and their structures is defined. Each function
* is associated with a number that will be used by the EvalExpression. 
*/
struct FuncEnv : public Util::SymbolTable<AST::Name, int> {
  FuncEnv();
};

const FuncEnv FUNC_ENV;

} // namespace Eval

} // namespace SBG

#endif
