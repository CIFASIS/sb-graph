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

auto min_visitor_ = Util::Overload {
  [](LIB::Interval a) { return Util::MD_NAT(a.begin()); },
  [](LIB::MultiDimInter a) { return a.minElem(); },
  [](LIB::UnordSet a) { return a.minElem(); },
  [](LIB::OrdSet a) { return a.minElem(); },
  [](auto a) { 
    Util::ERROR("min_visitor_: wrong argument ", a, " for minElem\n"); 
    return Util::MD_NAT();
  }
};

auto max_visitor_ = Util::Overload {
  [](LIB::Interval a) { return Util::MD_NAT(a.end()); },
  [](LIB::MultiDimInter a) { return a.maxElem(); },
  [](LIB::UnordSet a) { return a.maxElem(); },
  [](LIB::OrdSet a) { return a.maxElem(); },
  [](auto a) { 
    Util::ERROR("max_visitor_: wrong argument ", a, " for maxElem\n"); 
    return Util::MD_NAT(); 
  }
};

auto compose_linear_visitor_ = Util::Overload {
  [](LIB::LExp a, LIB::LExp b) { return LinearBaseType(a.composition(b)); },
  [](LIB::Exp a, LIB::Exp b) { return LinearBaseType(a.composition(b)); },
  [](auto a, auto b) {
    Util::ERROR("compose_linear_visitor_: wrong arguments ", a, ", ", b
      , " for compose\n"); 
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
    Util::ERROR("compose_map_visitor_: wrong arguments ", a, ", ", b
      , " for compose\n"); 
    return MapBaseType(); 
  }
};

auto image_visitor_ = Util::Overload {
  [](LIB::UnordSet a, LIB::BaseMap b) { return ContainerBaseType(b.image(a)); },
  [](LIB::OrdSet a, LIB::CanonMap b) { return ContainerBaseType(b.image(a)); },
  [](LIB::UnordSet a, LIB::BasePWMap b) { 
    return ContainerBaseType(b.image(a)); 
  },
  [](LIB::OrdSet a, LIB::CanonPWMap b) {
    return ContainerBaseType(b.image(a));
  },
  [](auto a, auto b) { 
    Util::ERROR("image_visitor_: wrong arguments ", a, ", ", b, " for image\n"); 
    return ContainerBaseType();
  }
};

auto pre_image_visitor_ = Util::Overload {
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
    Util::ERROR("pre_image_visitor_: wrong arguments ", a, ", ", b
      , " for pre-image\n"); 
    return ContainerBaseType(); 
  }
};

auto combine_visitor_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return MapBaseType(a.combine(b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) {
    return MapBaseType(a.combine(b));
  },
  [](auto a, auto b) { 
    Util::ERROR("combine_visitor_: wrong arguments ", a, ", ", b
      , " for combine\n"); 
    return MapBaseType(); 
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
    Util::ERROR("first_inv_visitor_: wrong argument ", a, " for firstInv\n"); 
    return MapBaseType();
  }
};

auto min_map_visitor_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return MapBaseType(a.minMap(b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) { return MapBaseType(a.minMap(b)); },
  [](auto a, auto b) {
    Util::ERROR("min_map_visitor_: wrong arguments ", a, ", ", b
      , " for minMap\n"); 
    return MapBaseType();
  }
};

auto reduce_visitor_ = Util::Overload {
  [](LIB::BasePWMap a) { return MapBaseType(a.reduce()); },
  [](LIB::CanonPWMap a) { return MapBaseType(a.reduce()); },
  [](auto a) {
    Util::ERROR("reduce_visitor_: wrong argument ", a, " for reduce\n"); 
    return MapBaseType();
  }
};

auto min_adj_visitor_ = Util::Overload {
  [](LIB::BasePWMap a, LIB::BasePWMap b) { return MapBaseType(a.minAdjMap(b)); },
  [](LIB::CanonPWMap a, LIB::CanonPWMap b) {
    return MapBaseType(a.minAdjMap(b));
  },
  [](auto a, auto b) { 
    Util::ERROR("min_adj_visitor_: wrong arguments ", a, ", ", b
      , " for minAdj\n"); 
    return MapBaseType(); 
  }
};

auto inf_visitor_ = Util::Overload {
  [](LIB::BasePWMap a) { return MapBaseType(a.mapInf()); },
  [](LIB::CanonPWMap a) { return MapBaseType(a.mapInf()); },
  [](auto a, auto b) { 
    Util::ERROR("inf_visitor_: wrong argument ", a, " for mapInf\n"); 
    return MapBaseType(); 
  }
};

