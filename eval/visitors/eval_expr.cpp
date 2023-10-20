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
// Function visitors -----------------------------------------------------------
// -----------------------------------------------------------------------------

auto empty_visitor_ = Util::Overload {
  [](LIB::Interval a) { return isEmpty(a); },
  [](LIB::MultiDimInter a) { return isEmpty(a); },
  [](LIB::UnordSet a) { return isEmpty(a); },
  [](LIB::OrdSet a) { return isEmpty(a); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for isEmpty"); 
    return true; 
  } 
};

auto member_visitor_ = Util::Overload {
  [](Util::NAT a, LIB::Interval b) { return isMember(a, b); },
  [](Util::NAT a, LIB::MultiDimInter b) { return isMember(Util::MD_NAT(a), b); },
  [](Util::MD_NAT a, LIB::MultiDimInter b) { return isMember(a, b); },
  [](Util::NAT a, LIB::UnordSet b) { return isMember(Util::MD_NAT(a), b); },
  [](Util::NAT a, LIB::OrdSet b) { return isMember(Util::MD_NAT(a), b); },
  [](Util::MD_NAT a, LIB::UnordSet b) { return isMember(a, b); },
  [](Util::MD_NAT a, LIB::OrdSet b) { return isMember(a, b); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for isMember"); 
    return false;
  } 
};

auto min_visitor_ = Util::Overload {
  [](LIB::Interval a) { return ExprBaseType(minElem(a)); },
  [](LIB::MultiDimInter a) { return ExprBaseType(minElem(a)); },
  [](LIB::UnordSet a) { return ExprBaseType(minElem(a)); },
  [](LIB::OrdSet a) { return ExprBaseType(minElem(a)); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for minElem"); 
    return ExprBaseType();
  }
};

auto max_visitor_ = Util::Overload {
  [](LIB::Interval a) { return ExprBaseType(maxElem(a)); },
  [](LIB::MultiDimInter a) { return ExprBaseType(maxElem(a)); },
  [](LIB::UnordSet a) { return ExprBaseType(maxElem(a)); },
  [](LIB::OrdSet a) { return ExprBaseType(maxElem(a)); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for maxElem"); 
    return ExprBaseType(); 
  }
};

auto lt_visitor_ = Util::Overload {
  [](LIB::Interval a, LIB::Interval b) { return ExprBaseType(least(a, b)); },
  [](auto a, auto b) {
    Util::ERROR("Wrong arguments for lt"); 
    return ExprBaseType(); 
  }
};

