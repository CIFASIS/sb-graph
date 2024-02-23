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

auto member_visitor_ = Util::Overload {
  [](Util::NAT a, LIB::Interval b) { return b.isMember(a); },
  [](Util::NAT a, LIB::MultiDimInter b) { return b.isMember(Util::MD_NAT(a)); },
  [](Util::MD_NAT a, LIB::MultiDimInter b) { return b.isMember(a); },
  [](auto a, auto b) {
    Util::ERROR("Wrong arguments for isMember"); 
    return false;
  } 
};

auto min_visitor_ = Util::Overload {
  [](LIB::Interval a) { return Util::MD_NAT(a.minElem()); },
  [](LIB::MultiDimInter a) { return a.minElem(); },
  [](LIB::UnordSet a) { return a.minElem(); },
  [](LIB::OrdSet a) { return a.minElem(); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for minElem"); 
    return Util::MD_NAT();
  }
};

auto max_visitor_ = Util::Overload {
  [](LIB::Interval a) { return Util::MD_NAT(a.maxElem()); },
  [](LIB::MultiDimInter a) { return a.maxElem(); },
  [](LIB::UnordSet a) { return a.maxElem(); },
  [](LIB::OrdSet a) { return a.maxElem(); },
  [](auto a) { 
    Util::ERROR("Wrong arguments for maxElem"); 
    return Util::MD_NAT(); 
  }
};

auto compose_linear_visitor_ = Util::Overload {
  [](LIB::LExp a, LIB::LExp b) { return LinearBaseType(a.composition(b)); },
  [](LIB::Exp a, LIB::Exp b) { return LinearBaseType(a.composition(b)); },
  [](auto a, auto b) {
    Util::ERROR("Wrong arguments for composition"); 
    return LinearBaseType(); 
   }
};

auto compose_map_visitor_ = Util::Overload {
  [](LIB::BaseMap a, LIB::BaseMap b) { return MapBaseType(a.composition(b)); },
  [](LIB::CanonMap a, LIB::CanonMap b) {
    return MapBaseType(a.composition(b));
  },
  [](LIB::BasePWMap a, LIB::BasePWMap b) {
    return MapBaseType(a.composition(b));
  },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) {
    return MapBaseType(a.composition(b));
  },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for composition"); 
    return MapBaseType(); 
  }
};

auto image_visitor2_ = Util::Overload {
  [](LIB::UnordSet a, LIB::BaseMap b) { return ContainerBaseType(b.image(a)); },
  [](LIB::OrdSet a, LIB::CanonMap b) { return ContainerBaseType(b.image(a)); },
  [](LIB::UnordSet a, LIB::BasePWMap b) { 
    return ContainerBaseType(b.image(a)); 
  },
  [](LIB::OrdSet a, LIB::CanonPWMap b) {
    return ContainerBaseType(b.image(a));
  },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for image 2"); 
    return ContainerBaseType();
  }
};

auto pre_image_visitor2_ = Util::Overload {
  [](LIB::UnordSet a, LIB::BaseMap b) {
    return ContainerBaseType(b.preImage(a));
  },
  [](LIB::OrdSet a, LIB::CanonMap b) {
    return ContainerBaseType(b.preImage(a));
  },
  [](LIB::UnordSet a, LIB::BasePWMap b) {
    return ContainerBaseType(b.preImage(a));
  },
  [](LIB::OrdSet a, LIB::CanonPWMap b) {
    return ContainerBaseType(b.preImage(a));
  },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for pre image 2"); 
    return ContainerBaseType(); 
  }
};

auto combine_visitor_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return MapBaseType(a.combine(b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) {
    return MapBaseType(a.combine(b));
  },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for combine"); 
    return MapBaseType(); 
  }
};

auto min_map_visitor2_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return MapBaseType(a.minMap(b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) { return MapBaseType(a.minMap(b)); },
  [](auto a, auto b) {
    Util::ERROR("Wrong arguments for minMap2");
    return MapBaseType();
  }
};

auto min_map_unord_visitor3_ = Util::Overload {
  [](LIB::Interval a, LIB::LExp b, LIB::LExp c) {
    return MapBaseType(LIB::BasePWMap().minMap(a, b, c));
  },
  [](LIB::SetPiece a, LIB::Exp b, LIB::Exp c) {
    return MapBaseType(LIB::BasePWMap().minMap(a, b, c));
  },
  [](LIB::UnordSet a, LIB::Exp b, LIB::Exp c) {
    return MapBaseType(LIB::BasePWMap().minMap(a, b, c));
  },
  [](auto a, auto b, auto c) {
    Util::ERROR("Wrong arguments for minMap3");
    return MapBaseType();
  }
};

