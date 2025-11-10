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

#ifndef EVAL_PRETTY_PRINT_HPP
#define EVAL_PRETTY_PRINT_HPP

#include "eval/base_type.hpp"

namespace SBG {

namespace Eval {

template<typename T, typename... Ts>
std::ostream &operator<<(std::ostream &out, const std::variant<T, Ts...> &v);

using StmResult = std::tuple<AST::Name, ExprBaseType>;
std::ostream &operator<<(std::ostream &out, const StmResult &e);
using StmResultList = std::vector<StmResult>;
std::ostream &operator<<(std::ostream &out, const StmResultList &e);
using ExprResult = std::tuple<AST::Expr, ExprBaseType>;
std::ostream &operator<<(std::ostream &out, const ExprResult &e);
using ExprResultList = std::vector<ExprResult>;
std::ostream &operator<<(std::ostream &out, const ExprResultList &ee);

/** 
 * @brief Class to pretty print a program and its correspondent evaluation.
 *   - Assign statements only evaluate the right side of an assignment.
 *   - There will be a tuple for each expression with its original form and
 *     the result of evaluating it.
 */
struct ProgramIO {
  member_class(unsigned int, nmbr_dims);
  member_class(StmResultList, stms);
  member_class(ExprResultList, exprs);
 
  ProgramIO(StmResultList stms, ExprResultList exprs);
  ProgramIO(unsigned int nmbr_dims, StmResultList stms, ExprResultList exprs);
};
std::ostream &operator<<(std::ostream &out, const ProgramIO &p);

} // namespace Eval

} // namespace SBG

#endif
