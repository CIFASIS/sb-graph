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
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/misc/causalization_builders.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/ts_fact.hpp"
#include "eval/visitors/func_evaluator.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Built-in operators evaluators -----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

ExprBaseType BuiltInOperators::oppositeEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "oppositeEvaluator: wrong number of arguments\n");

  auto opposite_evaluator = Overload {
    [](LIB::NAT a) { return ExprBaseType(LIB::RATIONAL(a, -1)); },
    [](LIB::RATIONAL a) { return ExprBaseType(LIB::RATIONAL(-1)*a); },
    [](LIB::Set a) { return ExprBaseType(a.complement()); },
    [](auto a) { 
      Util::ERROR("oppositeEvaluator: wrong type argument ", a
        , " for - (opposite)\n");
      return ExprBaseType(LIB::RATIONAL(0));
    }
  };
  return std::visit(opposite_evaluator, args[0]);
}

ExprBaseType BuiltInOperators::cardinalEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "cardinalEvaluator: wrong number of arguments\n");

  const auto cardinal_evaluator = Overload {
    [](LIB::Interval a) { return (LIB::NAT) a.cardinal(); },
    [](LIB::MultiDimInter a) { return (LIB::NAT) a.cardinal(); },
    [](LIB::Set a) { return (LIB::NAT) a.cardinal(); },
    [](auto a) { 
      Util::ERROR("cardinalEvaluator: wrong argument ", a, " for #\n");
      return (LIB::NAT) 0;
    }
  };
  return std::visit(cardinal_evaluator, args[0]);
}

