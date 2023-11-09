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

#include "eval/visitors/eval_expr.hpp"

namespace SBG {

namespace Eval {

// -----------------------------------------------------------------------------
// Expression evaluator --------------------------------------------------------
// -----------------------------------------------------------------------------

EvalExpression::EvalExpression() : nmbr_dims_(1), env_() {}
EvalExpression::EvalExpression(VarEnv env) : nmbr_dims_(1), env_(env) {}
EvalExpression::EvalExpression(unsigned int nmbr_dims, VarEnv env) : nmbr_dims_(nmbr_dims), env_(env) {}

ExprBaseType EvalExpression::operator()(AST::Natural v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::MDNatural v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::Rational v) const { return Apply(EvalRat(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::Boolean v) const { return v; }

ExprBaseType EvalExpression::operator()(Util::VariableName v) const 
{
  MaybeEBT v_opt = env_[v];
  if (v_opt)
    return *v_opt;

  Util::ERROR("EvalExpression: variable %s not defined", v.c_str());
  return 0; 
}

ExprBaseType EvalExpression::operator()(AST::UnaryOp v) const
{
  ExprBaseType x = ApplyThis(v.expr());

  if (is<Util::NAT>(x)) return Apply(EvalNat(env_), AST::Expr(v));

  return Apply(EvalRat(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::BinOp v) const
{ 
  ExprBaseType xl = ApplyThis(v.left()), xr = ApplyThis(v.right());
  bool r1 = is<Util::RATIONAL>(xl), r2 = is<Util::RATIONAL>(xr);

  if (!r1 && !r2) return Apply(EvalNat(env_), AST::Expr(v));

  return Apply(EvalRat(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::Call v) const
{
  std::string vname = v.name();

  auto venv = fenv_[vname];
  if (venv) { 
    AST::ExprList args = v.args();
    std::size_t args_size = v.args().size();
    bool arity_ok = false;
    switch (*venv) {
      case Eval::Func::empty: 
        if (args_size == 1) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          return isEmpty(Apply(visit_set, args[0]));
        }
        break;

      case Eval::Func::member:  
        if (args_size == 2) {
          arity_ok = true;

          EvalMDNat visit_mdnat(env_);
          Util::MD_NAT x = Apply(visit_mdnat, args[0]);
          EvalSet visit_set(nmbr_dims_, env_);
          LIB::Set s = Apply(visit_set, args[1]);
          return isMember(x, s);
        }
        break;

      case Eval::Func::min:
        if (args_size == 1) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          return minElem(Apply(visit_set, args[0]));
        }
        break;

      case Eval::Func::max:
        if (args_size == 1) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          return maxElem(Apply(visit_set, args[0]));
        }
        break;

      case Eval::Func::comp:
        if (args_size == 2) {
          arity_ok = true;

          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          LIB::PWMap pw1 = Apply(visit_pwmap, args[0]);
          LIB::PWMap pw2 = Apply(visit_pwmap, args[1]);
          return composition(pw1, pw2);
        }
        break;

      case Eval::Func::inv:
        if (args_size == 1) {
          arity_ok = true;

          EvalMDLE visit_mdle(env_);
          return inverse(Apply(visit_mdle, args[0]));
        }
        break;

      case Eval::Func::im:
        if (args_size == 1) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          return image(Apply(visit_set, args[0]));
        }

        else if (args_size == 2) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          LIB::Set s = Apply(visit_set, args[1]);
          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          LIB::PWMap pw = Apply(visit_pwmap, args[0]);
          return image(s, pw);
        }

        break;

      case Eval::Func::preim:
        if (args_size == 1) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          return preImage(Apply(visit_set, args[0]));
        }

        else if (args_size == 2) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          LIB::Set s = Apply(visit_set, args[1]);
          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          LIB::PWMap pw = Apply(visit_pwmap, args[0]);
          return preImage(s, pw);
        }

        break;

      case Eval::Func::dom:
        if (args_size == 1) {
          arity_ok = true;

          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          return LIB::dom(Apply(visit_pwmap, args[0]));
        }
        break;

      case Eval::Func::comb:
        if (args_size == 2) {
          arity_ok = true;

          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          LIB::PWMap pw1 = Apply(visit_pwmap, args[0]);
          LIB::PWMap pw2 = Apply(visit_pwmap, args[1]);
          return combine(pw1, pw2);
        }
        break;

      case Eval::Func::min_map:
        if (args_size == 2) {
          arity_ok = true;

          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          LIB::PWMap pw1 = Apply(visit_pwmap, args[0]);
          LIB::PWMap pw2 = Apply(visit_pwmap, args[1]);
          return minMap(pw1, pw2);
        }

        if (args_size == 3) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          LIB::Set s = Apply(visit_set, args[0]);
          EvalMDLE visit_mdle(env_);
          LIB::Exp e1 = Apply(visit_mdle, args[1]);
          LIB::Exp e2 = Apply(visit_mdle, args[2]);
          return minMap(s, e1, e2);
        }

        if (args_size == 4) {
          arity_ok = true;

          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          LIB::PWMap pw1 = Apply(visit_pwmap, args[0]);
          LIB::PWMap pw2 = Apply(visit_pwmap, args[1]);
          LIB::PWMap pw3 = Apply(visit_pwmap, args[2]);
          LIB::PWMap pw4 = Apply(visit_pwmap, args[3]);
          return minMap(pw1, pw2, pw3, pw4);
        }

        if (args_size == 5) {
          arity_ok = true;

          EvalSet visit_set(nmbr_dims_, env_);
          LIB::Set s = Apply(visit_set, args[0]);
          EvalMDLE visit_mdle(env_);
          LIB::Exp e1 = Apply(visit_mdle, args[1]);
          LIB::Exp e2 = Apply(visit_mdle, args[2]);
          LIB::Exp e3 = Apply(visit_mdle, args[3]);
          LIB::Exp e4 = Apply(visit_mdle, args[4]);
          return minMap(s, e1, e2, e3, e4);
        }
        break;

      case Eval::Func::red:
        if (args_size == 1) {
          arity_ok = true;

          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          return reduce(Apply(visit_pwmap, args[0]));
        }
        break;

      case Eval::Func::min_adj:
        if (args_size == 2) {
          arity_ok = true;

          EvalPWMap visit_pwmap(nmbr_dims_, env_);
          LIB::PWMap pw1 = Apply(visit_pwmap, args[0]);
          LIB::PWMap pw2 = Apply(visit_pwmap, args[1]);
          return minAdjMap(pw1, pw2);
        }
        break;

      case Eval::Func::connected:
        if (args_size == 1) {
          arity_ok = true;

          EvalGraph visit_graph(nmbr_dims_, env_);
          LIB::SBGraph g = Apply(visit_graph, args[0]).sbg();
          return connectedComponents(g);
        }
        break;

      case Eval::Func::matching:
        if (args_size == 1) {
          arity_ok = true;

          EvalGraph visit_graph(nmbr_dims_, env_);
          LIB::SBGraph g = Apply(visit_graph, args[0]).sbg();
          return LIB::SBGMatching(g).calculate().matched_edges();
        }
        break;

      default:
        Util::ERROR("EvalExpression: function %s not implemented", vname.c_str());
        return 0;
    }

