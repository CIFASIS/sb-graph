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

////////////////////////////////////////////////////////////////////////////////
// Overload pattern ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Provides in-place lambdas for visitation for the different
 * operations. These are needed because different structures share the same
 * functions (for example, isEmpty can be applied to intervals, sets, etc.).
 */

template<class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

////////////////////////////////////////////////////////////////////////////////
// Function visitors -----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

auto empty_visitor_ = Overload {
  [](LIB::Interval a) { return a.isEmpty(); },
  [](LIB::MultiDimInter a) { return a.isEmpty(); },
  [](LIB::Set a) { return a.isEmpty(); },
  [](auto a) { 
    Util::ERROR("empty_visitor_: wrong argument ", a, " for isEmpty\n"); 
    return false;
  }
};

auto min_visitor_ = Overload {
  [](LIB::Interval a) { return LIB::MD_NAT(a.begin()); },
  [](LIB::MultiDimInter a) { return a.minElem(); },
  [](LIB::Set a) { return a.minElem(); },
  [](auto a) { 
    Util::ERROR("min_visitor_: wrong argument ", a, " for minElem\n"); 
    return LIB::MD_NAT();
  }
};

auto max_visitor_ = Overload {
  [](LIB::Interval a) { return LIB::MD_NAT(a.end()); },
  [](LIB::MultiDimInter a) { return a.maxElem(); },
  [](LIB::Set a) { return a.maxElem(); },
  [](auto a) { 
    Util::ERROR("max_visitor_: wrong argument ", a, " for maxElem\n"); 
    return LIB::MD_NAT(); 
  }
};

auto compose_visitor_ = Overload {
  [](LIB::LExp a, LIB::LExp b) { return ExprBaseType(a.composition(b)); },
  [](LIB::Exp a, LIB::Exp b) { return ExprBaseType(a.composition(b)); },
  [](LIB::Map a, LIB::Map b) { return ExprBaseType(a.composition(b)); },
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.composition(b)); },
  [](auto a, auto b) {
    Util::ERROR("compose_visitor_: wrong arguments ", a, ", ", b
      , " for compose\n"); 
    return ExprBaseType(); 
   }
};

auto inverse_visitor_ = Overload {
  [](LIB::LExp a) { return ExprBaseType(a.inverse()); },
  [](LIB::Exp a) { return ExprBaseType(a.inverse()); },
  [](LIB::Map a) { return ExprBaseType(a.minInv()); },
  [](LIB::PWMap a) { return ExprBaseType(a.inverse()); },
  [](auto a) { 
    Util::ERROR("inverse_visitor_: wrong arguments ", a, " for inverse\n"); 
    return ExprBaseType();
  }
};

auto image_visitor_ = Overload {
  [](LIB::Map a) { return ExprBaseType(a.image()); },
  [](LIB::PWMap a) { return ExprBaseType(a.image()); },
  [](auto a) { 
    Util::ERROR("image_visitor_: wrong argument ", a, " for image\n"); 
    return ExprBaseType();
  }
};

auto image2_visitor_ = Overload {
  [](LIB::Set a, LIB::Map b) { return ExprBaseType(b.image(a)); },
  [](LIB::Set a, LIB::PWMap b) { return ExprBaseType(b.image(a)); },
  [](auto a, auto b) { 
    Util::ERROR("image2_visitor_: wrong arguments ", a, ", ", b
      , " for image2\n"); 
    return ExprBaseType();
  }
};

auto pre_image2_visitor_ = Overload {
  [](LIB::Set a, LIB::Map b) { return ExprBaseType(b.preImage(a)); },
  [](LIB::Set a, LIB::PWMap b) { return ExprBaseType(b.preImage(a)); },
  [](auto a, auto b) { 
    Util::ERROR("pre_image2_visitor_: wrong arguments ", a, ", ", b
      , " for pre-image2\n"); 
    return ExprBaseType(); 
  }
};

