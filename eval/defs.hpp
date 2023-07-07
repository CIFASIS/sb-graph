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

#include <tuple>
#include <variant>

#include <ast/expr.hpp>
#include <ast/statement.hpp>
#include <sbg/pw_inter.hpp>
#include <util/debug.hpp>
#include <util/defs.hpp>
#include <util/env.hpp>

namespace SBG {

namespace Eval {

// Type definitions ------------------------------------------------------------

typedef std::variant<Util::INT, Util::RATIONAL, SBG::Interval, SBG::Set> ExprBaseType;

template <class T>
struct streamer {
  const T &val;
};
template <class T> streamer(T) -> streamer<T>;

template <class T>
std::ostream &operator<<(std::ostream &out, streamer<T> s) 
{
  out << s.val;

  return out;
}

template <class... Ts>
std::ostream &operator<<(std::ostream &out, streamer<std::variant<Ts...>> sv)
{
  std::visit([&out](const auto &v) { out << streamer{v}; }, sv.val);

  return out;
}

/** @function toInt
 *
 * @brief Converts a rational n/1 to an integer n (if needed).
 */
Util::INT toInt(ExprBaseType t);

// Environments ----------------------------------------------------------------

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
 * is associated with a number that will be used by the EvalExpression. The
 * pairs should be inserted in the same order as the enum class. 
 */
struct FuncEnv : public Util::SymbolTable<AST::Name, int> {
  FuncEnv();
};

typedef enum { empty, member, min, max } Func;
const FuncEnv FUNC_ENV;

/** @struct Overload
 *
 * @brief To provide in-place lambdas for visitation for the different
 * operations. These is needed because different structures share the same
 * functions (for example, isEmpty can be applied to intervals, sets, etc.).
 */

template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

// Classes for pretty printing ------------------------------------------------

typedef std::tuple<AST::Expr, ExprBaseType> ExprEval;
std::ostream &operator<<(std::ostream &out, const ExprEval &e);
typedef std::vector<ExprEval> ExprEvalList; 
std::ostream &operator<<(std::ostream &out, const ExprEvalList &ee);

/** @struct ProgramIO
 *
 * @brief Class to pretty print a program and its correspondent evaluation.
 *   - Assign statements only evaluate the right side.
 *   - There will be a tuple for each expression with its original form and
 *     the result of evaluating it.
 */

struct ProgramIO {
  member_class(AST::StatementList, stms);
  member_class(ExprEvalList, exprs);
 
  ProgramIO(); 
  ProgramIO(AST::StatementList stms, ExprEvalList exprs);
};
std::ostream &operator<<(std::ostream &out, const ProgramIO &p);

} // namespace Eval

} // namespace SBG

#endif