ExprBaseType BuiltInOperators::complementEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "complementEvaluator: wrong number of arguments\n");

  const auto complement_evaluator = Overload {
    [](LIB::Set a) { return ExprBaseType(a.complement()); },
    [](auto a) { 
      Util::ERROR("complementEvaluator: wrong argument ", a
        , " for \' (complement)\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(complement_evaluator, args[0]);
}

ExprBaseType BuiltInOperators::addEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "addEvaluator: wrong number of arguments\n");

  const auto add_evaluator = Overload {
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
      Util::ERROR("addEvaluator: wrong arguments ", a, ", ", b
        , " for operator+\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(add_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::subEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "subEvaluator: wrong number of arguments\n");
  
  const auto sub_evaluator = Overload {
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
    [](auto a, auto b) { 
      Util::ERROR("subEvaluator: wrong arguments ", a, ", ", b
        , " for operator-\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(sub_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::multEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "multEvaluator: wrong number of arguments\n");

  const auto mult_evaluator = Overload {
    [](LIB::NAT a, LIB::NAT b) { return ExprBaseType(a*b); },
    [](LIB::RATIONAL a, LIB::RATIONAL b) { return ExprBaseType(a*b); },
    [](LIB::NAT a, LIB::RATIONAL b) { return ExprBaseType(LIB::RATIONAL(a)*b); },
    [](LIB::RATIONAL a, LIB::NAT b) { return ExprBaseType(a*LIB::RATIONAL(b)); },
    [](auto a, auto b) { 
      Util::ERROR("multEvaluator: wrong arguments ", a, ", ", b
        , " for operator*\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(mult_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::eqEvaluator(const EBTList& args)
{  
  Util::ERROR_UNLESS(args.size() == 2
    , "eqEvaluator: wrong number of arguments\n");

  const auto eq_evaluator = Overload {
    [](LIB::MD_NAT a, LIB::MD_NAT b) { return a == b; },
    [](LIB::RATIONAL a, LIB::RATIONAL b) { return a == b; },
    [](LIB::Interval a, LIB::Interval b) { return a == b; },
    [](LIB::SetPiece a, LIB::SetPiece b) { return a == b; },
    [](LIB::Set a, LIB::Set b) { return a == b; },
    [](LIB::Exp a, LIB::Exp b) { return a == b; },
    [](LIB::Map a, LIB::Map b) { return a == b; },
    [](LIB::PWMap a, LIB::PWMap b) { return a == b; },
    [](auto a, auto b) {
      Util::ERROR("eqEvaluator: wrong arguments ", a, ", ", b
        , " for operator==\n");
      return false;
    }
  };
  return std::visit(eq_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::lessEvaluator(const EBTList& args)
{ 
  Util::ERROR_UNLESS(args.size() == 2
    , "lessEvaluator: wrong number of arguments\n");

  const auto less_evaluator = Overload {
    [](LIB::MD_NAT a, LIB::MD_NAT b) { return a < b; },
    [](LIB::RATIONAL a, LIB::RATIONAL b) { return a < b; },
    [](LIB::Interval a, LIB::Interval b) { return a < b; },
    [](LIB::SetPiece a, LIB::SetPiece b) { return a < b; },
    [](auto a, auto b) { 
      Util::ERROR("lessEvaluator: wrong arguments ", a, ", ", b
        , " for operator<\n"); 
      return false;
    }
  };
  return std::visit(less_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::capEvaluator(const EBTList& args)
{ 
  Util::ERROR_UNLESS(args.size() == 2
    , "capEvaluator: wrong number of arguments\n");

  const auto cap_evaluator = Overload{
    [](LIB::Interval a, LIB::Interval b) {
      return ExprBaseType(a.intersection(b));
    },
    [](LIB::SetPiece a, LIB::SetPiece b) {
      return ExprBaseType(a.intersection(b));
    },
    [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.intersection(b)); },
    [](auto a, auto b) { 
      Util::ERROR("capEvaluator: wrong arguments ", a, ", ", b
        , " for intersection\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(cap_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::cupEvaluator(const EBTList& args)
{ 
  Util::ERROR_UNLESS(args.size() == 2
    , "cupEvaluator: wrong number of arguments\n");

  const auto cup_evaluator = Overload{
    [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.cup(b)); },
    [](auto a, auto b) { 
      Util::ERROR("cupEvaluator: wrong arguments ", a, ", ", b
        , " for union\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(cup_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::diffEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "diffEvaluator: wrong number of arguments\n");

  const auto diff_evaluator = Overload{
    [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.difference(b)); },
    [](auto a, auto b) { 
      Util::ERROR("diffEvaluator: wrong arguments ", a, ", ", b
        , " for difference\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(diff_evaluator, args[0], args[1]);
}

// Operators evaluators --------------------------------------------------------

UnaryOpEvaluator::UnaryOpEvaluator() {}

ExprBaseType UnaryOpEvaluator::evaluate(EBTList& evaluated_args, AST::UnOp op)
{
  switch (op) {
    case AST::UnOp::oppo:
      return BuiltInOperators::oppositeEvaluator(evaluated_args);
      break;

    case AST::UnOp::card:
      return BuiltInOperators::cardinalEvaluator(evaluated_args);
      break;

    case AST::UnOp::comp:
      return BuiltInOperators::complementEvaluator(evaluated_args);
      break;

    default:
      Util::ERROR("UnaryOpEvaluator: UnaryOp ", op, " unsupported\n");
      return ExprBaseType();
  }

  return ExprBaseType(); 
}

BinOpEvaluator::BinOpEvaluator() {}

ExprBaseType BinOpEvaluator::evaluate(EBTList& evaluated_args, AST::Op op)
{
  switch (op) {
    case AST::Op::add:
      return BuiltInOperators::addEvaluator(evaluated_args);
      break;

    case AST::Op::sub:
      return BuiltInOperators::subEvaluator(evaluated_args);
      break;

    case AST::Op::mult:
      return BuiltInOperators::multEvaluator(evaluated_args);
      break;

    case AST::Op::eq:
      return BuiltInOperators::eqEvaluator(evaluated_args);
      break;

    case AST::Op::less:
      return BuiltInOperators::lessEvaluator(evaluated_args);
      break;

    case AST::Op::cap:
      return BuiltInOperators::capEvaluator(evaluated_args);
      break;

    case AST::Op::cup:
      return BuiltInOperators::cupEvaluator(evaluated_args);
      break;

    case AST::Op::diff:
      return BuiltInOperators::diffEvaluator(evaluated_args);
      break;

    default:
      Util::ERROR("BinOpEvaluator: BinOp ", op, " unsupported\n");
      return ExprBaseType(); 
  } 

  return ExprBaseType(); 
}

////////////////////////////////////////////////////////////////////////////////
// Built-in functions evaluators -----------------------------------------------
////////////////////////////////////////////////////////////////////////////////
  
ExprBaseType BuiltInFunctions::emptyEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1 
    , "emptyEvaluator: wrong number of arguments\n");

  const auto empty_evaluator = Overload {
    [](LIB::Interval a) { return a.isEmpty(); },
    [](LIB::MultiDimInter a) { return a.isEmpty(); },
    [](LIB::Set a) { return a.isEmpty(); },
    [](auto a) { 
      Util::ERROR("emptyEvaluator: wrong argument ", a, " for isEmpty\n"); 
      return false;
    }
  };
  return std::visit(empty_evaluator, args[0]);
}
  
ExprBaseType BuiltInFunctions::minEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "minEvaluator: wrong number of arguments\n");

  const auto min_evaluator = Overload {
    [](LIB::Interval a) { return LIB::MD_NAT(a.begin()); },
    [](LIB::MultiDimInter a) { return a.minElem(); },
    [](LIB::Set a) { return a.minElem(); },
    [](auto a) { 
      Util::ERROR("minEvaluator: wrong argument ", a, " for minElem\n"); 
      return LIB::MD_NAT();
    }
  };
  return std::visit(min_evaluator, args[0]);
}
 
ExprBaseType BuiltInFunctions::maxEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "maxEvaluator: wrong number of arguments\n");

  const auto max_evaluator = Overload {
    [](LIB::Interval a) { return LIB::MD_NAT(a.end()); },
    [](LIB::MultiDimInter a) { return a.maxElem(); },
    [](LIB::Set a) { return a.maxElem(); },
    [](auto a) { 
      Util::ERROR("maxEvaluator: wrong argument ", a, " for maxElem\n"); 
      return LIB::MD_NAT(); 
    }
  };
  return std::visit(max_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::restrictEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "restrictEvaluator: wrong number of arguments\n");

  const auto restrict_evaluator = Overload {
    [](LIB::PWMap a, LIB::Set b) { return ExprBaseType(a.restrict(b)); },
    [](auto a, auto b) {
      Util::ERROR("restrictEvaluator: wrong arguments ", a, ", ", b
        , " for restrict\n"); 
      return ExprBaseType(); 
     }
  };
  return std::visit(restrict_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::composeEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "composeEvaluator: wrong number of arguments\n");

  const auto compose_evaluator = Overload {
    [](LIB::LExp a, LIB::LExp b) { return ExprBaseType(a.composition(b)); },
    [](LIB::Exp a, LIB::Exp b) { return ExprBaseType(a.composition(b)); },
    [](LIB::Map a, LIB::Map b) { return ExprBaseType(a.composition(b)); },
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.composition(b)); },
    [](auto a, auto b) {
      Util::ERROR("composeEvaluator: wrong arguments ", a, ", ", b
        , " for compose\n"); 
      return ExprBaseType(); 
     }
  };
  return std::visit(compose_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::inverseEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "inverseEvaluator: wrong number of arguments\n");

  const auto inverse_evaluator = Overload {
    [](LIB::LExp a) { return ExprBaseType(a.inverse()); },
    [](LIB::Exp a) { return ExprBaseType(a.inverse()); },
    [](LIB::Map a) { return ExprBaseType(a.minInv()); },
    [](LIB::PWMap a) { return ExprBaseType(a.inverse()); },
    [](auto a) { 
      Util::ERROR("inverseEvaluator: wrong arguments ", a, " for inverse\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(inverse_evaluator, args[0]);
}
 
ExprBaseType BuiltInFunctions::imageEvaluator(const EBTList& args)
{
  if (args.size() == 1) {
    const auto image_evaluator = Overload {
      [](LIB::Map a) { return ExprBaseType(a.image()); },
      [](LIB::PWMap a) { return ExprBaseType(a.image()); },
      [](auto a) { 
        Util::ERROR("imageEvaluator: wrong argument ", a, " for image\n"); 
        return ExprBaseType();
      }
    };
    return std::visit(image_evaluator, args[0]);
  }
  else if (args.size() == 2) {
    const auto image2_evaluator = Overload {
      [](LIB::Set a, LIB::Map b) { return ExprBaseType(b.image(a)); },
      [](LIB::Set a, LIB::PWMap b) { return ExprBaseType(b.image(a)); },
      [](auto a, auto b) { 
        Util::ERROR("imageEvaluator: wrong arguments ", a, ", ", b
          , " for image\n"); 
        return ExprBaseType();
      }
    };
    return std::visit(image2_evaluator, args[0], args[1]);
  }

  Util::ERROR_UNLESS("imageEvaluator: wrong number of arguments\n");

  return ExprBaseType();
}
 
ExprBaseType BuiltInFunctions::preImageEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "preImageEvaluator: wrong number of arguments\n");

  const auto pre_image_evaluator = Overload {
    [](LIB::Set a, LIB::Map b) { return ExprBaseType(b.preImage(a)); },
    [](LIB::Set a, LIB::PWMap b) { return ExprBaseType(b.preImage(a)); },
    [](auto a, auto b) { 
      Util::ERROR("preImageEvaluator: wrong arguments ", a, ", ", b
        , " for pre-image2\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(pre_image_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::domEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "domEvaluator: wrong number of arguments\n");

  const auto dom_evaluator = Overload {
    [](LIB::PWMap a) { return ExprBaseType(a.dom()); },
    [](auto a) {
      Util::ERROR("domEvaluator: wrong arguments ", a, "for dom\n");
      return ExprBaseType();
    }
  };
  return std::visit(dom_evaluator, args[0]);
}
 
ExprBaseType BuiltInFunctions::combineEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "combineEvaluator: wrong number of arguments\n");

  const auto combine_evaluator = Overload {
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.combine(b)); },
    [](auto a, auto b) { 
      Util::ERROR("combineEvaluator: wrong arguments ", a, ", ", b
        , " for combine\n"); 
      return ExprBaseType(); 
    }
  };
  return std::visit(combine_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::firstInvEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "firstInvEvaluator: wrong number of arguments\n");

  const auto first_inv_evaluator = Overload {
    [](LIB::PWMap a) { return ExprBaseType(a.firstInv()); },
    [](auto a) {
      Util::ERROR("firstInvEvaluator: wrong argument ", a, " for firstInv\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(first_inv_evaluator, args[0]);
}
 
ExprBaseType BuiltInFunctions::minMapEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "minMapEvaluator: wrong number of arguments\n");

  const auto min_map_evaluator = Overload {
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.minMap(b)); },
    [](auto a, auto b) {
      Util::ERROR("minMapEvaluator: wrong arguments ", a, ", ", b
        , " for minMap\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(min_map_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::reduceEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "reduceEvaluator: wrong number of arguments\n");

  const auto reduce_evaluator = Overload {
    [](LIB::PWMap a) { return ExprBaseType(a.reduce()); },
    [](auto a) {
      Util::ERROR("reduceEvaluator: wrong argument ", a, " for reduce\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(reduce_evaluator, args[0]);
}
  
ExprBaseType BuiltInFunctions::minAdjEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "minAdjEvaluator: wrong number of arguments\n");

  const auto min_adj_evaluator = Overload {
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.minAdjMap(b)); },
    [](auto a, auto b) { 
      Util::ERROR("minAdjEvaluator: wrong arguments ", a, ", ", b
        , " for minAdj\n"); 
      return ExprBaseType(); 
    }
  };
  return std::visit(min_adj_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::mapInfEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "mapInfEvaluator: wrong number of arguments\n");

  const auto inf_evaluator = Overload {
    [](LIB::PWMap a) { return ExprBaseType(a.mapInf()); },
    [](auto a) { 
      Util::ERROR("mapInfEvaluator: wrong argument ", a, " for mapInf\n"); 
      return ExprBaseType(); 
    }
  };
  return std::visit(inf_evaluator, args[0]);
}
  
// Algorithms evaluators ------------------------------------------------------

ExprBaseType BuiltInFunctions::connectedEvaluator(const EBTList& args)
{ 
  Util::ERROR_UNLESS(args.size() == 1
    , "connectedEvaluator: wrong number of arguments\n");

  const auto connected_evaluator = Overload {
    [](LIB::SBG a) { return ExprBaseType(connectedComponents(a)); },
    [](auto a) {
      Util::ERROR("connectedEvaluator: wrong argument ", a, " for CC\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(connected_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::matchingEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "matchingEvaluator: wrong number of arguments\n");

  LIB::Matching match_impl = LIB::MATCH_FACT.createMatchAlgorithm();
  const auto matching_evaluator = Overload {
    [&match_impl](LIB::SBG a, LIB::NAT b) { 
      return ExprBaseType(match_impl.calculate(a.copy(b)));
    },
    [&match_impl](LIB::SBG a, LIB::MD_NAT b) { 
      return ExprBaseType(match_impl.calculate(a.copy(b[0])));
    },
    [](auto a, auto b) {
      Util::ERROR("matchingEvaluator: wrong arguments ", a, ", ", b
        , " for matching\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(matching_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInFunctions::sccEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "sccEvaluator: wrong number of arguments\n");

  LIB::SCC scc_impl = LIB::SCC_FACT.createSCCAlgorithm();
  const auto scc_evaluator = Overload {
    [&scc_impl](LIB::DSBG a) { 
      return ExprBaseType(scc_impl.calculate(a).rmap());
    },
    [](auto a) {
      Util::ERROR("sccEvaluator: wrong argument ", a, " for scc\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(scc_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::topoSortEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "topoSortEvaluator: wrong number of arguments\n");

  LIB::TopoSort ts_impl = LIB::TS_FACT.createTSAlgorithm();
  const auto ts_evaluator = Overload {
    [&ts_impl](LIB::DSBG a) { 
      return ExprBaseType(ts_impl.calculate(a));
    },
    [](auto a) {
      Util::ERROR("topoSortEvaluator: wrong argument ", a, " for sort\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(ts_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::cutVertexEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "cutVertexEvaluator: wrong number of arguments\n");

  LIB::CutVertex cv_impl = LIB::CV_FACT.createCVAlgorithm();
  const auto cv_evaluator = Overload {
    [&cv_impl](LIB::DSBG a) { 
      return ExprBaseType(cv_impl.calculate(a));
    },
    [](auto a) {
      Util::ERROR("topoSortEvaluator: wrong argument ", a, " for sort\n"); 
      return ExprBaseType();
    }
  };
  return std::visit(cv_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::matchSCCEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "matchSCCEvaluator: wrong number of arguments");

  LIB::Matching match_impl = LIB::MATCH_FACT.createMatchAlgorithm();
  LIB::SCC scc_impl = LIB::SCC_FACT.createSCCAlgorithm();
  const auto match_scc_evaluator = Overload {
    [&match_impl, &scc_impl](LIB::SBG a, LIB::NAT b) { 
      LIB::MatchData match_result = match_impl.calculate(a.copy(b));
      LIB::DSBG dsbg = MISC::buildSCCFromMatching(match_result);
      return ExprBaseType(scc_impl.calculate(dsbg).rmap());
    },
    [&match_impl, &scc_impl](LIB::SBG a, LIB::MD_NAT b) { 
      LIB::MatchData match_result = match_impl.calculate(a.copy(b[0]));
      LIB::DSBG dsbg = MISC::buildSCCFromMatching(match_result);
      return ExprBaseType(scc_impl.calculate(dsbg).rmap());
    },
    [](auto a, auto b) {
      Util::ERROR("match_scc_evaluator: wrong arguments ", a, ", ", b
        , " for matchSCC\n"); 
      return ExprBaseType();
    }
  };

  return std::visit(match_scc_evaluator, args[0], args[1]);
}

/*
ExprBaseType BuiltInFunctions::matchSCCTSEvaluator(const EBTList& args)
{
  const auto match_scc_ts_evaluator = Overload {
    [](LIB::SBG a, LIB::NAT b, bool c) { 
      LIB::BFSMatching match(a.copy(b), c);
      LIB::Set match_res = match.calculate().matched_edges();
      LIB::SCC scc(MISC::buildSCCFromMatching(match), c);
      LIB::PWMap scc_res = scc.calculate();
      LIB::DSBG ts_dsbg = MISC::buildSortFromSCC(scc, scc_res);
      LIB::TopoSort ts = LIB::MinVertexTSAF().createTSAlgorithm(ts_dsbg);
      LIB::PWMap ts_res = ts.calculate(); 
      MISC::buildJson(match_res, scc_res, ts_res);
      return ExprBaseType(ts_res);
    },
    [](LIB::SBG a, LIB::MD_NAT b, bool c) { 
      LIB::BFSMatching match(a.copy(b[0]), c);
      LIB::Set match_res = match.calculate().matched_edges();
      LIB::SCC scc(MISC::buildSCCFromMatching(match), c);
      LIB::PWMap scc_res = scc.calculate();
      LIB::DSBG ts_dsbg = MISC::buildSortFromSCC(scc, scc_res);
      LIB::TopoSort ts = LIB::MinVertexTSAF().createTSAlgorithm(ts_dsbg);
      LIB::PWMap ts_res = ts.calculate(); 
      MISC::buildJson(match_res, scc_res, ts_res);
      return ExprBaseType(ts_res);
    },
    [](auto a, auto b, auto c) {
      Util::ERROR("match_scc_ts_evaluator: wrong arguments ", a, ", ", b
        , " for matchSCCTS\n"); 
      return ExprBaseType();
    }
  };
}
*/

} // namespace Eval

} // namespace SBG