auto connected_visitor_ = Util::Overload {
  [](LIB::BaseSBG a) { return MapBaseType(connectedComponents(a)); },
  [](LIB::CanonSBG a) { return MapBaseType(connectedComponents(a)); },
  [](auto a) {
    Util::ERROR("connected_visitor_: wrong argument ", a, " for CC\n"); 
    return MapBaseType();
  }
};

auto matching_visitor_ = Util::Overload {
  [](LIB::BaseSBG a, Util::MD_NAT b, bool c) { 
    LIB::BaseMatch match(a.copy(b[0]), c);
    return InfoBaseType(match.calculate());
  },
  [](LIB::CanonSBG a, Util::MD_NAT b, bool c) {
    LIB::CanonMatch match(a.copy(b[0]), c);
    return InfoBaseType(match.calculate());
  },
  [](auto a, auto b, auto c) {
    Util::ERROR("matching_visitor_: wrong arguments ", a, ", ", b
      , " for matching\n"); 
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
    Util::ERROR("scc_visitor_: wrong argument ", a, " for scc\n"); 
    return MapBaseType();
  }
};

auto ts_visitor_ = Util::Overload {
  [](LIB::BaseDSBG a, bool b) { 
    LIB::BaseTopSort ts(a, b);
    return MapBaseType(ts.calculate()); 
  },
  [](LIB::CanonDSBG a, bool b) {
    LIB::CanonTopSort ts(a, b);
    return MapBaseType(ts.calculate());
  },
  [](auto a, auto b) {
    Util::ERROR("ts_visitor_: wrong argument ", a, " for sort\n"); 
    return MapBaseType();
  }
};

auto match_scc_visitor_ = Util::Overload {
  [](LIB::BaseSBG a, Util::MD_NAT b, bool c) { 
    LIB::BaseMatch match(a.copy(b[0]), c);
    match.calculate();
    LIB::BaseSCC scc(buildSCCFromMatching(match), c);
    return MapBaseType(scc.calculate());
  },
  [](LIB::CanonSBG a, Util::MD_NAT b, bool c) {
    LIB::CanonMatch match(a.copy(b[0]), c);
    match.calculate();
    LIB::CanonSCC scc(buildSCCFromMatching(match), c);
    return MapBaseType(scc.calculate());
  },
  [](auto a, auto b, auto c) {
    Util::ERROR("match_scc_visitor_: wrong arguments ", a, ", ", b
      , " for matchSCC\n"); 
    return MapBaseType();
  }
};

auto match_scc_ts_visitor_ = Util::Overload {
  [](LIB::BaseSBG a, Util::MD_NAT b, bool c) { 
    LIB::BaseMatch match(a.copy(b[0]), c);
    LIB::UnordSet match_res = match.calculate().matched_edges();
    LIB::BaseSCC scc(buildSCCFromMatching(match), c);
    LIB::BasePWMap scc_res = scc.calculate();
    LIB::BaseTopSort ts(buildSortFromSCC(scc, scc_res), c);
    LIB::BasePWMap ts_res = ts.calculate(); 
    buildJson(match_res, scc_res, ts_res);
    return MapBaseType(ts_res);
  },
  [](LIB::CanonSBG a, Util::MD_NAT b, bool c) {
    LIB::CanonMatch match(a.copy(b[0]), c);
    LIB::OrdSet match_res = match.calculate().matched_edges();
    LIB::CanonSCC scc(buildSCCFromMatching(match), c);
    LIB::CanonPWMap scc_res = scc.calculate();
    LIB::CanonTopSort ts(buildSortFromSCC(scc, scc_res), c);
    LIB::CanonPWMap ts_res = ts.calculate();
    buildJson(match_res, scc_res, ts_res);
    return MapBaseType(ts_res);
  },
  [](auto a, auto b, auto c) {
    Util::ERROR("match_scc_ts_visitor_: wrong arguments ", a, ", ", b
      , " for matchSCCTS\n"); 
    return MapBaseType();
  }
};

auto cut_visitor_ = Util::Overload {
  [](LIB::BaseDSBG a, bool b) { 
    LIB::BaseCutSet cut_set(a, b);
    return ContainerBaseType(cut_set.calculate());
  },
  [](LIB::CanonDSBG a, bool b) {
    LIB::CanonCutSet cut_set(a, b);
    return ContainerBaseType(cut_set.calculate());
  },
  [](auto a, auto b) {
    Util::ERROR("cut_visitor_: wrong argument ", a, " for cut\n"); 
    return ContainerBaseType();
  }
};

// -----------------------------------------------------------------------------
// Expression evaluator --------------------------------------------------------
// -----------------------------------------------------------------------------