auto compose_visitor_ = Util::Overload {
  [](LIB::LExp a, LIB::LExp b) { return ExprBaseType(composition(a, b)); },
  [](LIB::Exp a, LIB::Exp b) { return ExprBaseType(composition(a, b)); },
  [](LIB::BaseMap a, LIB::BaseMap b) { return ExprBaseType(composition(a, b)); },
  [](LIB::CanonMap a, LIB::CanonMap b) { return ExprBaseType(composition(a, b)); },
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return ExprBaseType(composition(a, b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) { return ExprBaseType(composition(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for composition"); 
    return ExprBaseType(); }
};

auto inverse_visitor_ = Util::Overload {
  [](LIB::LExp a) { return ExprBaseType(inverse(a)); },
  [](LIB::Exp a) { return ExprBaseType(inverse(a)); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for inversion"); 
    return ExprBaseType();
  }
};

auto image_visitor1_ = Util::Overload {
  [](LIB::BaseMap a) { return ExprBaseType(image(a)); },
  [](LIB::CanonMap a) { return ExprBaseType(image(a)); },
  [](LIB::BasePWMap a) { return ExprBaseType(image(a)); },
  [](LIB::CanonPWMap a) { return ExprBaseType(image(a)); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for image 1"); 
    return ExprBaseType(); 
  }
};

auto image_visitor2_ = Util::Overload {
  [](LIB::UnordSet a, LIB::BaseMap b) { return ExprBaseType(image(a, b)); },
  [](LIB::OrdSet a, LIB::CanonMap b) { return ExprBaseType(image(a, b)); },
  [](LIB::UnordSet a, LIB::BasePWMap b) { return ExprBaseType(image(a, b)); },
  [](LIB::OrdSet a, LIB::CanonPWMap b) { return ExprBaseType(image(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for image 2"); 
    return ExprBaseType();
  }
};

auto pre_image_visitor1_ = Util::Overload {
  [](LIB::BaseMap a) { return ExprBaseType(preImage(a)); },
  [](LIB::CanonMap a) { return ExprBaseType(preImage(a)); },
  [](LIB::BasePWMap a) { return ExprBaseType(preImage(a)); },
  [](LIB::CanonPWMap a) { return ExprBaseType(preImage(a)); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for pre image 1"); 
    return ExprBaseType(); 
  }
};

auto pre_image_visitor2_ = Util::Overload {
  [](LIB::UnordSet a, LIB::BaseMap b) { return ExprBaseType(preImage(a, b)); },
  [](LIB::OrdSet a, LIB::CanonMap b) { return ExprBaseType(preImage(a, b)); },
  [](LIB::UnordSet a, LIB::BasePWMap b) { return ExprBaseType(preImage(a, b)); },
  [](LIB::OrdSet a, LIB::CanonPWMap b) { return ExprBaseType(preImage(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for pre image 2"); 
    return ExprBaseType(); 
  }
};

auto dom_visitor_ = Util::Overload {
  [](LIB::BasePWMap a) { return ExprBaseType(LIB::dom(a)); },
  [](LIB::CanonPWMap a) { return ExprBaseType(LIB::dom(a)); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for dom"); 
    return ExprBaseType(); 
  }
};

auto combine_visitor_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return ExprBaseType(combine(a, b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) { return ExprBaseType(combine(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for combine"); 
    return ExprBaseType(); 
  }
};

auto min_map_visitor2_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return ExprBaseType(minMap(a, b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) { return ExprBaseType(minMap(a, b)); },
  [](auto a, auto b) {
    Util::ERROR("Wrong arguments for minMap2");
    return ExprBaseType();
  }
};

auto min_map_unord_visitor3_ = Util::Overload {
  [](LIB::Interval a, LIB::LExp b, LIB::LExp c) { return ExprBaseType(LIB::minMap<LIB::UnordSet>(a, b, c)); },
  [](LIB::SetPiece a, LIB::Exp b, LIB::Exp c) { return ExprBaseType(LIB::minMap<LIB::UnordSet>(a, b, c)); },
  [](LIB::UnordSet a, LIB::Exp b, LIB::Exp c) { return ExprBaseType(LIB::minMap<LIB::UnordSet>(a, b, c)); },
  [](auto a, auto b, auto c) {
    Util::ERROR("Wrong arguments for minMap3");
    return ExprBaseType();
  }
};

auto min_map_ord_visitor3_ = Util::Overload {
  [](LIB::Interval a, LIB::LExp b, LIB::LExp c) { return ExprBaseType(LIB::minMap<LIB::OrdSet>(a, b, c)); },
  [](LIB::SetPiece a, LIB::Exp b, LIB::Exp c) { return ExprBaseType(LIB::minMap<LIB::OrdSet>(a, b, c)); },
  [](LIB::OrdSet a, LIB::Exp b, LIB::Exp c) { return ExprBaseType(LIB::minMap<LIB::OrdSet>(a, b, c)); },
  [](auto a, auto b, auto c) {
    Util::ERROR("Wrong arguments for minMap3");
    return ExprBaseType();
  }
};

auto min_map_visitor4_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b, LIB::BasePWMap c, LIB::BasePWMap d) {
    return ExprBaseType(minMap(a, b, c, d)); 
  },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b, LIB::CanonPWMap c, LIB::CanonPWMap d) {
    return ExprBaseType(minMap(a, b, c, d));
  },
  [](auto a, auto b, auto c, auto d) {
    Util::ERROR("Wrong arguments for minMap4");
    return ExprBaseType();
  }
};

auto min_map_unord_visitor5_ = Util::Overload {
  [](LIB::Interval a, LIB::LExp b, LIB::LExp c, LIB::LExp d, LIB::LExp e) {
    return ExprBaseType(LIB::minMap<LIB::UnordSet>(a, b, c, d, e));
  },
  [](LIB::SetPiece a, LIB::Exp b, LIB::Exp c, LIB::Exp d, LIB::Exp e) {
    return ExprBaseType(LIB::minMap<LIB::UnordSet>(a, b, c, d, e));
  },
  [](LIB::UnordSet a, LIB::Exp b, LIB::Exp c, LIB::Exp d, LIB::Exp e) {
    return ExprBaseType(LIB::minMap<LIB::UnordSet>(a, b, c, d, e));
  },
  [](auto a, auto b, auto c, auto d, auto e) {
    Util::ERROR("Wrong arguments for minMap5");
    return ExprBaseType();
  }
};

auto min_map_ord_visitor5_ = Util::Overload {
  [](LIB::Interval a, LIB::LExp b, LIB::LExp c, LIB::LExp d, LIB::LExp e) {
    return ExprBaseType(LIB::minMap<LIB::OrdSet>(a, b, c, d, e));
  },
  [](LIB::SetPiece a, LIB::Exp b, LIB::Exp c, LIB::Exp d, LIB::Exp e) {
    return ExprBaseType(LIB::minMap<LIB::OrdSet>(a, b, c, d, e));
  },
  [](LIB::OrdSet a, LIB::Exp b, LIB::Exp c, LIB::Exp d, LIB::Exp e) {
    return ExprBaseType(LIB::minMap<LIB::OrdSet>(a, b, c, d, e));
  },
  [](auto a, auto b, auto c, auto d, auto e) {
    Util::ERROR("Wrong arguments for minMap5");
    return ExprBaseType();
  }
};

auto reduce_visitor_ = Util::Overload {
  [](LIB::BasePWMap a) { return ExprBaseType(reduce(a)); },
  [](LIB::CanonPWMap a) { return ExprBaseType(reduce(a)); },
  [](auto a) {
    Util::ERROR("Wrong arguments for reduce");
    return ExprBaseType();
  }
};


auto min_adj_visitor2_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return ExprBaseType(minAdjMap(a, b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) { return ExprBaseType(minAdjMap(a, b)); },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for minAdjMap"); 
    return ExprBaseType(); 
  }
};

auto connected_visitor_ = Util::Overload {
  [](LIB::BaseSBG a) { return ExprBaseType(connectedComponents(a)); },
  [](LIB::CanonSBG a) { return ExprBaseType(connectedComponents(a)); },
  [](auto a) {
    Util::ERROR("Wrong arguments for connectedComponents");
    return ExprBaseType();
  }
};

auto min_reach_visitor_ = Util::Overload {
  [](LIB::BaseDSBG a) { 
    LIB::BaseMR mr(a);
    return ExprBaseType(mr.calculate().reps()); 
  },
  [](LIB::CanonDSBG a) {
    LIB::CanonMR mr(a);
    return ExprBaseType(mr.calculate().reps()); 
  },
  [](auto a) {
    Util::ERROR("Wrong arguments for minReach");
    return ExprBaseType();
  }
};


// -----------------------------------------------------------------------------
// Expression evaluator --------------------------------------------------------
// -----------------------------------------------------------------------------

EvalExpression::EvalExpression() : nmbr_dims_(1), env_() {}
EvalExpression::EvalExpression(VarEnv env) : nmbr_dims_(1), env_(env) {}
EvalExpression::EvalExpression(unsigned int nmbr_dims, VarEnv env) : nmbr_dims_(nmbr_dims), env_(env) {}

ExprBaseType EvalExpression::operator()(AST::Natural v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::MDNatural v) const { return v; }

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
    std::vector<ExprBaseType> eval_args;
    BOOST_FOREACH (AST::Expr a, v.args()) 
      eval_args.push_back(ApplyThis(a));

    bool arity_ok = false;
    switch (*venv) {
      case Eval::Func::empty: 
        if (eval_args.size() == 1) {
          arity_ok = true;

          ContainerBaseType container = Apply(EvalContainer{}, eval_args[0]);
          bool result = std::visit(empty_visitor_, container);
          return result;
        }
        break;

      case Eval::Func::member:  
        if (eval_args.size() == 2) {
          arity_ok = true;

          ContainerBaseType container = Apply(EvalContainer{}, eval_args[1]);
          NatBaseType x = Apply(EvalNatBT{}, eval_args[0]);
          bool result = std::visit(member_visitor_, x, container);
          return result;
        }
        break;

      case Eval::Func::min:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ContainerBaseType container = Apply(EvalContainer{}, eval_args[0]);
          ExprBaseType result = std::visit(min_visitor_, container);
          return result;
        }
        break;

      case Eval::Func::max:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ContainerBaseType container = Apply(EvalContainer{}, eval_args[0]);
          ExprBaseType result = std::visit(max_visitor_, container);
          return result;
        }
        break;

      case Eval::Func::lt:
        if (eval_args.size() == 2) {
          arity_ok = true;

          EvalContainer visit_cont;
          ContainerBaseType cont1 = Apply(visit_cont, eval_args[0]), cont2 = Apply(visit_cont, eval_args[1]);
          ExprBaseType result = std::visit(lt_visitor_, cont1, cont2);
          return result;
        }
        break;

      case Eval::Func::comp:
        if (eval_args.size() == 2) {
          arity_ok = true;

          ExprBaseType result;
          if (is<LIB::LExp>(eval_args[0]) || is<LIB::Exp>(eval_args[0])){ 
            EvalLinear visit_linear;
            LinearBaseType e1 = Apply(visit_linear, eval_args[0]), e2  = Apply(visit_linear, eval_args[1]);
            result = std::visit(compose_visitor_, e1, e2);
          }

          else{
            EvalMap visit_map;
            MapBaseType m1 = Apply(visit_map, eval_args[0]), m2 = Apply(visit_map, eval_args[1]);
            result = std::visit(compose_visitor_, m1, m2);
          }

          return result;
        }
        break;

      case Eval::Func::inv:
        if (eval_args.size() == 1) {
          arity_ok = true;

          LinearBaseType lexp = Apply(EvalLinear(), eval_args[0]);
          ExprBaseType result = std::visit(inverse_visitor_, lexp);
          return result;
        }
        break;

      case Eval::Func::im:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType sbgmap = Apply(EvalMap(), eval_args[0]);
          ExprBaseType result = std::visit(image_visitor1_, sbgmap);
          return result;
        }

        else if (eval_args.size() == 2) {
          arity_ok = true;

          ContainerBaseType subdom = Apply(EvalContainer{}, eval_args[0]);
          MapBaseType sbgmap = Apply(EvalMap(), eval_args[1]);
          ExprBaseType result = std::visit(image_visitor2_, subdom, sbgmap);
          return result;
        }

        break;

      case Eval::Func::preim:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType sbgmap = Apply(EvalMap(), eval_args[0]);
          ExprBaseType result = std::visit(pre_image_visitor1_, sbgmap);
          return result;
        }

        else if (eval_args.size() == 2) {
          arity_ok = true;

          ContainerBaseType subdom = Apply(EvalContainer{}, eval_args[0]);
          MapBaseType map = Apply(EvalMap(), eval_args[1]);
          ExprBaseType result = std::visit(pre_image_visitor2_, subdom, map);
          return result;
        }
        
        break;

      case Eval::Func::dom:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType pw = Apply(EvalMap(), eval_args[0]); 
          ExprBaseType result = std::visit(dom_visitor_, pw);
          return result;
        }
        break;

      case Eval::Func::comb:
        if (eval_args.size() == 2) {
          arity_ok = true;
 
          MapBaseType pw1 = Apply(EvalMap(), eval_args[0]), pw2 = Apply(EvalMap(), eval_args[1]);
          ExprBaseType result = std::visit(combine_visitor_, pw1, pw2);
          return result;
        }
        break;

      case Eval::Func::min_map:
        if (eval_args.size() == 2) {
          arity_ok = true;

          EvalMap visit_map;
          MapBaseType pw1 = Apply(visit_map, eval_args[0]), pw2 = Apply(visit_map, eval_args[1]);
          ExprBaseType result = std::visit(min_map_visitor2_, pw1, pw2);
          return result;
        }

        if (eval_args.size() == 3) {
          arity_ok = true;

          ContainerBaseType container = Apply(EvalContainer{}, eval_args[0]);
          EvalLinear visit_linear;
          LinearBaseType e1 = Apply(visit_linear, eval_args[1]), e2 = Apply(visit_linear, eval_args[2]);

          ExprBaseType result;
          if (nmbr_dims_ == 1) result = std::visit(min_map_ord_visitor3_, container, e1, e2);
          else result = std::visit(min_map_unord_visitor3_, container, e1, e2);

          return result;
        }

        if (eval_args.size() == 4) {
          arity_ok = true;

          EvalMap visit_map;
          MapBaseType pw1 = Apply(visit_map, eval_args[0]), pw2 = Apply(visit_map, eval_args[1]);
          MapBaseType pw3 = Apply(visit_map, eval_args[2]), pw4 = Apply(visit_map, eval_args[3]);

          ExprBaseType result = std::visit(min_map_visitor4_, pw1, pw2, pw3, pw4);
          return result;
        }

        if (eval_args.size() == 5) {
          arity_ok = true;

          ContainerBaseType container = Apply(EvalContainer{}, eval_args[0]);
          EvalLinear visit_linear;
          LinearBaseType e1 = Apply(visit_linear, eval_args[1]), e2 = Apply(visit_linear, eval_args[2]);
          LinearBaseType e3 = Apply(visit_linear, eval_args[3]), e4 = Apply(visit_linear, eval_args[4]);
          ExprBaseType result;

          if (nmbr_dims_ == 1) result = std::visit(min_map_ord_visitor5_, container, e1, e2, e3, e4);
          else result = std::visit(min_map_unord_visitor5_, container, e1, e2, e3, e4);

          return result;
        }
        break;

      case Eval::Func::red:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType map = Apply(EvalMap{}, eval_args[0]);
          ExprBaseType result = std::visit(reduce_visitor_, map);
          return result;
        }
        break;

      case Eval::Func::min_adj:
        if (eval_args.size() == 2) {
          arity_ok = true;

          MapBaseType map1 = Apply(EvalMap{}, eval_args[0]), map2 = Apply(EvalMap{}, eval_args[1]);
          ExprBaseType result = std::visit(min_adj_visitor2_, map1, map2);
          return result;
        }
        break;

      case Eval::Func::connected:
        if (eval_args.size() == 1) {
          arity_ok = true;

          SBGBaseType g = Apply(EvalGraph{}, eval_args[0]);
          ExprBaseType result = std::visit(connected_visitor_, g);
          return result;
        }
        break;

      case Eval::Func::min_reach:
        if (eval_args.size() == 1) {
          arity_ok = true;

          SBGBaseType g = Apply(EvalGraph{}, eval_args[0]);
          ExprBaseType result = std::visit(min_reach_visitor_, g);
          return result;
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

ExprBaseType EvalExpression::operator()(AST::MultiDimInter v) const { return Apply(EvalMDI(env_), AST::Expr(v)); }

ExprBaseType EvalExpression::operator()(AST::MDInterUnaryOp v) const
{
  AST::Expr exp = v.e();
  EvalMDI visit_mdi(env_);
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(Apply(visit_mdi, exp));

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
  AST::Expr l = v.left(), r = v.right();
  EvalMDI visit_mdi(env_);
  switch (v.op()) {
    case AST::ContainerOp::cap: {
      return intersection(Apply(visit_mdi, l), Apply(visit_mdi, r));
    }

    case AST::ContainerOp::less:
      return Apply(visit_mdi, l) < Apply(visit_mdi, r);
      break;

    case AST::ContainerOp::eq:
      return Apply(visit_mdi, l) == Apply(visit_mdi, r);
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

ExprBaseType EvalExpression::operator()(AST::Set v) const
{
  if (nmbr_dims_ == 1) return Apply(EvalOrdSet(env_), AST::Expr(v));

  return Apply(EvalUnordSet(env_), AST::Expr(v));
}

// ----- //

ExprBaseType VisitSetUnOp(AST::SetUnaryOp v, LIB::OrdSet s)
{
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(s);

    case AST::ContainerUOp::comp:
      return complement(s);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetUnaryOp %s not supported.", ss.str().c_str());
      return 0;
  }

  return 0;
}

ExprBaseType VisitSetUnOp(AST::SetUnaryOp v, LIB::UnordSet s)
{
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return cardinal(s);

    case AST::ContainerUOp::comp:
      return complement(s);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetUnaryOp %s not supported.", ss.str().c_str());
      return 0;
  }

  return 0;
}

ExprBaseType EvalExpression::operator()(AST::SetUnaryOp v) const
{
  AST::Expr exp = v.e();
  switch (nmbr_dims_) {
    case 1: {
      EvalOrdSet visit_ord_set(env_);
      LIB::OrdSet s = Apply(visit_ord_set, exp);
      return VisitSetUnOp(v, s);
    }
 
    default: {
      EvalUnordSet visit_unord_set(env_);
      LIB::UnordSet s = Apply(visit_unord_set, exp);
      return VisitSetUnOp(v, s);
    }
  }

  return 0;
}

// ----- //

ExprBaseType VisitSetBinOp(AST::SetBinOp v, LIB::OrdSet sl, LIB::OrdSet sr)
{
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(sl, sr);

    case AST::ContainerOp::diff:
      return difference(sl, sr);

    case AST::ContainerOp::eq:
      return sl == sr;

    case AST::ContainerOp::cup:
      return cup(sl, sr);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetBinOp %s not supported.", ss.str().c_str());
      return 0;
  }

  return 0;
}

