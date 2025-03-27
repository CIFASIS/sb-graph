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

auto oppo_visitor_ = Overload {
  [](LIB::NAT a) { return LIB::RATIONAL(a, -1); },
  [](LIB::RATIONAL a) { return LIB::RATIONAL(-1)*a; },
  [](auto a) { 
    Util::ERROR("oppo_visitor_: wrong argument ", a, " for - (opposite)\n");
    return LIB::RATIONAL(0);
  }
};

auto cardinal_visitor_ = Overload {
  [](LIB::Interval a) { return (LIB::NAT) a.cardinal(); },
  [](LIB::MultiDimInter a) { return (LIB::NAT) a.cardinal(); },
  [](LIB::Set a) { return (LIB::NAT) a.cardinal(); },
  [](auto a) { 
    Util::ERROR("empty_visitor_: wrong argument ", a, " for #\n");
    return (LIB::NAT) 0;
  }
};

auto complement_visitor_ = Overload {
  [](LIB::Set a) { return ExprBaseType(a.complement()); },
  [](auto a) { 
    Util::ERROR("empty_visitor_: wrong argument ", a, " for \' (complement)\n"); 
    return ExprBaseType();
  }
};

auto add_visitor_ = Overload {
  [](LIB::NAT a, LIB::NAT b) { return ExprBaseType(a + b); },
  [](LIB::MD_NAT a, LIB::MD_NAT b) { return ExprBaseType(a + b); },
  [](LIB::RATIONAL a, LIB::RATIONAL b) { return ExprBaseType(a + b); },
  [](LIB::NAT a, LIB::RATIONAL b) {
    return ExprBaseType(LIB::RATIONAL(a) + b);
  },
  [](LIB::RATIONAL a, LIB::NAT b) {
    return ExprBaseType(a + LIB::RATIONAL(b));
  },
  [](LIB::Exp a, LIB::Exp b) { return ExprBaseType(a + b); },
  [](LIB::Map a, LIB::Map b) { return ExprBaseType(a + b); },
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a + b); },
  [](auto a, auto b) { 
    Util::ERROR("add_visitor_: wrong arguments ", a, ", ", b
      , " for operator+\n"); 
    return ExprBaseType();
  }
};

auto sub_visitor_ = Overload {
  [](LIB::NAT a, LIB::NAT b) {
    if (a > b)
      return ExprBaseType(LIB::NAT(a - b));
    else
      return ExprBaseType(LIB::RATIONAL(a) - LIB::RATIONAL(b));
  },
  [](LIB::RATIONAL a, LIB::RATIONAL b) { return ExprBaseType(a - b); },
  [](LIB::NAT a, LIB::RATIONAL b) {
    return ExprBaseType(LIB::RATIONAL(a) - b);
  },
  [](LIB::RATIONAL a, LIB::NAT b) {
    return ExprBaseType(a - LIB::RATIONAL(b));
  },
  [](LIB::Exp a, LIB::Exp b) { return ExprBaseType(a - b); },
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a - b); },
  [](auto a, auto b) { 
    Util::ERROR("sub_visitor_: wrong arguments ", a, ", ", b
      , " for operator-\n"); 
    return ExprBaseType();
  }
};

auto mult_visitor_ = Overload {
  [](LIB::NAT a, LIB::NAT b) { return ExprBaseType(a*b); },
  [](LIB::RATIONAL a, LIB::RATIONAL b) { return ExprBaseType(a*b); },
  [](LIB::NAT a, LIB::RATIONAL b) { return ExprBaseType(LIB::RATIONAL(a)*b); },
  [](LIB::RATIONAL a, LIB::NAT b) { return ExprBaseType(a*LIB::RATIONAL(b)); },
  [](auto a, auto b) { 
    Util::ERROR("mult_visitor_: wrong arguments ", a, ", ", b
      , " for operator*\n"); 
    return ExprBaseType();
  }
};

auto eq_visitor_ = Overload {
  [](LIB::MD_NAT a, LIB::MD_NAT b) { return Boolean(a == b); },
  [](LIB::RATIONAL a, LIB::RATIONAL b) { return Boolean(a == b); },
  [](LIB::Interval a, LIB::Interval b) { return Boolean(a == b); },
  [](LIB::SetPiece a, LIB::SetPiece b) { return Boolean(a == b); },
  [](LIB::Set a, LIB::Set b) { return Boolean(a == b); },
  [](LIB::Exp a, LIB::Exp b) { return Boolean(a == b); },
  [](LIB::Map a, LIB::Map b) { return Boolean(a == b); },
  [](LIB::PWMap a, LIB::PWMap b) { return Boolean(a == b); },
  [](auto a, auto b) {
    Util::ERROR("eq_visitor_: wrong arguments ", a, ", ", b
      , " for operator==\n");
    return Boolean();
  }
};

