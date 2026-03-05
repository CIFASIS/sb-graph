/** @file pretty_print.hpp

 @brief <b>Pretty printer for evaluated SBG programs</b>
 
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

#ifndef SBGRAPH_EVAL_PRETTY_PRINT_HPP_
#define SBGRAPH_EVAL_PRETTY_PRINT_HPP_

#include "ast/expr.hpp"
#include "eval/base_type.hpp"

#include <iosfwd>
#include <tuple>
#include <variant>

namespace SBG {

namespace Eval {

template<typename T, typename... Ts>
std::ostream& operator<<(std::ostream& out, const std::variant<T, Ts...>& v);

using ExprResult = std::tuple<AST::Expr, ExprBaseType>;
std::ostream& operator<<(std::ostream& out, const ExprResult& e);
using ExprResultList = std::vector<ExprResult>;
std::ostream& operator<<(std::ostream& out, const ExprResultList& ee);
using StmResult = std::tuple<AST::Name, ExprBaseType>;
std::ostream& operator<<(std::ostream& out, const StmResult& s);
using StmResultList = std::vector<StmResult>;
std::ostream& operator<<(std::ostream& out, const StmResultList& ss);

/** 
 * @brief Class to pretty print a program and its correspondent evaluation.
 *   - Assign statements only evaluate the right side of an assignment.
 *   - There will be a tuple for each expression with its original form and
 *     the result of evaluating it.
 */
class ProgramIO {
public:
  ProgramIO(StmResultList stms, ExprResultList exprs);
  ProgramIO(std::size_t n, StmResultList stms, ExprResultList exprs);

  const std::size_t& arity() const;
  const StmResultList& stms() const;
  const ExprResultList& exprs() const;

private:
  std::size_t _arity;
  StmResultList _stms;
  ExprResultList _exprs;
};

std::ostream& operator<<(std::ostream& out, const ProgramIO& p);

} // namespace Eval

} // namespace SBG

#endif