EvalExpression::EvalExpression() : nmbr_dims_(1), opt_conds_(true), env_()
  , debug_(false) {}
EvalExpression::EvalExpression(VarEnv env)
  : nmbr_dims_(1), opt_conds_(true), env_(env), debug_(false) {}
EvalExpression::EvalExpression(unsigned int nmbr_dims, bool opt_conds
  , VarEnv env, bool debug)
  : nmbr_dims_(nmbr_dims), opt_conds_(opt_conds), env_(env), debug_(debug) {}

ExprBaseType EvalExpression::operator()(AST::Natural v) const
{
  Util::ERROR_UNLESS(nmbr_dims_ == 1
    ,"EvalExpr: nmbr_dims_: ", nmbr_dims_, " not 1\n");

  return Util::MD_NAT(v);
}

ExprBaseType EvalExpression::operator()(AST::MDNatural v) const
{
  Util::ERROR_UNLESS(nmbr_dims_ == v.arity() || 0 == v.arity()
    ,"EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", v, ")\n");

  return v;
}

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

  Util::ERROR("EvalExpression: variable ", v, " undefined\n");
  return Util::MD_NAT(0); 
}

ExprBaseType EvalExpression::operator()(AST::UnaryOp v) const
{
  ExprBaseType x = boost::apply_visitor(*this, v.expr());

  if (std::holds_alternative<Util::MD_NAT>(x)) {
    auto x_value = std::get<Util::MD_NAT>(x); 

    Util::ERROR_UNLESS(nmbr_dims_ == x_value.arity() || 0 == x_value.arity()
      ,"EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", x_value, ")\n");
   
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
          ExprBaseType result = std::visit(image_visitor_, subdom, sbgmap);
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
          ExprBaseType result = std::visit(pre_image_visitor_, subdom, map);
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

      case Eval::Func::first_inv:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType pw = std::visit(EvalMap(), eval_args[0]); 
          MapBaseType result = std::visit(first_inv_visitor_, pw);
          return result;
        }

      case Eval::Func::min_map:
        if (eval_args.size() == 2) {
          arity_ok = true;

          EvalMap visit_map;
          MapBaseType pw1 = std::visit(visit_map, eval_args[0]);
          MapBaseType pw2 = std::visit(visit_map, eval_args[1]);
          MapBaseType result = std::visit(min_map_visitor_, pw1, pw2);
          return result;
        }

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
          MapBaseType result = std::visit(min_adj_visitor_, map1, map2);
          return result;
        }
        break;

      case Eval::Func::inf:
        if (eval_args.size() == 1) {
          arity_ok = true;

          MapBaseType map = std::visit(EvalMap{}, eval_args[0]);
          MapBaseType result = std::visit(inf_visitor_, map);
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
        if (eval_args.size() == 2) {
          arity_ok = true;

          SBGBaseType g = std::visit(EvalGraph{}, eval_args[0]);
          NatBaseType copies = std::visit(EvalNatBT{}, eval_args[1]);
          InfoBaseType result = std::visit(
            matching_visitor_, g, copies, std::variant<bool>(debug_)
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
          MapBaseType result = std::visit(
            ts_visitor_, g, std::variant<bool>(debug_)
          );
          return result;
        }
        break;

      case Eval::Func::match_scc:
        if (eval_args.size() == 2) {
          arity_ok = true;

          SBGBaseType g = std::visit(EvalGraph{}, eval_args[0]);
          NatBaseType copies = std::visit(EvalNatBT{}, eval_args[1]);
          MapBaseType result = std::visit(
            match_scc_visitor_, g, copies, std::variant<bool>(debug_)
          );
          return result;
        }
        break;

      case Eval::Func::match_scc_ts:
        if (eval_args.size() == 2) {
          arity_ok = true;

          SBGBaseType g = std::visit(EvalGraph{}, eval_args[0]);
          NatBaseType copies = std::visit(EvalNatBT{}, eval_args[1]);
          MapBaseType result = std::visit(
            match_scc_ts_visitor_, g, copies, std::variant<bool>(debug_)
          );
          return result;
        }
        break;

      case Eval::Func::cut_set:
        if (eval_args.size() == 1) {
          arity_ok = true;

          SBGBaseType g = std::visit(EvalGraph{}, eval_args[0]);
          ContainerBaseType result = std::visit(
            cut_visitor_, g, std::variant<bool>(debug_)
          );
          return result;
        }
        break;

      default:
        Util::ERROR("EvalExpression: function ", vname, " not implemented\n");
        return Util::MD_NAT(0);
    }

    if (!arity_ok) {
      Util::ERROR("EvalExpression: wrong number of arguments for Call "
        , vname, "\n");
      return Util::MD_NAT(0);
    }
  }

  Util::ERROR("EvalExpression: function ", vname, " doesn't exist\n");
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
      Util::ERROR("EvalExpression: InterUnaryOp ", v.op(), " unsupported\n");
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
      Util::ERROR("EvalExpression: InterBinOp ", v.op(), " unsupported\n");
      return Util::MD_NAT(0);
  }
}

