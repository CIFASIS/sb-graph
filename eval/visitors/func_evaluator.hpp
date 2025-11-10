/** @file func_evaluator.hpp

 @brief <b>Function evaluator</b>

 Evaluator for built-in functions. Currently user-defined functions aren't
 allowed.

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

#ifndef FUNC_EVALUATOR 
#define FUNC_EVALUATOR 

#include "algorithms/cutvertex/cut_vertex.hpp"
#include "algorithms/matching/matching.hpp"
#include "algorithms/scc/scc.hpp"
#include "algorithms/toposort/topo_sort.hpp"
#include "eval/base_type.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Overload pattern ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Provides in-place lambdas for visitation for the different
 * operations. These are needed because different structures share the same
 * functions (for example, isEmpty can be applied to intervals, sets, etc.).
 */

template<class... Ts> class Overload : Ts... {
  public:
  using Ts::operator()...;
  Overload(Ts... ts) : Ts(ts)... {};
};
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

////////////////////////////////////////////////////////////////////////////////
// Built-in Operators ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class BuiltInOperators {
  public:
  static ExprBaseType oppositeEvaluator(const EBTList& args);
  static ExprBaseType cardinalEvaluator(const EBTList& args);
  static ExprBaseType complementEvaluator(const EBTList& args);
  static ExprBaseType addEvaluator(const EBTList& args);
  static ExprBaseType subEvaluator(const EBTList& args);
  static ExprBaseType multEvaluator(const EBTList& args);
  static ExprBaseType eqEvaluator(const EBTList& args);
  static ExprBaseType lessEvaluator(const EBTList& args);
  static ExprBaseType capEvaluator(const EBTList& args);
  static ExprBaseType cupEvaluator(const EBTList& args);
  static ExprBaseType diffEvaluator(const EBTList& args);
};

class UnaryOpEvaluator {
  public:
  UnaryOpEvaluator();

  ExprBaseType evaluate(EBTList& evaluated_args, AST::UnOp op);
};

class BinOpEvaluator {
  public:
  BinOpEvaluator();

  ExprBaseType evaluate(EBTList& evaluated_args, AST::Op op);
};

////////////////////////////////////////////////////////////////////////////////
// Built-in Functions ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

class BuiltInFunctions {
  public:
  static ExprBaseType emptyEvaluator(const EBTList& args);
  static ExprBaseType minEvaluator(const EBTList& args);
  static ExprBaseType maxEvaluator(const EBTList& args);
  static ExprBaseType composeEvaluator(const EBTList& args);
  static ExprBaseType inverseEvaluator(const EBTList& args);
  static ExprBaseType imageEvaluator(const EBTList& args);
  static ExprBaseType preImageEvaluator(const EBTList& args);
  static ExprBaseType domEvaluator(const EBTList& args);
  static ExprBaseType combineEvaluator(const EBTList& args);
  static ExprBaseType firstInvEvaluator(const EBTList& args);
  static ExprBaseType minMapEvaluator(const EBTList& args);
  static ExprBaseType reduceEvaluator(const EBTList& args);
  static ExprBaseType minAdjEvaluator(const EBTList& args);
  static ExprBaseType mapInfEvaluator(const EBTList& args);
  static ExprBaseType connectedEvaluator(const EBTList& args);
  static ExprBaseType matchingEvaluator(const EBTList& args);
  static ExprBaseType sccEvaluator(const EBTList& args);
  static ExprBaseType topoSortEvaluator(const EBTList& args);
  static ExprBaseType cutVertexEvaluator(const EBTList& args);
  static ExprBaseType matchSCCEvaluator(const EBTList& args);
};

} // namespace Eval

} // namespace SBG

#endif
