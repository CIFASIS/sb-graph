/*******************************************************************************

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

#include "algorithms/cc/cc.hpp"
//#include "algorithms/cutvertex/cv_fact.hpp"
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
//#include "algorithms/toposort/ts_fact.hpp"
#include "algorithms/misc/causalization_builders.hpp"
#include "algorithms/misc/causalization_json.hpp"
#include "eval/visitors/expr_evaluator.hpp"
#include "eval/visitors/func_evaluator.hpp"
#include "eval/visitors/linear_expr_evaluator.hpp"
#include "eval/visitors/nat_evaluator.hpp"
#include "eval/visitors/rational_evaluator.hpp"
#include "sbg/pwmap_fact.hpp"
#include "sbg/set_fact.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace Eval {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Expression evaluator --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

template<typename T>
T eval(const ExprEvaluator& visit, AST::Expr e, std::string t = "UNDEF")
{
  ExprBaseType visited = boost::apply_visitor(visit, e);

  Util::ERROR_UNLESS(std::holds_alternative<T>(visited)
    , "ExprEvaluator: expression ", e, " is not of type ", t, "\n");

  return std::get<T>(visited);
}

ExprEvaluator::ExprEvaluator(EvalContext& eval_ctx) : _eval_context(eval_ctx)
{
  // Set built-in functions
  eval_ctx.insertFunction("isEmpty", BuiltInFunctions::emptyEvaluator);
  eval_ctx.insertFunction("minElem", BuiltInFunctions::minEvaluator);
  eval_ctx.insertFunction("maxElem", BuiltInFunctions::maxEvaluator);
  eval_ctx.insertFunction("restrict", BuiltInFunctions::restrictEvaluator);
  eval_ctx.insertFunction("compose", BuiltInFunctions::composeEvaluator);
  eval_ctx.insertFunction("inv", BuiltInFunctions::inverseEvaluator);
  eval_ctx.insertFunction("image", BuiltInFunctions::imageEvaluator);
  eval_ctx.insertFunction("preImage", BuiltInFunctions::preImageEvaluator);
  eval_ctx.insertFunction("dom", BuiltInFunctions::domEvaluator);
  eval_ctx.insertFunction("combine", BuiltInFunctions::combineEvaluator);
  eval_ctx.insertFunction("minMap", BuiltInFunctions::minMapEvaluator);
  eval_ctx.insertFunction("minAdj", BuiltInFunctions::minAdjEvaluator);
  eval_ctx.insertFunction("mapInf", BuiltInFunctions::mapInfEvaluator);
  eval_ctx.insertFunction("imgMult", BuiltInFunctions::imageMultEvaluator);
  eval_ctx.insertFunction("cc", BuiltInFunctions::connectedEvaluator);
  eval_ctx.insertFunction("match", BuiltInFunctions::matchingEvaluator);
  eval_ctx.insertFunction("scc", BuiltInFunctions::sccEvaluator);
  eval_ctx.insertFunction("matchSCC", BuiltInFunctions::matchSCCEvaluator);
  eval_ctx.insertFunction("mfvs", BuiltInFunctions::mfvsEvaluator);
}

ExprBaseType ExprEvaluator::operator()(AST::Natural v) const
{
  return (LIB::NAT) v;
}

ExprBaseType ExprEvaluator::operator()(AST::Rational v) const
{
  return boost::apply_visitor(RationalEvaluator{_eval_context.venv()}
    , AST::Expr{v});
}

ExprBaseType ExprEvaluator::operator()(AST::Name v) const 
{
  auto var_definition = _eval_context.venv().find(v);
  if (var_definition != _eval_context.venv().end()) { 
    return var_definition->second;
  }

  Util::ERROR("ExprEvaluator: variable ", v, " undefined\n");
  return ExprBaseType{}; 
}

ExprBaseType ExprEvaluator::operator()(AST::UnaryOp v) const
{
  ExprBaseType inner = boost::apply_visitor(*this, v.expr());
  EBTList evaluated_args;
  evaluated_args.push_back(inner);
  UnaryOpEvaluator unary_op_eval;
  return unary_op_eval.evaluate(evaluated_args, v.op());
}

ExprBaseType ExprEvaluator::operator()(AST::BinOp v) const
{ 
  ExprBaseType left = boost::apply_visitor(*this, v.left());
  ExprBaseType right = boost::apply_visitor(*this, v.right());
  EBTList evaluated_args;
  evaluated_args.push_back(left);
  evaluated_args.push_back(right);
  BinOpEvaluator bin_op_eval;
  return bin_op_eval.evaluate(evaluated_args, v.op());
}

ExprBaseType ExprEvaluator::operator()(AST::Call v) const
{
  std::string func_name = v.name();

  auto func_definition = _eval_context.fenv().find(func_name);
  if (func_definition != _eval_context.fenv().end()) { 
    std::vector<ExprBaseType> evaluated_args;
    for (AST::Expr a : v.args()) {
      evaluated_args.push_back(boost::apply_visitor(*this, a));
    }

    return func_definition->second(evaluated_args);
  }

  Util::ERROR("ExprEvaluator: function ", func_name, " doesn't exist\n");
  return ExprBaseType{};
}

ExprBaseType ExprEvaluator::operator()(AST::Interval v) const
{
  NatEvaluator nat_evaluator{_eval_context.venv()};

  LIB::NAT b = boost::apply_visitor(nat_evaluator, v.begin());
  LIB::NAT s = boost::apply_visitor(nat_evaluator, v.step());
  LIB::NAT e = boost::apply_visitor(nat_evaluator, v.end());

  return LIB::SET_FACT.createSet(b, s, e);
}

ExprBaseType ExprEvaluator::operator()(AST::MultiDimInter v) const
{
  LIB::Set result = LIB::SET_FACT.createSet();

  int i = 0;
  for (const AST::Expr& e : v.intervals())  {
    LIB::Set kth_component = eval<LIB::Set>(*this, e, "Set");
    if (i == 0) {
      result = std::move(kth_component);
    } else {
      result = result.cartesianProduct(kth_component);
    }
    ++i;
  }

  Util::ERROR_UNLESS(result.arity() == _eval_context.arity()
    || result.arity() == 0, "EvalMDI[arity = ", _eval_context.arity()
    , "]: arity(", result, ") = ", result.arity(), "\n");

  return result;
}


ExprBaseType ExprEvaluator::operator()(AST::Set v) const
{
  LIB::Set result = LIB::SET_FACT.createSet();

  for (const AST::Expr& e : v.pieces()) {
    LIB::Set jth_element = eval<LIB::Set>(*this, e, "Set");
    result = std::move(result).disjointCup(std::move(jth_element));
  }

  Util::ERROR_UNLESS(result.arity() == _eval_context.arity()
    || result.arity() == 0, "ExprEvaluator[arity = ", _eval_context.arity()
    , "]: arity(", result, ") = ", result.arity(), "\n");

  return result;
}

ExprBaseType ExprEvaluator::operator()(AST::LinearExp v) const
{
  LinearExprEvaluator linear_expr_evaluator{_eval_context.venv()};
  LIB::detail::LinearExpr linear_expr
    = boost::apply_visitor(linear_expr_evaluator, AST::Expr(v));
  return LIB::Expression{linear_expr.slope(), linear_expr.offset()};
}

ExprBaseType ExprEvaluator::operator()(AST::MDLExp v) const
{
  LIB::Expression result;

  LinearExprEvaluator linear_expr_evaluator{_eval_context.venv()};
  for (const AST::Expr& e : v.exps()) {
    LIB::detail::LinearExpr kth_component
      = boost::apply_visitor(linear_expr_evaluator, e);
    result = result.cartesianProduct(LIB::Expression{kth_component.slope()
      , kth_component.offset()});
  }

  Util::ERROR_UNLESS(result.arity() == _eval_context.arity()
    || result.arity() == 0, "ExprEvaluator[arity = ", _eval_context.arity(),
    "]: arity(", result, ") = ", result.arity(), "\n");

  return result;
}

ExprBaseType ExprEvaluator::operator()(AST::LinearMap v) const
{
  LIB::Set domain = eval<LIB::Set>(*this, v.dom(), "Set");
  LIB::Expression law = eval<LIB::Expression>(*this, v.lexp(), "Expression");

  LIB::Map result{domain, law};

  Util::ERROR_UNLESS(result.arity() == _eval_context.arity()
    || result.arity() == 0, "ExprEvaluator[arity = ", _eval_context.arity()
    , "]: arity(", result, ") = ", result.arity(), "\n");

  return result;
}

ExprBaseType ExprEvaluator::operator()(AST::PWLMap v) const
{
  LIB::PWMap result = LIB::PWMAP_FACT.createPWMap();

  for (const AST::Expr& e : v.maps()) {
    result.insert(eval<LIB::Map>(*this, e, "Map"));
  }

  Util::ERROR_UNLESS(result.arity() == _eval_context.arity()
    || result.arity() == 0, "ExprEvaluator[arity = ", _eval_context.arity()
    , "]: arity(", result, ") = ", result.arity(), "\n");

  return result;
}

ExprBaseType ExprEvaluator::operator()(AST::SBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V(), "Set");
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap(), "PWMap");
  LIB::PWMap map1 = eval<LIB::PWMap>(*this, v.map1(), "PWMap");
  LIB::PWMap map2 = eval<LIB::PWMap>(*this, v.map2(), "PWMap");
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap(), "PWMap");

  return LIB::SBG{V, Vmap, map1, map2, Emap};
}

ExprBaseType ExprEvaluator::operator()(AST::BipartiteSBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V(), "Set");
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap(), "PWMap");
  LIB::PWMap map1 = eval<LIB::PWMap>(*this, v.map1(), "PWMap");
  LIB::PWMap map2 = eval<LIB::PWMap>(*this, v.map2(), "PWMap");
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap(), "PWMap");
  LIB::Set X = eval<LIB::Set>(*this, v.X(), "Set");
  LIB::Set Y = eval<LIB::Set>(*this, v.Y(), "Set");

  return LIB::BipartiteSBG{V, Vmap, map1, map2, Emap, X, Y};
}

ExprBaseType ExprEvaluator::operator()(AST::DSBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V(), "PWMap");
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap(), "PWMap");
  LIB::PWMap mapB = eval<LIB::PWMap>(*this, v.mapB(), "PWMap");
  LIB::PWMap mapD = eval<LIB::PWMap>(*this, v.mapD(), "PWMap");
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap(), "PWMap");

  return LIB::DirectedSBG{V, Vmap, mapB, mapD, Emap};
}

ExprBaseType ExprEvaluator::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace detail

} // namespace Eval

} // namespace SBG
