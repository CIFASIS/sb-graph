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
#include "algorithms/cutvertex/cv_fact.hpp"
#include "algorithms/matching/bfs_matching.hpp"
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/ts_fact.hpp"
#include "algorithms/misc/causalization_builders.hpp"
#include "algorithms/misc/causalization_json.hpp"
#include "eval/visitors/expr_evaluator.hpp"
#include "eval/visitors/func_evaluator.hpp"
#include "eval/visitors/linear_expr_evaluator.hpp"
#include "eval/visitors/nat_evaluator.hpp"
#include "eval/visitors/rational_evaluator.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Expression evaluator --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

template<typename T>
T eval(const ExprEvaluator &visit, AST::Expr e, std::string t = "UNDEF")
{
  ExprBaseType visited = boost::apply_visitor(visit, e);

  Util::ERROR_UNLESS(std::holds_alternative<T>(visited)
    , "ExprEvaluator: expression ", e, " is not of type ", t, "\n");

  return std::get<T>(visited);
}

ExprEvaluator::ExprEvaluator(EvalContext&& eval_ctx) : eval_ctx_(eval_ctx)
{
  // Set built-in functions
  //fenv_.insert("minus", BuiltInOperators::oppositeEvaluator);
  //fenv_.insert("#", BuiltInOperators::cardinalEvaluator);
  //fenv_.insert("'", BuiltInOperators::complementEvaluator);
  //fenv_.insert("+", BuiltInOperators::addEvaluator);
  //fenv_.insert("-", BuiltInOperators::subEvaluator);

  //BuiltInFunctions::setMatching(LIB::BFSMatching);
  //fenv_.insert("isEmpty", BuiltInFunctions::emptyEvaluator);
  //fenv_.insert("minElem", BuiltInFunctions::minEvaluator);
  //fenv_.insert("maxElem", BuiltInFunctions::maxEvaluator);
  //fenv_.insert("compose", BuiltInFunctions::composeEvaluator);
  //fenv_.insert("inv", BuiltInFunctions::inverseEvaluator);
  //fenv_.insert("image", BuiltInFunctions::imageEvaluator);
  //fenv_.insert("preImage", BuiltInFunctions::preImageEvaluator);
  //fenv_.insert("dom", BuiltInFunctions::domEvaluator);
  //fenv_.insert("combine", BuiltInFunctions::combineEvaluator);
  //fenv_.insert("firstInv", BuiltInFunctions::firstInvEvaluator);
  //fenv_.insert("minMap", BuiltInFunctions::minMapEvaluator);
  //fenv_.insert("reduce", BuiltInFunctions::reduceEvaluator);
  //fenv_.insert("minAdj", BuiltInFunctions::minAdjEvaluator);
  //fenv_.insert("mapInf", BuiltInFunctions::mapInfEvaluator);
  //fenv_.insert("cc", BuiltInFunctions::connectedEvaluator);
  //fenv_.insert("match", BuiltInFunctions::matchingEvaluator);
  //fenv_.insert("scc", BuiltInFunctions::sccEvaluator);
  //fenv_.insert("sort", BuiltInFunctions::topoSortEvaluator);
  //fenv_.insert("cut", BuiltInFunctions::cutVertexEvaluator);
  //fenv_.insert("",);
  //fenv_.insert("",);
}

ExprBaseType ExprEvaluator::operator()(AST::Natural v) const
{
  return (LIB::NAT) v;
}

ExprBaseType ExprEvaluator::operator()(AST::Rational v) const
{
  return boost::apply_visitor(RationalEvaluator(ctx_eval_.venv()), AST::Expr(v));
}

ExprBaseType ExprEvaluator::operator()(AST::Name v) const 
{
  auto var_definition = ctx_eval_.venv().find(v);
  if (var_definition != ctx_eval_.venv().end()) { 
    return var_definition->second;
  }

  Util::ERROR("ExprEvaluator: variable ", v, " undefined\n");
  return ExprBaseType(); 
}

ExprBaseType ExprEvaluator::operator()(AST::UnaryOp v) const
{
  ExprBaseType x = boost::apply_visitor(*this, v.expr());

  /*
  switch (v.op()) {
    case AST::UnOp::oppo:
      return std::visit(oppo_visitor_, x);
      break;

    case AST::UnOp::card:
      return std::visit(cardinal_visitor_, x);
      break;

    case AST::UnOp::comp:
      return std::visit(complement_visitor_, x);
      break;

    default:
      Util::ERROR("ExprEvaluator: UnaryOp ", v.op(), " unsupported\n");
      return ExprBaseType();
  }
  */

  return ExprBaseType(); 
}