auto less_visitor_ = Overload {
  [](LIB::MD_NAT a, LIB::MD_NAT b) { return Boolean(a < b); },
  [](LIB::RATIONAL a, LIB::RATIONAL b) { return Boolean(a < b); },
  [](LIB::Interval a, LIB::Interval b) { return Boolean(a < b); },
  [](LIB::SetPiece a, LIB::SetPiece b) { return Boolean(a < b); },
  [](auto a, auto b) { 
    Util::ERROR("less_visitor_: wrong arguments ", a, ", ", b
      , " for operator<\n"); 
    return Boolean();
  }
};

auto cap_visitor_ = Overload{
  [](LIB::Interval a, LIB::Interval b) {
    return ExprBaseType(a.intersection(b));
  },
  [](LIB::SetPiece a, LIB::SetPiece b) {
    return ExprBaseType(a.intersection(b));
  },
  [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.intersection(b)); },
  [](auto a, auto b) { 
    Util::ERROR("cap_visitor_: wrong arguments ", a, ", ", b
      , " for intersection\n"); 
    return ExprBaseType();
  }
};

auto cup_visitor_ = Overload{
  [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.cup(b)); },
  [](auto a, auto b) { 
    Util::ERROR("cap_visitor_: wrong arguments ", a, ", ", b
      , " for union\n"); 
    return ExprBaseType();
  }
};

auto diff_visitor_ = Overload{
  [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.difference(b)); },
  [](auto a, auto b) { 
    Util::ERROR("diff_visitor_: wrong arguments ", a, ", ", b
      , " for difference\n"); 
    return ExprBaseType();
  }
};

auto empty_visitor_ = Overload {
  [](LIB::Interval a) { return Boolean(a.isEmpty()); },
  [](LIB::MultiDimInter a) { return Boolean(a.isEmpty()); },
  [](LIB::Set a) { return Boolean(a.isEmpty()); },
  [](auto a) { 
    Util::ERROR("empty_visitor_: wrong argument ", a, " for isEmpty\n"); 
    return Boolean();
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
  [](LIB::SBG a, LIB::NAT b, bool c) { 
    LIB::SBGMatching match(a.copy(b), c);
    return ExprBaseType(match.calculate());
  },
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
  [](LIB::SBG a, LIB::NAT b, bool c) { 
    LIB::SBGMatching match(a.copy(b), c);
    match.calculate();
    LIB::SBGSCC scc(buildSCCFromMatching(match), c);
    return ExprBaseType(scc.calculate());
  },
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
  [](LIB::SBG a, LIB::NAT b, bool c) { 
    LIB::SBGMatching match(a.copy(b), c);
    LIB::Set match_res = match.calculate().matched_edges();
    LIB::SBGSCC scc(buildSCCFromMatching(match), c);
    LIB::PWMap scc_res = scc.calculate();
    LIB::SBGTopSort ts(buildSortFromSCC(scc, scc_res), c);
    LIB::PWMap ts_res = ts.calculate(); 
    buildJson(match_res, scc_res, ts_res);
    return ExprBaseType(ts_res);
  },
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

template<typename T>
T eval(const EvalExpression &visit, AST::Expr e)
{
  ExprBaseType visited = boost::apply_visitor(visit, e);

  Util::ERROR_UNLESS(std::holds_alternative<T>(visited)
    , "EvalExpr: expression ", e, " is not ???\n");

  return std::get<T>(visited);
}

EvalExpression::EvalExpression(unsigned int nmbr_dims
  , const LIB::PWMapAF &fact , VarEnv env, bool debug)
  : nmbr_dims_(nmbr_dims), fact_(fact), env_(env), debug_(debug) {}

ExprBaseType EvalExpression::operator()(AST::Natural v) const
{
  return (LIB::NAT) v;
}

ExprBaseType EvalExpression::operator()(AST::Rational v) const
{
  return boost::apply_visitor(EvalRat(env_), AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::Name v) const 
{
  MaybeEBT v_opt = env_[v];
  if (v_opt)
    return *v_opt;

  Util::ERROR("EvalExpression: variable ", v, " undefined\n");
  return ExprBaseType(); 
}

ExprBaseType EvalExpression::operator()(AST::UnaryOp v) const
{
  ExprBaseType x = boost::apply_visitor(*this, v.expr());

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
      Util::ERROR("EvalExpression: UnaryOp ", v.op(), " unsupported\n");
      return ExprBaseType();
  }

  return ExprBaseType(); 
}

ExprBaseType EvalExpression::operator()(AST::BinOp v) const
{ 
  ExprBaseType vl = boost::apply_visitor(*this, v.left());
  ExprBaseType vr = boost::apply_visitor(*this, v.right());

  EvalNat visit_nat(env_);
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
      Util::ERROR("EvalExpression: BinOp ", v.op(), " unsupported\n");
      return ExprBaseType(); 
  } 

  return ExprBaseType(); 
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
          return std::visit(empty_visitor_, eval_args[0]);
        }
        break;

      case Eval::Func::min:
        if (eval_args.size() == 1) {
          arity_ok = true;
          return std::visit(min_visitor_, eval_args[0]);
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
        return ExprBaseType();
    }

    if (!arity_ok) {
      Util::ERROR("EvalExpression: wrong number of arguments for Call "
        , vname, "\n");
      return ExprBaseType();
    }
  }

  Util::ERROR("EvalExpression: function ", vname, " doesn't exist\n");
  return ExprBaseType();
}