auto dom_visitor_ = Overload {
  [](LIB::Map a) { return ExprBaseType(a.dom()); },
  [](LIB::PWMap a) { return ExprBaseType(a.dom()); },
  [](auto a) {
    Util::ERROR("dom_visitor_: wrong arguments ", a, "for dom\n");
    return ExprBaseType();
  }
};

auto combine_visitor_ = Overload {
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.combine(b)); },
  [](auto a, auto b) { 
    Util::ERROR("combine_visitor_: wrong arguments ", a, ", ", b
      , " for combine\n"); 
    return ExprBaseType(); 
  }
};

auto first_inv_visitor_ = Overload {
  [](LIB::PWMap a) { return ExprBaseType(a.firstInv()); },
  [](auto a) {
    Util::ERROR("first_inv_visitor_: wrong argument ", a, " for firstInv\n"); 
    return ExprBaseType();
  }
};

auto min_map_visitor_ = Overload {
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.minMap(b)); },
  [](auto a, auto b) {
    Util::ERROR("min_map_visitor_: wrong arguments ", a, ", ", b
      , " for minMap\n"); 
    return ExprBaseType();
  }
};

auto reduce_visitor_ = Overload {
  [](LIB::PWMap a) { return ExprBaseType(a.reduce()); },
  [](auto a) {
    Util::ERROR("reduce_visitor_: wrong argument ", a, " for reduce\n"); 
    return ExprBaseType();
  }
};

auto min_adj_visitor_ = Overload {
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.minAdjMap(b)); },
  [](auto a, auto b) { 
    Util::ERROR("min_adj_visitor_: wrong arguments ", a, ", ", b
      , " for minAdj\n"); 
    return ExprBaseType(); 
  }
};

auto inf_visitor_ = Overload {
  [](LIB::PWMap a) { return ExprBaseType(a.mapInf()); },
  [](auto a) { 
    Util::ERROR("inf_visitor_: wrong argument ", a, " for mapInf\n"); 
    return ExprBaseType(); 
  }
};

auto connected_visitor_ = Overload {
  [](LIB::SBG a) { return ExprBaseType(connectedComponents(a)); },
  [](auto a) {
    Util::ERROR("connected_visitor_: wrong argument ", a, " for CC\n"); 
    return ExprBaseType();
  }
};

auto matching_visitor_ = Overload {
  [](LIB::SBG a, LIB::MD_NAT b, bool c) { 
    LIB::SBGMatching match(a.copy(b[0]), c);
    return ExprBaseType(match.calculate());
  },
  [](auto a, auto b, auto c) {
    Util::ERROR("matching_visitor_: wrong arguments ", a, ", ", b
      , " for matching\n"); 
    return ExprBaseType();
  }
};

auto scc_visitor_ = Overload {
  [](LIB::DSBG a, bool b) { 
    LIB::SBGSCC scc(a, b);
    return ExprBaseType(scc.calculate());
  },
  [](auto a, auto b) {
    Util::ERROR("scc_visitor_: wrong argument ", a, " for scc\n"); 
    return ExprBaseType();
  }
};

auto ts_visitor_ = Overload {
  [](LIB::DSBG a, bool b) { 
    LIB::SBGTopSort ts(a, b);
    return ExprBaseType(ts.calculate()); 
  },
  [](auto a, auto b) {
    Util::ERROR("ts_visitor_: wrong argument ", a, " for sort\n"); 
    return ExprBaseType();
  }
};

auto match_scc_visitor_ = Overload {
  [](LIB::SBG a, LIB::MD_NAT b, bool c) { 
    LIB::SBGMatching match(a.copy(b[0]), c);
    match.calculate();
    LIB::SBGSCC scc(buildSCCFromMatching(match), c);
    return ExprBaseType(scc.calculate());
  },
  [](auto a, auto b, auto c) {
    Util::ERROR("match_scc_visitor_: wrong arguments ", a, ", ", b
      , " for matchSCC\n"); 
    return ExprBaseType();
  }
};