ExprBaseType ExprEvaluator::operator()(AST::BinOp v) const
{ 
  ExprBaseType vl = boost::apply_visitor(*this, v.left());
  ExprBaseType vr = boost::apply_visitor(*this, v.right());

  NatEvaluator visit_nat(ctx_eval_.venv());
  /*
  switch (v.op()) {
    case AST::Op::add:
      return std::visit(add_visitor_, vl, vr);
      break;

    case AST::Op::sub:
      return std::visit(sub_visitor_, vl, vr);
      break;

    case AST::Op::mult:
      return std::visit(mult_visitor_, vl, vr);
      break;

    case AST::Op::eq:
      return std::visit(eq_visitor_, vl, vr);
      break;

    case AST::Op::less:
      return std::visit(less_visitor_, vl, vr);
      break;

    case AST::Op::cap:
      return std::visit(cap_visitor_, vl, vr);
      break;

    case AST::Op::cup:
      return std::visit(cup_visitor_, vl, vr);
      break;

    case AST::Op::diff:
      return std::visit(diff_visitor_, vl, vr);
      break;

    default:
      Util::ERROR("ExprEvaluator: BinOp ", v.op(), " unsupported\n");
      return ExprBaseType(); 
  } 
  */

  return ExprBaseType(); 
}

ExprBaseType ExprEvaluator::operator()(AST::Call v) const
{
  std::string func_name = v.name();

  auto func_definition = fenv_.find(func_name);
  if (func_definition != fenv_.end()) { 
    std::vector<ExprBaseType> evaluated_args;
    for (AST::Expr a : v.args()) {
      evaluated_args.push_back(boost::apply_visitor(*this, a));
    }

    return func_definition->second(evaluated_args);
  }

  Util::ERROR("ExprEvaluator: function ", func_name, " doesn't exist\n");
  return ExprBaseType();
}

ExprBaseType ExprEvaluator::operator()(AST::Interval v) const
{
  NatEvaluator visit_nat(ctx_eval_.venv());

  LIB::NAT b = boost::apply_visitor(visit_nat, v.begin());
  LIB::NAT s = boost::apply_visitor(visit_nat, v.step());
  LIB::NAT e = boost::apply_visitor(visit_nat, v.end());

  return LIB::Interval(b, s, e);
}

ExprBaseType ExprEvaluator::operator()(AST::MultiDimInter v) const
{
  LIB::SetPiece res;

  for (const AST::Expr &e : v.intervals()) 
    res.emplaceBack(eval<LIB::Interval>(*this, e, "Interval"));

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "EvalMDI[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}


ExprBaseType ExprEvaluator::operator()(AST::Set v) const
{
  LIB::Set res = fact_.createSet();

  for (const AST::Expr &e : v.pieces())
    res.emplaceBack(eval<LIB::SetPiece>(*this, e, "SetPiece"));

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "ExprEvaluator[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType ExprEvaluator::operator()(AST::LinearExp v) const
{
  LinearExprEvaluator visit_le(ctx_eval_.venv());
  return boost::apply_visitor(visit_le, AST::Expr(v));
}

ExprBaseType ExprEvaluator::operator()(AST::MDLExp v) const
{
  LIB::Exp res;

  LinearExprEvaluator visit_le(ctx_eval_.venv());
  for (const AST::Expr &e : v.exps()) {
    LIB::LExp ith = boost::apply_visitor(visit_le, e);
    res.emplaceBack(ith);
  }

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "ExprEvaluator[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType ExprEvaluator::operator()(AST::LinearMap v) const
{
  LIB::Set d = eval<LIB::Set>(*this, v.dom(), "Set");
  LIB::Exp e = eval<LIB::Exp>(*this, v.lexp(), "Exp");

  LIB::Map res = fact_.createMap(d, e);

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "ExprEvaluator[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType ExprEvaluator::operator()(AST::PWLMap v) const
{
  LIB::PWMap res = fact_.createPWMap();

  for (const AST::Expr &e : v.maps())
    res.emplaceBack(eval<LIB::Map>(*this, e, "Map"));

  Util::ERROR_UNLESS(res.arity() == eval_ctx_.arity() || res.arity() == 0
    , "ExprEvaluator[nmbr_dims = ", eval_ctx_.arity(), "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType ExprEvaluator::operator()(AST::SBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V(), "Set");
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap(), "PWMap");
  LIB::PWMap map1 = eval<LIB::PWMap>(*this, v.map1(), "PWMap");
  LIB::PWMap map2 = eval<LIB::PWMap>(*this, v.map2(), "PWMap");
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap(), "PWMap");
  LIB::PWMap subE = eval<LIB::PWMap>(*this, v.subE_map(), "PWMap");

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &m : Emap) {
      for (const LIB::SetPiece &mdi : m.dom()) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(fact_.createMap(fact_.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::SBG(fact_, V, Vmap, map1, map2, Emap, subE);
}

ExprBaseType ExprEvaluator::operator()(AST::DSBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V(), "PWMap");
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap(), "PWMap");
  LIB::PWMap mapB = eval<LIB::PWMap>(*this, v.mapB(), "PWMap");
  LIB::PWMap mapD = eval<LIB::PWMap>(*this, v.mapD(), "PWMap");
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap(), "PWMap");
  LIB::PWMap subE = eval<LIB::PWMap>(*this, v.subE_map(), "PWMap");

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &m : Emap) {
      for (const LIB::SetPiece &mdi : m.dom()) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(fact_.createMap(fact_.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::DSBG(fact_, V, Vmap, mapB, mapD, Emap, subE);
}

ExprBaseType ExprEvaluator::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
