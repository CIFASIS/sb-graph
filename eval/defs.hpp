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
#include <variant>

#include "ast/expr.hpp"
#include "ast/statement.hpp"
#include "sbg/af_pwmap.hpp"
#include "sbg/sbg_algorithms.hpp"

namespace SBG {

namespace Eval {

// Type definitions ------------------------------------------------------------

struct Boolean {
  bool b_;

  Boolean();
  Boolean(bool b);
};
std::ostream &operator<<(std::ostream &out, const Boolean &b);

typedef std::variant<Boolean
  , LIB::NAT
  , LIB::MD_NAT
  , LIB::RATIONAL
  , LIB::Interval
  , LIB::SetPiece
  , LIB::Set
  , LIB::Exp
  , LIB::Map
  , LIB::PWMap
  , LIB::SBG
  , LIB::DSBG
  , LIB::MatchInfo> ExprBaseType;
typedef std::optional<ExprBaseType> MaybeEBT;

// Environments ----------------------------------------------------------------

typedef AST::Name VKey;
typedef ExprBaseType VValue;
typedef std::optional<VValue> MaybeVValue;
typedef std::map<VKey, VValue> VarEnvType;

/** 
 * @brief Variable environment (with expressions already evaluated). This env
 * will be populated by StmVisitor, and used by EvalExpression.
 */
struct VarEnv {
  VarEnv();

  void insert(VKey k, VValue v);
  MaybeVValue operator[](VKey k) const;

  private:
  mutable VarEnvType mapping_;
};

typedef AST::Name FKey;
typedef int FValue;
typedef std::optional<FValue> MaybeFValue;
typedef std::map<FKey, FValue> FuncEnvType;

/**
 * @brief Function environment. Statically defined: SBG programs don't allow
 * the definition of new functions. This table should be updated manually each
 * time a new operation for SBG and their structures is defined. Each function
 * is associated with a number that will be used by the EvalExpression. The
 * pairs should be inserted in the same order as the enum class. 
 */
struct FuncEnv{
  FuncEnv();

  MaybeFValue operator[](AST::Name) const;

  private:
  static FuncEnvType mapping_;
};

typedef enum { empty, min, max, comp, inv, im, preim, dom, comb
  , first_inv, min_map, red, min_adj, inf, connected, matching, scc, ts
  , match_scc, match_scc_ts, cut_set } Func;

// Classes for pretty printing ------------------------------------------------

template<typename T, typename... Ts>
std::ostream &operator<<(std::ostream &out, const std::variant<T, Ts...> &v);

typedef std::tuple<AST::Name, ExprBaseType> StmEval;
std::ostream &operator<<(std::ostream &out, const StmEval &e);
typedef std::vector<StmEval> StmEvalList;
std::ostream &operator<<(std::ostream &out, const StmEvalList &e);
typedef std::tuple<AST::Expr, ExprBaseType> ExprEval;
std::ostream &operator<<(std::ostream &out, const ExprEval &e);
typedef std::vector<ExprEval> ExprEvalList; 
std::ostream &operator<<(std::ostream &out, const ExprEvalList &ee);

/** 
 * @brief Class to pretty print a program and its correspondent evaluation.
 *   - Assign statements only evaluate the right side of an assignment.
 *   - There will be a tuple for each expression with its original form and
 *     the result of evaluating it.
 */
struct ProgramIO {
  member_class(unsigned int, nmbr_dims);
  member_class(StmEvalList, stms);
  member_class(ExprEvalList, exprs);
 
  ProgramIO(StmEvalList stms, ExprEvalList exprs);
  ProgramIO(unsigned int nmbr_dims, StmEvalList stms, ExprEvalList exprs);
};
std::ostream &operator<<(std::ostream &out, const ProgramIO &p);

} // namespace Eval

} // namespace SBG

#endif