auto match_scc_ts_visitor_ = Overload {
  [](LIB::SBG a, LIB::MD_NAT b, bool c) { 
    LIB::SBGMatching match(a.copy(b[0]), c);
    LIB::Set match_res = match.calculate().matched_edges();
    LIB::SBGSCC scc(buildSCCFromMatching(match), c);
    LIB::PWMap scc_res = scc.calculate();
    LIB::SBGTopSort ts(buildSortFromSCC(scc, scc_res), c);
    LIB::PWMap ts_res = ts.calculate(); 
    buildJson(match_res, scc_res, ts_res);
    return ExprBaseType(ts_res);
  },
  [](auto a, auto b, auto c) {
    Util::ERROR("match_scc_ts_visitor_: wrong arguments ", a, ", ", b
      , " for matchSCCTS\n"); 
    return ExprBaseType();
  }
};

auto cut_visitor_ = Overload {
  [](LIB::DSBG a, bool b) { 
    LIB::SBGCutSet cut_set(a, b);
    return ExprBaseType(cut_set.calculate());
  },
  [](auto a, auto b) {
    Util::ERROR("cut_visitor_: wrong argument ", a, " for cut\n"); 
    return ExprBaseType();
  }
};

////////////////////////////////////////////////////////////////////////////////
// Expression evaluator --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

EvalExpression::EvalExpression(VarEnv env)
  : nmbr_dims_(1), fact_(LIB::UnordPWMapFact(LIB::MapFact(LIB::UnordSetFact())))
  , env_(env), debug_(false) {}
EvalExpression::EvalExpression(unsigned int nmbr_dims
  , const LIB::PWMapFact &fact , VarEnv env, bool debug)
  : nmbr_dims_(nmbr_dims), fact_(fact), env_(env), debug_(debug) {}

ExprBaseType EvalExpression::operator()(AST::Natural v) const
{
  return LIB::MD_NAT(v);
}