ExprBaseType EvalExpression::operator()(AST::MultiDimInter v) const
{
  SBG::LIB::SetPiece mdi = boost::apply_visitor(EvalMDI(env_), AST::Expr(v));

  Util::ERROR_UNLESS(mdi.arity() == nmbr_dims_ || mdi.arity() == 0
    , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", mdi, ")\n");

  return mdi;
}

ExprBaseType EvalExpression::operator()(AST::MDInterUnaryOp v) const
{
  EvalMDI visit_mdi(env_);
  LIB::MultiDimInter mdi = boost::apply_visitor(visit_mdi, v.e());

  Util::ERROR_UNLESS(mdi.arity() == nmbr_dims_ || mdi.arity() == 0
    , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", mdi, ")\n");

  switch (v.op()) {
    case AST::ContainerUOp::card:
      return Util::MD_NAT(mdi.cardinal());

    default:
      Util::ERROR("EvalExpression: MDInterUnaryOp ", v.op(), " unsupported\n");
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::MDInterBinOp v) const
{
  EvalMDI visit_mdi(env_);
  LIB::MultiDimInter l = boost::apply_visitor(visit_mdi, v.left());
  LIB::MultiDimInter r = boost::apply_visitor(visit_mdi, v.right());

  Util::ERROR_UNLESS((l.arity() == nmbr_dims_ && r.arity() == nmbr_dims_)
    || l.arity() == 0 || r.arity() == 0
    ,"EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", l, ") or arity(", r
    , ")\n");

  switch (v.op()) {
    case AST::ContainerOp::cap:
      return l.intersection(r);

    case AST::ContainerOp::less:
      return Util::MD_NAT(l < r);

    case AST::ContainerOp::eq:
      return Util::MD_NAT(l == r);

    default:
      Util::ERROR("EvalExpression: MDInterBinOp ", v.op(), " unsupported\n");
      return Util::MD_NAT(0);
  }


  return Util::MD_NAT(0);
}


ExprBaseType EvalExpression::operator()(AST::Set v) const
{
  ContainerBaseType c;
  if (nmbr_dims_ == 1 && opt_conds_) {
    LIB::OrdSet s = boost::apply_visitor(EvalOrdSet(env_), AST::Expr(v));
    c = s;

    Util::ERROR_UNLESS(s.arity() == 1 || s.arity() == 0
      , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", s, ")\n");
  }
  else {
    LIB::UnordSet s = boost::apply_visitor(EvalUnordSet(env_), AST::Expr(v));
    c = s;

    Util::ERROR_UNLESS(s.arity() == nmbr_dims_ || s.arity() == 0
      , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", s, ")\n");
  }

  return c;
}

// ----- //

ExprBaseType VisitSetUnOp(AST::SetUnaryOp v, LIB::OrdSet s)
{
  switch (v.op()) {
    case AST::ContainerUOp::card:
      return Util::MD_NAT(s.cardinal());

    default:
      Util::ERROR("EvalExpression: SetUnaryOp ", v.op(), " unsupported\n");
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
      Util::ERROR("EvalExpression: SetUnaryOp ", v.op(), " unsupported\n");
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::SetUnaryOp v) const
{
  AST::Expr exp = v.e();
  if (nmbr_dims_ == 1 && opt_conds_) {
    EvalOrdSet visit_ord_set(env_);
    LIB::OrdSet s = boost::apply_visitor(visit_ord_set, exp);

    Util::ERROR_UNLESS(s.arity() == 1 || s.arity() == 0
      , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", s, ")\n");

    return VisitSetUnOp(v, s);
  }
 
  else {
    EvalUnordSet visit_unord_set(env_);
    LIB::UnordSet s = boost::apply_visitor(visit_unord_set, exp);

    Util::ERROR_UNLESS(s.arity() == nmbr_dims_ || s.arity() == 0
      , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", s, ")\n");

    return VisitSetUnOp(v, s);
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
      Util::ERROR("EvalExpression: SetBinOp ", v.op(), " unsupported\n");
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
      Util::ERROR("EvalExpression: SetBinOp ", v.op(), " unsupported\n");
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::SetBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  if (nmbr_dims_ == 1 && opt_conds_) {
    EvalOrdSet visit_ord_set(env_);
    LIB::OrdSet sl = boost::apply_visitor(visit_ord_set, l);
    LIB::OrdSet sr = boost::apply_visitor(visit_ord_set, r);

    Util::ERROR_UNLESS((sl.arity() == 1 && sr.arity() == 1)
        || sl.arity() == 0 || sr.arity() == 0
      ,"EvalExpr: nmbr_dims_: ", nmbr_dims_
      , " != arity(", sl, ") or arity(", sr, ")\n");

    return VisitSetBinOp(v, sl, sr);
  }

  else {
    EvalUnordSet visit_unord_set(env_);
    LIB::UnordSet sl = boost::apply_visitor(visit_unord_set, l);
    LIB::UnordSet sr = boost::apply_visitor(visit_unord_set, r);

    Util::ERROR_UNLESS((sl.arity() == nmbr_dims_ && sr.arity() == nmbr_dims_)
        || sl.arity() == 0 || sr.arity() == 0
      ,"EvalExpr: nmbr_dims_: ", nmbr_dims_
      , " != arity(", sl, ") or arity(", sr, ")\n");

    return VisitSetBinOp(v, sl, sr);
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
      Util::ERROR("EvalExpression: LExpBinOp ", v.op(), " unsupported\n");
      return Util::MD_NAT(0);
  }
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::MDLExp v) const
{
  LIB::Exp mdle = boost::apply_visitor(EvalMDLE(env_), AST::Expr(v));
  
  Util::ERROR_UNLESS(mdle.arity() == nmbr_dims_
    , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", mdle, ")\n");

  return mdle;
}

ExprBaseType EvalExpression::operator()(AST::MDLExpBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalMDLE visit_le(env_);
  LIB::Exp lexp = boost::apply_visitor(visit_le, l);
  LIB::Exp rexp = boost::apply_visitor(visit_le, r);

  Util::ERROR_UNLESS(lexp.arity() == nmbr_dims_ && rexp.arity() == nmbr_dims_
    ,"EvalExpr: nmbr_dims_: ", nmbr_dims_
    , " != arity(", lexp, ") or arity(", rexp, ")\n");

  switch (v.op()) {
    case AST::ExpOp::eq:
      return Util::MD_NAT(lexp == rexp);

    case AST::ExpOp::add:
      return lexp + rexp;

    case AST::ExpOp::sub:
      return lexp - rexp;

    default:
      Util::ERROR("EvalExpression: MDLExpBinOp ", v.op(), " unsupported\n");
      return Util::MD_NAT(0);
  }

  return Util::MD_NAT(0);
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::LinearMap v) const
{
  if (nmbr_dims_ == 1 && opt_conds_) {
    LIB::CanonMap m = boost::apply_visitor(EvalCanonMap(env_), AST::Expr(v));

    Util::ERROR_UNLESS(m.arity() == 1 || m.arity() == 0
      , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", m, ")\n");

    return m;
  }

  LIB::BaseMap m = boost::apply_visitor(EvalBaseMap(env_), AST::Expr(v));
  
  Util::ERROR_UNLESS(m.arity() == nmbr_dims_ || m.arity() == 0
    , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", m, ")\n");

  return m;
}

// ----- //

ExprBaseType EvalExpression::operator()(AST::PWLMap v) const
{
  if (nmbr_dims_ == 1 && opt_conds_) {
    LIB::CanonPWMap pw = boost::apply_visitor(EvalCanonPWMap(env_), AST::Expr(v));

    Util::ERROR_UNLESS(pw.arity() == 1 || pw.arity() == 0
      , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", pw, ")\n");

    return pw;
  }

  LIB::BasePWMap pw =  boost::apply_visitor(EvalBasePWMap(env_), AST::Expr(v));

  Util::ERROR_UNLESS(pw.arity() == nmbr_dims_ || pw.arity() == 0
    , "EvalExpr: nmbr_dims_: ", nmbr_dims_, " != arity(", pw, ")\n");

  return pw;
}

ExprBaseType EvalExpression::operator()(AST::SBG v) const
{
  if (nmbr_dims_ == 1 && opt_conds_)
    return boost::apply_visitor(EvalCanonSBG(env_), AST::Expr(v));

  return boost::apply_visitor(EvalBaseSBG(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::DSBG v) const
{
  if (nmbr_dims_ == 1 && opt_conds_)
    return boost::apply_visitor(EvalCanonDSBG(env_), AST::Expr(v));

  return boost::apply_visitor(EvalBaseDSBG(env_), AST::Expr(v));
}

} // namespace Eval

} // namespace SBG
