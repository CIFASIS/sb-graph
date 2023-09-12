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

#include <map>
#include <tuple>

#include "ast/expr.hpp"
#include "ast/statement.hpp"
#include "sbg/unord_pw_mdinter.hpp"
#include "sbg/ord_pw_mdinter.hpp"
#include "sbg/pw_map.hpp"

namespace SBG {

namespace Eval {

// Type definitions ------------------------------------------------------------

typedef std::variant<Util::NAT
  , Util::MD_NAT
  , Util::RATIONAL
  , LIB::Interval
  , LIB::MultiDimInter
  , LIB::UnordPWMDInter
  , LIB::OrdPWMDInter
  , LIB::LExp
  , LIB::MDLExp
  , LIB::BaseMap
  , LIB::CanonMap
  , LIB::BasePWMap
  , LIB::CanonPWMap> ExprBaseType;
typedef std::optional<ExprBaseType> MaybeEBT;

/** @function toNat
 *
 * @brief Converts a positive rational n/1 to n (if possible).
 */
Util::NAT toNat(ExprBaseType t);

// Environments ----------------------------------------------------------------

/** @struct VarEnv
 *
 * @brief Variable environment (with expressions already evaluated). This env
 * will be populated by StmVisitor, and used by EvalExpression.
 */
typedef Util::VariableName VKey;
typedef ExprBaseType VValue;
typedef std::optional<VValue> MaybeVValue;
typedef std::map<VKey, VValue> VarEnvType;
struct VarEnv{
  VarEnv();

  void insert(VKey k, VValue v);
  MaybeVValue operator[](VKey k) const;

  private:
  mutable VarEnvType mapping_;
};

/** @struct FuncEnv
 *
 * @brief Function environment. Statically defined: SBG programs don't allow
 * the definition of new functions. This table should be updated manually each
 * time a new operation for SBG and their structures is defined. Each function
 * is associated with a number that will be used by the EvalExpression. The
 * pairs should be inserted in the same order as the enum class. 
 */
typedef AST::Name FKey;
typedef int FValue;
typedef std::optional<FValue> MaybeFValue;
typedef std::map<FKey, FValue> FuncEnvType;
struct FuncEnv{
  FuncEnv();

  MaybeFValue operator[](AST::Name) const;

  private:
  static FuncEnvType mapping_;
};

typedef enum { empty, member, min, max, lt, comp, inv, im, preim, dom, min_adj } Func;

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
  member_class(Util::NAT, nmbr_dims);
  member_class(AST::StatementList, stms);
  member_class(ExprEvalList, exprs);
 
  ProgramIO(); 
  ProgramIO(AST::StatementList stms, ExprEvalList exprs);
  ProgramIO(Util::NAT nmbr_dims, AST::StatementList stms, ExprEvalList exprs);
};
std::ostream &operator<<(std::ostream &out, const ProgramIO &p);

} // namespace Eval

} // namespace SBG

#endif