ExprBaseType EvalExpression::operator()(AST::Rational v) const
{
  return boost::apply_visitor(EvalRat(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::VariableName v) const 
{
  MaybeEBT v_opt = env_[v];
  if (v_opt)
    return *v_opt;

  Util::ERROR("EvalExpression: variable ", v, " undefined\n");
  return LIB::MD_NAT(0); 
}

ExprBaseType EvalExpression::operator()(AST::UnaryOp v) const
{
  ExprBaseType x = boost::apply_visitor(*this, v.expr());

  switch (v.op()) {
    case AST::UnOp::neg: 
      if (std::holds_alternative<LIB::MD_NAT>(x)) {
        LIB::MD_NAT x_value = std::get<LIB::MD_NAT>(x); 

        Util::ERROR_UNLESS(1 == x_value.arity()
          ,"EvalExpr: UnaryOp neg only supported for unidimensional values, "
          ,"arity(", x_value, ") = ", x_value.arity(), "\n");
       
        return LIB::RATIONAL(x_value[0], -1);
      }
      else if (std::holds_alternative<LIB::RATIONAL>(x)) {
        return std::get<LIB::RATIONAL>(x)*(-1);
      }

      Util::ERROR("EvalExpression: incompatible application of UnaryOp neg "
        , v, "\n");
      return LIB::MD_NAT(0);
      break;

    default:
      Util::ERROR("EvalExpression: UnaryOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
  }

  return LIB::MD_NAT(0); 
}

ExprBaseType EvalExpression::operator()(AST::BinOp v) const
{ 
  ExprBaseType vl = boost::apply_visitor(*this, v.left());
  ExprBaseType vr = boost::apply_visitor(*this, v.right());
  bool n1 = std::holds_alternative<LIB::MD_NAT>(vl);
  bool n2 = std::holds_alternative<LIB::MD_NAT>(vr);

  EvalRat visit_rat(env_);
  if (n1 && n2) {
    EvalNat visit_nat(env_);
    switch (v.op()) {
      case AST::Op::sub: {
        LIB::NAT xl = boost::apply_visitor(visit_nat, v.left());
        LIB::NAT xr = boost::apply_visitor(visit_nat, v.right());
        if (xl >= xr)
          return LIB::MD_NAT(xl - xr);
        else
          return boost::apply_visitor(visit_rat, AST::Expr(v));
        break;
      }

      default:
        return boost::apply_visitor(visit_rat, AST::Expr(v));
    } 
  }

  return boost::apply_visitor(visit_rat, AST::Expr(v));
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

          bool res = std::visit(empty_visitor_, eval_args[0]);
          return LIB::MD_NAT(res);
        }
        break;

      case Eval::Func::min:
        if (eval_args.size() == 1) {
          arity_ok = true;

          LIB::MD_NAT result = std::visit(min_visitor_, eval_args[0]);
          return result;
        }
        break;

      case Eval::Func::max:
        if (eval_args.size() == 1) {
          arity_ok = true;

          return std::visit(max_visitor_, eval_args[0]);
        }
        break;

      case Eval::Func::comp:
        if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(compose_visitor_, eval_args[0], eval_args[1]);
        }
        break;

      case Eval::Func::inv:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(inverse_visitor_, eval_args[0]);
        }
        break;

      case Eval::Func::im:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(image_visitor_, eval_args[0]);
        }

        else if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(image2_visitor_, eval_args[0], eval_args[1]);
        }

        break;

      case Eval::Func::preim:
        if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(pre_image2_visitor_, eval_args[0], eval_args[1]);
        }
        
        break;

      case Eval::Func::dom:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(dom_visitor_, eval_args[0]);
        }
        break;

      case Eval::Func::comb:
        if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(combine_visitor_, eval_args[0], eval_args[1]);
        }
        break;

      case Eval::Func::first_inv:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(first_inv_visitor_, eval_args[0]);
        }

      case Eval::Func::min_map:
        if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(min_map_visitor_, eval_args[0], eval_args[1]);
        }

      case Eval::Func::red:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(reduce_visitor_, eval_args[0]);
        }
        break;

      case Eval::Func::min_adj:
        if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(min_adj_visitor_, eval_args[0], eval_args[1]);
        }
        break;

      case Eval::Func::inf:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(inf_visitor_, eval_args[0]);
        }
        break;

      case Eval::Func::connected:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(connected_visitor_, eval_args[0]);
        }
        break;

      case Eval::Func::matching:
        if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(matching_visitor_, eval_args[0], eval_args[1]
            , std::variant<bool>(debug_));
        }
        break;

      case Eval::Func::scc:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(scc_visitor_, eval_args[0]
            , std::variant<bool>(debug_));
        }
        break;


      case Eval::Func::ts:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(ts_visitor_, eval_args[0]
            , std::variant<bool>(debug_));
        }
        break;

      case Eval::Func::match_scc:
        if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(match_scc_visitor_, eval_args[0], eval_args[1]
            , std::variant<bool>(debug_));
        }
        break;

      case Eval::Func::match_scc_ts:
        if (eval_args.size() == 2) {
          arity_ok = true;
          return std::visit(match_scc_ts_visitor_, eval_args[0], eval_args[1]
            , std::variant<bool>(debug_));
        }
        break;

      case Eval::Func::cut_set:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(cut_visitor_, eval_args[0]
            , std::variant<bool>(debug_));
        }
        break;

      default:
        Util::ERROR("EvalExpression: function ", vname, " not implemented\n");
        return LIB::MD_NAT(0);
    }

    if (!arity_ok) {
      Util::ERROR("EvalExpression: wrong number of arguments for Call "
        , vname, "\n");
      return LIB::MD_NAT(0);
    }
  }

  Util::ERROR("EvalExpression: function ", vname, " doesn't exist\n");
  return LIB::MD_NAT(0);
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
      return LIB::MD_NAT(i.cardinal());

    default:
      Util::ERROR("EvalExpression: InterUnaryOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
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
      return LIB::MD_NAT(l < r);

    case AST::ContainerOp::eq:
      return LIB::MD_NAT(l == r);

    default:
      Util::ERROR("EvalExpression: InterBinOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
  }
}