    if (!arity_ok) {
      Util::ERROR("EvalExpression: wrong number of arguments for Call");
      return 0;
    }
  }

  Util::ERROR("EvalExpression: function %s does not exist", vname.c_str());
  return 0;
}

ExprBaseType EvalExpression::operator()(AST::Interval v) const { return Apply(EvalInterval(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::InterUnaryOp v) const
{
  AST::Expr exp = v.e();
  EvalInterval visit_interval(env_);
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(Apply(visit_interval, exp));

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: InterUnaryOp %s not supported.", ss.str().c_str());
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::InterBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalInterval visit_interval(env_);
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(Apply(visit_interval, l), Apply(visit_interval, r));

    case AST::ContainerOp::less:
      return Apply(visit_interval, l) < Apply(visit_interval, r);

    case AST::ContainerOp::eq:
      return Apply(visit_interval, l) == Apply(visit_interval, r);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: InterBinOp %s not supported.", ss.str().c_str());
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::MultiDimInter v) const { return Apply(EvalMDI(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::MDInterUnaryOp v) const
{
  EvalMDI visit_mdi(env_);
  LIB::MultiDimInter mdi = Apply(visit_mdi, v.e());
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(mdi);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: MDInterUnaryOp %s not supported.", ss.str().c_str());
      return 0;
  }

  return 0;
}

ExprBaseType EvalExpression::operator()(AST::MDInterBinOp v) const
{
  EvalMDI visit_mdi(env_);
  LIB::MultiDimInter l = Apply(visit_mdi, v.left()), r = Apply(visit_mdi, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(l, r);
      break;

    case AST::ContainerOp::less:
      return l < r;
      break;

    case AST::ContainerOp::eq:
      return l == r;
      break;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: MDInterBinOp %s not supported.", ss.str().c_str());
      return 0;
      break;
  }

  return 0;
}

ExprBaseType EvalExpression::operator()(AST::Set v) const { return Apply(EvalSet(nmbr_dims_, env_), AST::Expr(v)); }

// ----- //

ExprBaseType EvalExpression::operator()(AST::SetUnaryOp v) const
{
  EvalSet visit_set(nmbr_dims_, env_);
  LIB::Set s = Apply(visit_set, v.e());
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(s);
      break;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetUnaryOp %s not supported.", ss.str().c_str());
      return 0;
      break;
  }

  return 0;
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::SetBinOp v) const
{
  EvalSet visit_set(nmbr_dims_, env_);
  LIB::Set l = Apply(visit_set, v.left()), r = Apply(visit_set, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(l, r);

    case AST::ContainerOp::cup:
      return cup(l, r);

    case AST::ContainerOp::diff:
      return difference(l, r);

    case AST::ContainerOp::less:
      return l < r;
      break;

    case AST::ContainerOp::eq:
      return l == r;
      break;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: MDInterBinOp %s not supported.", ss.str().c_str());
      return 0;
      break;
  }

  return 0;
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::LinearExp v) const { return Apply(EvalLE(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::LExpBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalLE visit_le(env_);
  LIB::LExp lexp = Apply(visit_le, l), rexp = Apply(visit_le, r);
  switch (v.op()) {
    case AST::ExpOp::eq:
      return lexp == rexp;

    case AST::ExpOp::add:
      return lexp + rexp;

    case AST::ExpOp::sub:
      return lexp - rexp;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: LExpBinOp %s not supported.", ss.str().c_str());
      return 0;
  }
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::MDLExp v) const { return Apply(EvalMDLE(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::MDLExpBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalMDLE visit_le(env_);
  LIB::Exp lexp = Apply(visit_le, l), rexp = Apply(visit_le, r);
  switch (v.op()) {
    case AST::ExpOp::eq:
      return lexp == rexp;

    case AST::ExpOp::add:
      return lexp + rexp;

    case AST::ExpOp::sub:
      return lexp - rexp;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: MDLExpBinOp %s not supported.", ss.str().c_str());
      return 0;
  }

  return 0;
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::LinearMap v) const
{
  return Apply(EvalMap(nmbr_dims_, env_), AST::Expr(v));
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::PWLMap v) const
{
  return Apply(EvalPWMap(nmbr_dims_, env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::SBG v) const
{
  EvalGraphType res = Apply(EvalGraph(nmbr_dims_, env_), AST::Expr(v));
  return res.sbg();
}

ExprBaseType EvalExpression::operator()(AST::DSBG v) const
{
  EvalGraphType res = Apply(EvalGraph(nmbr_dims_, env_), AST::Expr(v));
  return res.dsbg();
}

} // namespace Eval

} // namespace SBG