auto min_map_ord_visitor3_ = Util::Overload {
  [](LIB::Interval a, LIB::LExp b, LIB::LExp c) {
    return MapBaseType(LIB::CanonPWMap().minMap(a, b, c));
  },
  [](LIB::SetPiece a, LIB::Exp b, LIB::Exp c) {
    return MapBaseType(LIB::CanonPWMap().minMap(a, b, c));
  },
  [](LIB::OrdSet a, LIB::Exp b, LIB::Exp c) {
    return MapBaseType(LIB::CanonPWMap().minMap(a, b, c));
  },
  [](auto a, auto b, auto c) {
    Util::ERROR("Wrong arguments for minMap3");
    return MapBaseType();
  }
};

auto min_map_visitor4_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b, LIB::BasePWMap c, LIB::BasePWMap d) {
    return MapBaseType(a.minMap(b, c, d));
  },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b, LIB::CanonPWMap c, LIB::CanonPWMap d) {
    return MapBaseType(a.minMap(b, c, d));
  },
  [](auto a, auto b, auto c, auto d) {
    Util::ERROR("Wrong arguments for minMap4");
    return MapBaseType();
  }
};

auto min_map_unord_visitor5_ = Util::Overload {
  [](LIB::Interval a, LIB::LExp b, LIB::LExp c, LIB::LExp d, LIB::LExp e) {
    return MapBaseType(LIB::BasePWMap().minMap(a, b, c, d, e));
  },
  [](LIB::SetPiece a, LIB::Exp b, LIB::Exp c, LIB::Exp d, LIB::Exp e) {
    return MapBaseType(LIB::BasePWMap().minMap(a, b, c, d, e));
  },
  [](LIB::UnordSet a, LIB::Exp b, LIB::Exp c, LIB::Exp d, LIB::Exp e) {
    return MapBaseType(LIB::BasePWMap().minMap(a, b, c, d, e));
  },
  [](auto a, auto b, auto c, auto d, auto e) {
    Util::ERROR("Wrong arguments for minMap5");
    return MapBaseType();
  }
};

auto min_map_ord_visitor5_ = Util::Overload {
  [](LIB::Interval a, LIB::LExp b, LIB::LExp c, LIB::LExp d, LIB::LExp e) {
    return MapBaseType(LIB::CanonPWMap().minMap(a, b, c, d, e));
  },
  [](LIB::SetPiece a, LIB::Exp b, LIB::Exp c, LIB::Exp d, LIB::Exp e) {
    return MapBaseType(LIB::CanonPWMap().minMap(a, b, c, d, e));
  },
  [](LIB::OrdSet a, LIB::Exp b, LIB::Exp c, LIB::Exp d, LIB::Exp e) {
    return MapBaseType(LIB::CanonPWMap().minMap(a, b, c, d, e));
  },
  [](auto a, auto b, auto c, auto d, auto e) {
    Util::ERROR("Wrong arguments for minMap5");
    return MapBaseType();
  }
};

auto reduce_visitor_ = Util::Overload {
  [](LIB::BasePWMap a) { return MapBaseType(a.reduce()); },
  [](LIB::CanonPWMap a) { return MapBaseType(a.reduce()); },
  [](auto a) {
    Util::ERROR("Wrong arguments for reduce");
    return MapBaseType();
  }
};

auto min_adj_visitor2_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return MapBaseType(a.minAdjMap(b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) {
    return MapBaseType(a.minAdjMap(b));
  },
  [](auto a, auto b) { 
    Util::ERROR("Wrong arguments for minAdjMap"); 
    return MapBaseType(); 
  }
};

auto connected_visitor_ = Util::Overload {
  [](LIB::BaseSBG a) { return MapBaseType(connectedComponents(a)); },
  [](LIB::CanonSBG a) { return MapBaseType(connectedComponents(a)); },
  [](auto a) {
    Util::ERROR("Wrong arguments for connectedComponents");
    return MapBaseType();
  }
};