ExprBaseType EvalExpression::operator()(AST::MultiDimInter v) const
{
  return boost::apply_visitor(EvalMDI(nmbr_dims_, env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::MDInterUnaryOp v) const
{
  EvalMDI visit_mdi(nmbr_dims_, env_);
  LIB::MultiDimInter mdi = boost::apply_visitor(visit_mdi, v.e());

  switch (v.op()) {
    case AST::ContainerUOp::card:
      return LIB::MD_NAT(mdi.cardinal());

    default:
      Util::ERROR("EvalExpression: MDInterUnaryOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
  }

  return LIB::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::MDInterBinOp v) const
{
  EvalMDI visit_mdi(nmbr_dims_, env_);
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
      return LIB::MD_NAT(l < r);

    case AST::ContainerOp::eq:
      return LIB::MD_NAT(l == r);

    default:
      Util::ERROR("EvalExpression: MDInterBinOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
  }


  return LIB::MD_NAT(0);
}


ExprBaseType EvalExpression::operator()(AST::Set v) const
{
  return boost::apply_visitor(EvalSet(nmbr_dims_, fact_, env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::SetUnaryOp v) const
{
  SBG::LIB::Set s
    = boost::apply_visitor(EvalSet(nmbr_dims_, fact_, env_), v.e());

  switch (v.op()) {
    case AST::ContainerUOp::card:
      return LIB::MD_NAT(s.cardinal());

    default:
      Util::ERROR("EvalExpression: SetUnaryOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
  }

  return LIB::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::SetBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalSet visit_set(nmbr_dims_, fact_, env_);
  LIB::Set sl = boost::apply_visitor(visit_set, l);
  LIB::Set sr = boost::apply_visitor(visit_set, r);

  switch (v.op()) {
    case AST::ContainerOp::cap:
      return sl.intersection(sr);

    case AST::ContainerOp::cup:
      return sl.cup(sr);

    case AST::ContainerOp::diff:
      return sl.difference(sr);

    case AST::ContainerOp::eq:
      return LIB::MD_NAT(sl == sr);

    default:
      Util::ERROR("EvalExpression: SetBinOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
  }

  return LIB::MD_NAT(0);
}

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
      return LIB::MD_NAT(lexp == rexp);

    case AST::ExpOp::add:
      return lexp + rexp;

    case AST::ExpOp::sub:
      return lexp - rexp;

    default:
      Util::ERROR("EvalExpression: LExpBinOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
  }
}

ExprBaseType EvalExpression::operator()(AST::MDLExp v) const
{
  return boost::apply_visitor(EvalMDLE(nmbr_dims_, env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::MDLExpBinOp v) const
{
  AST::Expr l = v.left(), r = v.right();
  EvalMDLE visit_le(nmbr_dims_, env_);
  LIB::Exp lexp = boost::apply_visitor(visit_le, l);
  LIB::Exp rexp = boost::apply_visitor(visit_le, r);

  switch (v.op()) {
    case AST::ExpOp::eq:
      return LIB::MD_NAT(lexp == rexp);

    case AST::ExpOp::add:
      return lexp + rexp;

    case AST::ExpOp::sub:
      return lexp - rexp;

    default:
      Util::ERROR("EvalExpression: MDLExpBinOp ", v.op(), " unsupported\n");
      return LIB::MD_NAT(0);
  }

  return LIB::MD_NAT(0);
}

ExprBaseType EvalExpression::operator()(AST::LinearMap v) const
{
  return boost::apply_visitor(EvalMap(nmbr_dims_, fact_, env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::PWLMap v) const
{
  return boost::apply_visitor(EvalPWMap(nmbr_dims_, fact_, env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::SBG v) const
{
  return boost::apply_visitor(EvalSBG(nmbr_dims_, fact_, env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::DSBG v) const
{
  return boost::apply_visitor(EvalDSBG(nmbr_dims_, fact_, env_), AST::Expr(v));
}

} // namespace Eval

} // namespace SBG
