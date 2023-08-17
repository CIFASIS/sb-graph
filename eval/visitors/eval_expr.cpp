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

// Function visitors -----------------------------------------------------------

auto empty_visitor_ = Overload{
  [](LIB::Interval a) { return isEmpty(a); },
  [](LIB::Set a) { return isEmpty(a); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for isEmpty"); 
    return true; 
  } 
};

auto member_visitor_ = Overload{
  [](Util::NAT a, LIB::Interval b) { return isMember(a, b); },
  [](Util::NAT a, LIB::Set b) { return isMember(a, b); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for isMember"); 
    return false;
  } 
};

auto min_visitor_ = Overload{
  [](LIB::Interval a) { return minElem(a); },
  [](LIB::Set a) { return minElem(a); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for minElem"); 
    return (Util::NAT) 0;
  }
};

auto max_visitor_ = Overload{
  [](LIB::Interval a) { return maxElem(a); },
  [](LIB::Set a) { return maxElem(a); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for maxElem"); 
    return (Util::NAT) 0; 
  }
};

auto lt_visitor_ = Overload{
  [](LIB::Interval a, LIB::Interval b) { return least(a, b); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for lt"); 
    return LIB::Interval(); 
  }
};

auto compose_visitor_ = Overload{
  [](LIB::LExp a, LIB::LExp b) { return ExprBaseType(composition(a, b)); },
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(composition(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for composition"); 
    return ExprBaseType(); }
};

auto inverse_visitor_ = Overload{
  [](LIB::LExp a) { return inverse(a); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for inversion"); 
    return LIB::LExp(); 
  }
};

auto image_visitor1_ = Overload{
  [](LIB::SBGMap a) { return ExprBaseType(image(a)); },
  [](LIB::PWMap a) { return ExprBaseType(image(a)); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for image 1"); 
    return ExprBaseType(); 
  }
};

auto image_visitor2_ = Overload{
  [](LIB::SetPiece a, LIB::SBGMap b) { return ExprBaseType(image(a, b)); },
  [](LIB::Set a, LIB::PWMap b) { return ExprBaseType(image(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for image 2"); 
    return ExprBaseType();
  }
};

auto pre_image_visitor1_ = Overload{
  [](LIB::SBGMap a) { return ExprBaseType(preImage(a)); },
  [](LIB::PWMap a) { return ExprBaseType(preImage(a)); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for pre image 1"); 
    return ExprBaseType(); 
  }
};

auto pre_image_visitor2_ = Overload{
  [](LIB::SetPiece a, LIB::SBGMap b) { return ExprBaseType(preImage(a, b)); },
  [](LIB::Set a, LIB::PWMap b) { return ExprBaseType(preImage(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for pre image 2"); 
    return ExprBaseType(); 
  }
};

auto dom_visitor_ = Overload{
  [](LIB::PWMap a) { return LIB::dom(a); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for dom"); 
    return LIB::Set(); 
  }
};

/*
auto min_adj_visitor_ = Overload{
  [](LIB::SBGMap a, LIB::SBGMap b) { return ExprBaseType(minAdjMap(a, b)); },
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(minAdjMap(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for minAdjMap"); 
    return ExprBaseType(); 
  }
};
*/

// Expression evaluator --------------------------------------------------------

EvalExpression::EvalExpression() : env_() {}
EvalExpression::EvalExpression(VarEnv env) : env_(env) {}

ExprBaseType EvalExpression::operator()(AST::Natural v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::Rational v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::Boolean v) const { return v; }

ExprBaseType EvalExpression::operator()(Util::VariableName v) const 
{
  MaybeEBT v_opt = env_[v];
  if (v_opt)
    return *v_opt;

  Util::ERROR("EvalExpression: variable %s not defined", v.c_str());
  return 0; 
}

ExprBaseType EvalExpression::operator()(AST::UnaryOp v) const { return Apply(EvalRat(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::BinOp v) const { return Apply(EvalRat(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::Call v) const
{
  std::string vname = v.name();

  auto venv = fenv_[vname];
  if (venv) { 
    std::vector<ExprBaseType> eval_args;
    BOOST_FOREACH (AST::Expr a, v.args()) 
      eval_args.push_back(ApplyThis(a));

    bool arity_ok = false;
    switch (*venv) {
      case Eval::Func::empty: 
        if (eval_args.size() == 1) {
          arity_ok = true;

          ExprBaseType container = eval_args[0];
          bool result = std::visit(empty_visitor_, container);
          return result;
        }
        break;

      case Eval::Func::member:  
        if (eval_args.size() == 2) {
          arity_ok = true;

          ExprBaseType x = eval_args[0], container = eval_args[1];
          bool result = std::visit(member_visitor_, x, container);
          return result;
        }
        break;

      case Eval::Func::min:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ExprBaseType container = eval_args[0];
          Util::NAT result = std::visit(min_visitor_, container);
          return result;
        }
        break;

      case Eval::Func::max:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ExprBaseType container = eval_args[0];
          Util::NAT result = std::visit(max_visitor_, container);
          return result;
        }
        break;

      case Eval::Func::lt:
        if (eval_args.size() == 2) {
          arity_ok = true;

          ExprBaseType cont1 = eval_args[0], cont2 = eval_args[1];
          LIB::Interval result = std::visit(lt_visitor_, cont1, cont2);
          return result;
        }
        break;

      case Eval::Func::comp:
        if (eval_args.size() == 2) {
          arity_ok = true;

          ExprBaseType lexp1 = eval_args[0], lexp2 = eval_args[1];
          ExprBaseType result = std::visit(compose_visitor_, lexp1, lexp2);
          return result;
        }
        break;

      case Eval::Func::inv:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ExprBaseType lexp = eval_args[0];
          LIB::LExp result = std::visit(inverse_visitor_, lexp);
          return result;
        }
        break;

      case Eval::Func::im:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ExprBaseType sbgmap = eval_args[0];
          ExprBaseType result = std::visit(image_visitor1_, sbgmap);
          return result;
        }

        else if (eval_args.size() == 2) {
          arity_ok = true;

          ExprBaseType subdom = eval_args[0], sbgmap = eval_args[1];
          ExprBaseType result = std::visit(image_visitor2_, subdom, sbgmap);
          return result;
        }

        break;

      case Eval::Func::preim:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ExprBaseType sbgmap = eval_args[0];
          ExprBaseType result = std::visit(pre_image_visitor1_, sbgmap);
          return result;
        }

        else if (eval_args.size() == 2) {
          arity_ok = true;

          ExprBaseType subdom = eval_args[0], map = eval_args[1];
          ExprBaseType result = std::visit(pre_image_visitor2_, subdom, map);
          return result;
        }
        
        break;


      case Eval::Func::dom:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ExprBaseType pw = eval_args[0]; 
          LIB::Set result = std::visit(dom_visitor_, pw);
          return result;
        }
        break;

      /*
      case Eval::Func::min_adj:
        if (eval_args.size() == 2) {
          arity_ok = true;

          ExprBaseType map1 = eval_args[0], map2 = eval_args[1];
          ExprBaseType result = std::visit(min_adj_visitor_, map1, map2);
          return result;
        }
        break;
      */

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
  EvalInterval eval_interval(env_);
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(Apply(eval_interval, exp));

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
  EvalInterval eval_interval(env_);
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(Apply(eval_interval, l), Apply(eval_interval, r));

    case AST::ContainerOp::less:
      return Apply(eval_interval, l) < Apply(eval_interval, r);

    case AST::ContainerOp::eq:
      return Apply(eval_interval, l) == Apply(eval_interval, r);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: InterBinOp %s not supported.", ss.str().c_str());
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::Set v) const { return Apply(EvalSet(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::SetUnaryOp v) const
{
  AST::Expr exp = v.e();
  EvalSet eval_set(env_);
  LIB::Set eval_exp = Apply(eval_set, exp);
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(eval_exp);

    case AST::ContainerUOp::comp:
      return complement(eval_exp);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetUnaryOp %s not supported.", ss.str().c_str());
      return 0;
  }
}

ExprBaseType EvalExpression::operator()(AST::SetBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalSet eval_set(env_);
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(Apply(eval_set, l), Apply(eval_set, r));

    case AST::ContainerOp::diff:
      return difference(Apply(eval_set, l), Apply(eval_set, r));

    case AST::ContainerOp::eq:
      return Apply(eval_set, l) == Apply(eval_set, r);

    case AST::ContainerOp::cup:
      return cup(Apply(eval_set, l), Apply(eval_set, r));

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetBinOp %s not supported.", ss.str().c_str());
      return 0;
  }
}

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

ExprBaseType EvalExpression::operator()(AST::LinearMap v) const { return Apply(EvalMap(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::PWLMap v) const { return Apply(EvalPWMap(env_), AST::Expr(v)); }

} // namespace Eval

} // namespace SBG