auto matching_visitor_ = Util::Overload {
  [](LIB::BaseSBG a, Util::MD_NAT b, Util::MD_NAT c, bool d) { 
    LIB::BaseMatch match(a.copy(b[0]), d);
    return InfoBaseType(match.calculate(c[0]));
  },
  [](LIB::CanonSBG a, Util::MD_NAT b, Util::MD_NAT c, bool d) {
    LIB::CanonMatch match(a.copy(b[0]), d);
    return InfoBaseType(match.calculate(c[0]));
  },
  [](auto a, auto b, auto c, auto d) {
    Util::ERROR("Wrong arguments for matching");
    return InfoBaseType();
  }
};

auto scc_visitor_ = Util::Overload {
  [](LIB::BaseDSBG a, bool b) { 
    LIB::BaseSCC scc(a, b);
    return MapBaseType(scc.calculate());
  },
  [](LIB::CanonDSBG a, bool b) {
    LIB::CanonSCC scc(a, b);
    return MapBaseType(scc.calculate());
  },
  [](auto a, auto b) {
    Util::ERROR("Wrong arguments for scc");
    return MapBaseType();
  }
};

auto ts_visitor_ = Util::Overload {
  [](LIB::BaseDSBG a, bool b) { 
    LIB::BaseTopSort ts(a, b);
    return InfoBaseType(ts.calculate()); 
  },
  [](LIB::CanonDSBG a, bool b) {
    LIB::CanonTopSort ts(a, b);
    return InfoBaseType(ts.calculate());
  },
  [](auto a, auto b) {
    Util::ERROR("Wrong arguments for ts");
    return InfoBaseType();
  }
};

auto first_inv_visitor_ = Util::Overload {
  [](LIB::BasePWMap a) { 
    return MapBaseType(a.firstInv()); 
  },
  [](LIB::CanonPWMap a) {
    return MapBaseType(a.firstInv());
  },
  [](auto a) {
    Util::ERROR("Wrong arguments for firstInv");
    return MapBaseType();
  }
};

auto match_scc_visitor_ = Util::Overload {
  [](LIB::BaseSBG a, Util::MD_NAT b, Util::MD_NAT c, bool d) { 
    LIB::BaseMatch match(a.copy(b[0]), d);
    match.calculate(c[0]);
    LIB::BaseSCC scc(buildSCCFromMatching(match), d);
    return MapBaseType(scc.calculate());
  },
  [](LIB::CanonSBG a, Util::MD_NAT b, Util::MD_NAT c, bool d) {
    LIB::CanonMatch match(a.copy(b[0]), d);
    match.calculate(c[0]);
    LIB::CanonSCC scc(buildSCCFromMatching(match), d);
    return MapBaseType(scc.calculate());
  },
  [](auto a, auto b, auto c, auto d) {
    Util::ERROR("Wrong arguments for scc");
    return MapBaseType();
  }
};

// -----------------------------------------------------------------------------
// Expression evaluator --------------------------------------------------------
// -----------------------------------------------------------------------------

EvalExpression::EvalExpression() : nmbr_dims_(1), env_(), debug_(false) {}
EvalExpression::EvalExpression(VarEnv env)
  : nmbr_dims_(1), env_(env), debug_(false) {}
EvalExpression::EvalExpression(unsigned int nmbr_dims, VarEnv env, bool debug)
  : nmbr_dims_(nmbr_dims), env_(env), debug_(debug) {}

ExprBaseType EvalExpression::operator()(AST::Natural v) const
{
  return Util::MD_NAT(v);
}

ExprBaseType EvalExpression::operator()(AST::MDNatural v) const { return v; }