ExprBaseType VisitSetBinOp(AST::SetBinOp v, LIB::UnordSet sl, LIB::UnordSet sr)
{
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return intersection(sl, sr);

    case AST::ContainerOp::diff:
      return difference(sl, sr);

    case AST::ContainerOp::eq:
      return sl == sr;

    case AST::ContainerOp::cup:
      return cup(sl, sr);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetBinOp %s not supported.", ss.str().c_str());
      return 0;
  }

  return 0;
}

ExprBaseType EvalExpression::operator()(AST::SetBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  switch (nmbr_dims_) {
    case 1: {
      EvalOrdSet visit_ord_set(env_);
      LIB::OrdSet sl = Apply(visit_ord_set, l), sr = Apply(visit_ord_set, r);
      return VisitSetBinOp(v, sl, sr);
    }

    default: {
      EvalUnordSet visit_unord_set(env_);
      LIB::UnordSet sl = Apply(visit_unord_set, l), sr = Apply(visit_unord_set, r);
      return VisitSetBinOp(v, sl, sr);
    }
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
  if (nmbr_dims_ == 1) return Apply(EvalCanonMap(env_), AST::Expr(v));

  return Apply(EvalBaseMap(env_), AST::Expr(v));
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::PWLMap v) const
{
  if (nmbr_dims_ == 1) return Apply(EvalCanonPWMap(env_), AST::Expr(v));

  return Apply(EvalBasePWMap(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::SBG v) const
{
  if (nmbr_dims_ == 1) return Apply(EvalCanonSBG(env_), AST::Expr(v));

  return Apply(EvalBaseSBG(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::DSBG v) const
{
  if (nmbr_dims_ == 1) return Apply(EvalCanonDSBG(env_), AST::Expr(v));

  return Apply(EvalBaseDSBG(env_), AST::Expr(v));
}

} // namespace Eval

} // namespace SBG