ExprBaseType EvalExpression::operator()(AST::Interval v) const
{
  EvalNat visit_nat(env_);

  LIB::NAT b = boost::apply_visitor(visit_nat, v.begin());
  LIB::NAT s = boost::apply_visitor(visit_nat, v.step());
  LIB::NAT e = boost::apply_visitor(visit_nat, v.end());

  return LIB::Interval(b, s, e);
}

ExprBaseType EvalExpression::operator()(AST::MultiDimInter v) const
{
  LIB::SetPiece res;

  for (const AST::Expr &e : v.intervals()) 
    res.emplaceBack(eval<LIB::Interval>(*this, e));

  Util::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalMDI[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType EvalExpression::operator()(AST::Set v) const
{
  LIB::Set res = fact_.createSet();

  for (const AST::Expr &e : v.pieces())
    res.emplaceBack(eval<LIB::SetPiece>(*this, e));

  Util::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalExpr[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType EvalExpression::operator()(AST::LinearExp v) const
{
  EvalLE visit_le(env_);
  return boost::apply_visitor(visit_le, AST::Expr(v));
}

ExprBaseType EvalExpression::operator()(AST::MDLExp v) const
{
  LIB::Exp res;

  EvalLE visit_le(env_);
  for (const AST::Expr &e : v.exps()) {
    LIB::LExp ith = boost::apply_visitor(visit_le, e);
    res.emplaceBack(ith);
  }

  Util::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalExpr[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType EvalExpression::operator()(AST::LinearMap v) const
{
  LIB::Set d = eval<LIB::Set>(*this, v.dom());
  LIB::Exp e = eval<LIB::Exp>(*this, v.lexp());

  LIB::Map res = fact_.createMap(d, e);

  Util::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalExpr[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType EvalExpression::operator()(AST::PWLMap v) const
{
  LIB::PWMap res = fact_.createPWMap();

  for (const AST::Expr &e : v.maps())
    res.emplaceBack(eval<LIB::Map>(*this, e));

  Util::ERROR_UNLESS(res.arity() == nmbr_dims_ || res.arity() == 0
    , "EvalExpr[nmbr_dims = ", nmbr_dims_, "]: arity(", res, ") = "
    , res.arity(), "\n");

  return res;
}

ExprBaseType EvalExpression::operator()(AST::SBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V());
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap());
  LIB::PWMap map1 = eval<LIB::PWMap>(*this, v.map1());
  LIB::PWMap map2 = eval<LIB::PWMap>(*this, v.map2());
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap());
  LIB::PWMap subE = eval<LIB::PWMap>(*this, v.subE_map());

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &m : Emap) {
      LIB::Set dom = m.dom();
      for (const LIB::SetPiece &mdi : dom) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(fact_.createMap(fact_.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::SBG(fact_, V, Vmap, map1, map2, Emap, subE);
}

ExprBaseType EvalExpression::operator()(AST::DSBG v) const
{
  LIB::Set V = eval<LIB::Set>(*this, v.V());
  LIB::PWMap Vmap = eval<LIB::PWMap>(*this, v.Vmap());
  LIB::PWMap mapB = eval<LIB::PWMap>(*this, v.mapB());
  LIB::PWMap mapD = eval<LIB::PWMap>(*this, v.mapD());
  LIB::PWMap Emap = eval<LIB::PWMap>(*this, v.Emap());
  LIB::PWMap subE = eval<LIB::PWMap>(*this, v.subE_map());

  if (subE.dom().isEmpty() && !Emap.dom().isEmpty()) {
    unsigned int j = 1;
    for (const LIB::Map &sbgmap : Emap) {
      LIB::Set dom = sbgmap.dom();
      for (const LIB::SetPiece &mdi : dom) {
        LIB::Exp off(LIB::MD_NAT(mdi.arity(), j));
        subE.emplaceBack(fact_.createMap(fact_.createSet(mdi), off)); 
        ++j;
      }
    }
  } 

  return LIB::DSBG(fact_, V, Vmap, mapB, mapD, Emap, subE);
}

ExprBaseType EvalExpression::operator()(AST::ParenExpr v) const
{
  return boost::apply_visitor(*this, v.e());
}

} // namespace Eval

} // namespace SBG