ExprBaseType EvalExpression::operator()(AST::Rational v) const
{
  return boost::apply_visitor(EvalRat(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::Boolean v) const
{
  return Util::MD_NAT(v);
}

ExprBaseType EvalExpression::operator()(Util::VariableName v) const 
{
  MaybeEBT v_opt = env_[v];
  if (v_opt)
    return *v_opt;

  Util::ERROR("EvalExpression: variable %s not defined", v.c_str());
  return Util::MD_NAT(0); 
}

ExprBaseType EvalExpression::operator()(AST::UnaryOp v) const
{
  ExprBaseType x = boost::apply_visitor(*this, v.expr());

  if (std::holds_alternative<Util::MD_NAT>(x)) {
    auto x_value = std::get<Util::MD_NAT>(x); 
    if (x_value.size() == 1)
      return Util::MD_NAT(boost::apply_visitor(EvalNat(env_), AST::Expr(v)));
  }

  return boost::apply_visitor(EvalRat(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::BinOp v) const
{ 
  ExprBaseType xl = boost::apply_visitor(*this, v.left());
  ExprBaseType xr = boost::apply_visitor(*this, v.right());
  bool r1 = std::holds_alternative<Util::RATIONAL>(xl);
  bool r2 = std::holds_alternative<Util::RATIONAL>(xr);

  if (!r1 && !r2)
    return Util::MD_NAT(boost::apply_visitor(EvalNat(env_), AST::Expr(v)));

  return boost::apply_visitor(EvalRat(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::Call v) const
{
  std::string vname = v.name();

  auto venv = fenv_[vname];
  if (venv) { 
    std::vector<ExprBaseType> eval_args;
    for (AST::Expr a : v.args()) 
      eval_args.push_back(boost::apply_visitor(*this, a));

    bool arity_ok = false;
    switch (*venv) {
      case Eval::Func::empty: 
        if (eval_args.size() == 1) {
          arity_ok = true;

          ContainerBaseType container 
            = std::visit(EvalContainer{}, eval_args[0]);
          auto f = [](auto const &c) {
            return c.isEmpty();
          };
          bool result = std::visit(f, container);
          return Util::MD_NAT(result);
        }
        break;

      case Eval::Func::member:  
        if (eval_args.size() == 2) {
          arity_ok = true;

          ContainerBaseType container
            = std::visit(EvalContainer{}, eval_args[1]);
          NatBaseType x = std::visit(EvalNatBT{}, eval_args[0]);
          bool result = std::visit(member_visitor_, x, container);
          return Util::MD_NAT(result);
        }
        break;

      case Eval::Func::min:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ContainerBaseType container
            = std::visit(EvalContainer{}, eval_args[0]);
          Util::MD_NAT result = std::visit(min_visitor_, container);
          return result;
        }
        break;

      case Eval::Func::max:
        if (eval_args.size() == 1) {
          arity_ok = true;

          ContainerBaseType container
            = std::visit(EvalContainer{}, eval_args[0]);
          Util::MD_NAT result = std::visit(max_visitor_, container);
          return result;
        }
        break;

      case Eval::Func::comp:
        if (eval_args.size() == 2) {
          arity_ok = true;

          ExprBaseType result;
          if (std::holds_alternative<LinearBaseType>(eval_args[0])) { 
            EvalLinear visit_linear;
            LinearBaseType e1 = std::visit(visit_linear, eval_args[0]);
            LinearBaseType e2  = std::visit(visit_linear, eval_args[1]);
            result = std::visit(compose_linear_visitor_, e1, e2);
          }

          else {
            EvalMap visit_map;
            MapBaseType m1 = std::visit(visit_map, eval_args[0]);
            MapBaseType m2 = std::visit(visit_map, eval_args[1]);
            result = std::visit(compose_map_visitor_, m1, m2);
          }

          return result;
        }
        break;

      case Eval::Func::inv:
        if (eval_args.size() == 1) {
          arity_ok = true;

          LinearBaseType lexp 
            = std::visit(EvalLinear(), eval_args[0]);
          auto f = [](auto const &l) {
            return LinearBaseType(l.inverse());
          };
          LinearBaseType result = std::visit(f, lexp);
          return result;
        }
        break;

      case Eval::Func::im:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType sbgmap = std::visit(EvalMap(), eval_args[0]);
          auto f = [](auto const &m) {
            return ContainerBaseType(m.image());
          };
          ContainerBaseType result = std::visit(f, sbgmap);
          return result;
        }

        else if (eval_args.size() == 2) {
          arity_ok = true;

          ContainerBaseType subdom
            = std::visit(EvalContainer{}, eval_args[0]);
          MapBaseType sbgmap = std::visit(EvalMap(), eval_args[1]);
          ExprBaseType result = std::visit(image_visitor2_, subdom, sbgmap);
          return result;
        }

        break;

      case Eval::Func::preim:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType sbgmap = std::visit(EvalMap(), eval_args[0]);
          auto f = [](auto const &m) {
            return ContainerBaseType(m.preImage());
          };
          ContainerBaseType result = std::visit(f, sbgmap);
          return result;
        }

        else if (eval_args.size() == 2) {
          arity_ok = true;

          ContainerBaseType subdom = std::visit(EvalContainer{}, eval_args[0]);
          MapBaseType map = std::visit(EvalMap(), eval_args[1]);
          ExprBaseType result = std::visit(pre_image_visitor2_, subdom, map);
          return result;
        }
        
        break;

      case Eval::Func::dom:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType pw = std::visit(EvalMap(), eval_args[0]); 
          auto f = [](auto const &c) {
            return ContainerBaseType(c.dom());
          };
          ContainerBaseType result = std::visit(f, pw);
          return result;
        }
        break;

      case Eval::Func::comb:
        if (eval_args.size() == 2) {
          arity_ok = true;
 
          MapBaseType pw1 = std::visit(EvalMap(), eval_args[0]);
          MapBaseType pw2 = std::visit(EvalMap(), eval_args[1]);
          MapBaseType result = std::visit(combine_visitor_, pw1, pw2);
          return result;
        }
        break;

      case Eval::Func::min_map:
        if (eval_args.size() == 2) {
          arity_ok = true;

          EvalMap visit_map;
          MapBaseType pw1 = std::visit(visit_map, eval_args[0]);
          MapBaseType pw2 = std::visit(visit_map, eval_args[1]);
          MapBaseType result = std::visit(min_map_visitor2_, pw1, pw2);
          return result;
        }

        if (eval_args.size() == 3) {
          arity_ok = true;

          ContainerBaseType container
            = std::visit(EvalContainer{}, eval_args[0]);
          EvalLinear visit_linear;
          LinearBaseType e1 = std::visit(visit_linear, eval_args[1]);
          LinearBaseType e2 = std::visit(visit_linear, eval_args[2]);

          MapBaseType result;
          if (nmbr_dims_ == 1)
            result = std::visit(min_map_ord_visitor3_, container, e1, e2);
          else
            result = std::visit(min_map_unord_visitor3_, container, e1, e2);

          return result;
        }

        if (eval_args.size() == 4) {
          arity_ok = true;

          EvalMap visit_map;
          MapBaseType pw1 = std::visit(visit_map, eval_args[0]);
          MapBaseType pw2 = std::visit(visit_map, eval_args[1]);
          MapBaseType pw3 = std::visit(visit_map, eval_args[2]);
          MapBaseType pw4 = std::visit(visit_map, eval_args[3]);

          MapBaseType result = std::visit(min_map_visitor4_, pw1, pw2, pw3, pw4);
          return result;
        }

        if (eval_args.size() == 5) {
          arity_ok = true;

          ContainerBaseType container
            = std::visit(EvalContainer{}, eval_args[0]);
          EvalLinear visit_linear;
          LinearBaseType e1 = std::visit(visit_linear, eval_args[1]);
          LinearBaseType e2 = std::visit(visit_linear, eval_args[2]);
          LinearBaseType e3 = std::visit(visit_linear, eval_args[3]);
          LinearBaseType e4 = std::visit(visit_linear, eval_args[4]);

          MapBaseType result;
          if (nmbr_dims_ == 1)
            result = std::visit(min_map_ord_visitor5_, container, e1, e2, e3, e4);
          else
            result = std::visit(min_map_unord_visitor5_, container, e1, e2, e3, e4);

          return result;
        }
        break;

      case Eval::Func::red:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType map = std::visit(EvalMap{}, eval_args[0]);
          MapBaseType result = std::visit(reduce_visitor_, map);
          return result;
        }
        break;

      case Eval::Func::min_adj:
        if (eval_args.size() == 2) {
          arity_ok = true;

          MapBaseType map1 = std::visit(EvalMap{}, eval_args[0]);
          MapBaseType map2 = std::visit(EvalMap{}, eval_args[1]);
          MapBaseType result = std::visit(min_adj_visitor2_, map1, map2);
          return result;
        }
        break;

      case Eval::Func::connected:
        if (eval_args.size() == 1) {
          arity_ok = true;

          SBGBaseType sbg = std::visit(EvalGraph{}, eval_args[0]);
          MapBaseType result = std::visit(connected_visitor_, sbg);
          return result;
        }
        break;

      case Eval::Func::matching:
        if (eval_args.size() == 3) {
          arity_ok = true;

          SBGBaseType g = std::visit(EvalGraph{}, eval_args[0]);
          NatBaseType copies = std::visit(EvalNatBT{}, eval_args[1]);
          NatBaseType k = std::visit(EvalNatBT{}, eval_args[2]);
          InfoBaseType result = std::visit(
            matching_visitor_, g, copies, k, std::variant<bool>(debug_)
          );
          return result;
        }
        break;

      case Eval::Func::scc:
        if (eval_args.size() == 1) {
          arity_ok = true;

          SBGBaseType g = std::visit(EvalGraph{}, eval_args[0]);
          MapBaseType result = std::visit(
            scc_visitor_, g, std::variant<bool>(debug_)
          );
          return result;
        }
        break;


      case Eval::Func::ts:
        if (eval_args.size() == 1) {
          arity_ok = true;

          SBGBaseType g = std::visit(EvalGraph{}, eval_args[0]);
          InfoBaseType result = std::visit(
            ts_visitor_, g, std::variant<bool>(debug_)
          );
          return result;
        }
        break;

      case Eval::Func::first_inv:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType pw = std::visit(EvalMap(), eval_args[0]); 
          MapBaseType result = std::visit(first_inv_visitor_, pw);
          return result;
        }

      case Eval::Func::match_scc:
        if (eval_args.size() == 3) {
          arity_ok = true;

          SBGBaseType g = std::visit(EvalGraph{}, eval_args[0]);
          NatBaseType copies = std::visit(EvalNatBT{}, eval_args[1]);
          NatBaseType k = std::visit(EvalNatBT{}, eval_args[2]);
          MapBaseType result = std::visit(
            match_scc_visitor_, g, copies, k, std::variant<bool>(debug_)
          );
          return result;
        }
        break;

      default:
        Util::ERROR("EvalExpression: function %s not implemented", vname.c_str());
        return Util::MD_NAT(0);
    }

    if (!arity_ok) {
      Util::ERROR("EvalExpression: wrong number of arguments for Call");
      return Util::MD_NAT(0);
    }
  }

  Util::ERROR("EvalExpression: function %s does not exist", vname.c_str());
  return Util::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::Interval v) const
{
  return boost::apply_visitor(EvalInterval(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::InterUnaryOp v) const
{
  EvalInterval eval_interval(env_);
  LIB::Interval i = boost::apply_visitor(eval_interval, v.e());
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return Util::MD_NAT(i.cardinal());

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: InterUnaryOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }
}

ExprBaseType EvalExpression::operator()(AST::InterBinOp v) const
{
  EvalInterval eval_interval(env_);
  LIB::Interval l = boost::apply_visitor(eval_interval, v.left());
  LIB::Interval r = boost::apply_visitor(eval_interval, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l.intersection(r);

    case AST::ContainerOp::less:
      return Util::MD_NAT(l < r);

    case AST::ContainerOp::eq:
      return Util::MD_NAT(l == r);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: InterBinOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }
}

ExprBaseType EvalExpression::operator()(AST::MultiDimInter v) const
{
  return boost::apply_visitor(EvalMDI(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::MDInterUnaryOp v) const
{
  EvalMDI visit_mdi(env_);
  LIB::MultiDimInter mdi = boost::apply_visitor(visit_mdi, v.e());
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return Util::MD_NAT(mdi.cardinal());

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: MDInterUnaryOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::MDInterBinOp v) const
{
  EvalMDI visit_mdi(env_);
  LIB::MultiDimInter l = boost::apply_visitor(visit_mdi, v.left());
  LIB::MultiDimInter r = boost::apply_visitor(visit_mdi, v.right());
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l.intersection(r);

    case AST::ContainerOp::less:
      return Util::MD_NAT(l < r);

    case AST::ContainerOp::eq:
      return Util::MD_NAT(l == r);

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: MDInterBinOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::Set v) const
{
  ContainerBaseType c;
  if (nmbr_dims_ == 1)
    c = boost::apply_visitor(EvalOrdSet(env_), AST::Expr(v));
  else
    c = boost::apply_visitor(EvalUnordSet(env_), AST::Expr(v));

  return c;
}

// ----- //

ExprBaseType VisitSetUnOp(AST::SetUnaryOp v, LIB::OrdSet s)
{
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return Util::MD_NAT(s.cardinal());

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetUnaryOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType VisitSetUnOp(AST::SetUnaryOp v, LIB::UnordSet s)
{
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return Util::MD_NAT(s.cardinal());

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetUnaryOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::SetUnaryOp v) const
{
  AST::Expr exp = v.e();
  switch (nmbr_dims_) {
    case 1: {
      EvalOrdSet visit_ord_set(env_);
      LIB::OrdSet s = boost::apply_visitor(visit_ord_set, exp);
      return VisitSetUnOp(v, s);
    }
 
    default: {
      EvalUnordSet visit_unord_set(env_);
      LIB::UnordSet s = boost::apply_visitor(visit_unord_set, exp);
      return VisitSetUnOp(v, s);
    }
  }

  return Util::MD_NAT(0);
}

// ----- //

ExprBaseType VisitSetBinOp(AST::SetBinOp v, LIB::OrdSet sl, LIB::OrdSet sr)
{
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return ContainerBaseType(sl.intersection(sr));

    case AST::ContainerOp::diff:
      return ContainerBaseType(sl.difference(sr));

    case AST::ContainerOp::eq:
      return Util::MD_NAT(sl == sr);

    case AST::ContainerOp::cup:
      return ContainerBaseType(sl.cup(sr));

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetBinOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType VisitSetBinOp(AST::SetBinOp v, LIB::UnordSet sl, LIB::UnordSet sr)
{
  switch (v.op()) {
    case AST::ContainerOp::cap:
      return ContainerBaseType(sl.intersection(sr));

    case AST::ContainerOp::diff:
      return ContainerBaseType(sl.difference(sr));

    case AST::ContainerOp::eq:
      return Util::MD_NAT(sl == sr);

    case AST::ContainerOp::cup:
      return ContainerBaseType(sl.cup(sr));

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: SetBinOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::SetBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  switch (nmbr_dims_) {
    case 1: {
      EvalOrdSet visit_ord_set(env_);
      LIB::OrdSet sl = boost::apply_visitor(visit_ord_set, l);
      LIB::OrdSet sr = boost::apply_visitor(visit_ord_set, r);
      return VisitSetBinOp(v, sl, sr);
    }

    default: {
      EvalUnordSet visit_unord_set(env_);
      LIB::UnordSet sl = boost::apply_visitor(visit_unord_set, l);
      LIB::UnordSet sr = boost::apply_visitor(visit_unord_set, r);
      return VisitSetBinOp(v, sl, sr);
    }
  }

  return Util::MD_NAT(0);
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::LinearExp v) const
{
  return boost::apply_visitor(EvalLE(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::LExpBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalLE visit_le(env_);
  const LIB::LExp lexp = boost::apply_visitor(visit_le, l);
  const LIB::LExp rexp = boost::apply_visitor(visit_le, r);
  switch (v.op()) {
    case AST::ExpOp::eq:
      return Util::MD_NAT(lexp == rexp);

    case AST::ExpOp::add:
      return lexp + rexp;

    case AST::ExpOp::sub:
      return lexp - rexp;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: LExpBinOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::MDLExp v) const
{
  return boost::apply_visitor(EvalMDLE(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::MDLExpBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalMDLE visit_le(env_);
  LIB::Exp lexp = boost::apply_visitor(visit_le, l), rexp = boost::apply_visitor(visit_le, r);
  switch (v.op()) {
    case AST::ExpOp::eq:
      return Util::MD_NAT(lexp == rexp);

    case AST::ExpOp::add:
      return lexp + rexp;

    case AST::ExpOp::sub:
      return lexp - rexp;

    default:
      std::stringstream ss;
      ss << v.op();
      Util::ERROR("EvalExpression: MDLExpBinOp %s not supported.", ss.str().c_str());
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::LinearMap v) const
{
  if (nmbr_dims_ == 1) return boost::apply_visitor(EvalCanonMap(env_), AST::Expr(v));

  return boost::apply_visitor(EvalBaseMap(env_), AST::Expr(v));
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::PWLMap v) const
{
  if (nmbr_dims_ == 1) return boost::apply_visitor(EvalCanonPWMap(env_), AST::Expr(v));

  return boost::apply_visitor(EvalBasePWMap(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::SBG v) const
{
  if (nmbr_dims_ == 1) return boost::apply_visitor(EvalCanonSBG(env_), AST::Expr(v));

  return boost::apply_visitor(EvalBaseSBG(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::DSBG v) const
{
  if (nmbr_dims_ == 1) return boost::apply_visitor(EvalCanonDSBG(env_), AST::Expr(v));

  return boost::apply_visitor(EvalBaseDSBG(env_), AST::Expr(v));
}

} // namespace Eval

} // namespace SBG
