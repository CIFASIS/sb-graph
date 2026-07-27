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

#include "eval/visitors/func_evaluator.hpp"
#include "algorithms/cc/cc.hpp"
#include "algorithms/matching/matching.hpp"
#include "algorithms/mfvs/min_feedback_vertex_set.hpp"
#include "algorithms/misc/causalization_builders.hpp"
#include "algorithms/misc/causalization_json.hpp"
#include "algorithms/scc/scc.hpp"
#include "algorithms/sorting/topological/topological_sorting.hpp"
#include "eval/base_type.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "sbg/expression.hpp"
#include "sbg/interval.hpp"
#include "sbg/map.hpp"
#include "sbg/multidim_inter.hpp"
#include "sbg/natural.hpp"
#include "sbg/rational.hpp"
#include "sbg/set.hpp"
#include "sbg/pw_map.hpp"
#include "util/debug.hpp"
#include "util/defs.hpp"

namespace SBG {

namespace Eval {

namespace detail {

////////////////////////////////////////////////////////////////////////////////
// Built-in operators evaluators -----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

ExprBaseType BuiltInOperators::oppositeEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "oppositeEvaluator: wrong number of arguments\n");

  auto opposite_evaluator = Util::Overload {
    [](LIB::NAT a)
    {
      return ExprBaseType{LIB::RATIONAL{static_cast<LIB::INT>(a), -1}};
    },
    [](LIB::RATIONAL a) { return ExprBaseType{LIB::RATIONAL{-1}*a}; },
    [](LIB::Set a) { return ExprBaseType{a.complement()}; },
    [](auto a) { 
      Util::ERROR("oppositeEvaluator: wrong type argument ", a
        , " for - (opposite)\n");
      return ExprBaseType{LIB::RATIONAL{0}};
    }
  };
  return std::visit(opposite_evaluator, args[0]);
}

ExprBaseType BuiltInOperators::cardinalEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "cardinalEvaluator: wrong number of arguments\n");

  const auto cardinal_evaluator = Util::Overload {
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

  const auto complement_evaluator = Util::Overload {
    [](LIB::Set a) { return ExprBaseType{a.complement()}; },
    [](auto a) { 
      Util::ERROR("complementEvaluator: wrong argument ", a
        , " for \' (complement)\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(complement_evaluator, args[0]);
}

ExprBaseType BuiltInOperators::addEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "addEvaluator: wrong number of arguments\n");

  const auto add_evaluator = Util::Overload {
    [](LIB::NAT a, LIB::NAT b) { return ExprBaseType{a + b}; },
    [](LIB::MD_NAT a, LIB::MD_NAT b) { return ExprBaseType{a + b}; },
    [](LIB::RATIONAL a, LIB::RATIONAL b) { return ExprBaseType{a + b}; },
    [](LIB::NAT a, LIB::RATIONAL b) {
      return ExprBaseType{LIB::RATIONAL{static_cast<LIB::INT>(a)} + b};
    },
    [](LIB::RATIONAL a, LIB::NAT b) {
      return ExprBaseType{a + LIB::RATIONAL{static_cast<LIB::INT>(b)}};
    },
    [](LIB::Expression a, LIB::Expression b) { return ExprBaseType{a + b}; },
    [](LIB::Map a, LIB::Map b) { return ExprBaseType{a + b}; },
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType{a + b}; },
    [](auto a, auto b) { 
      Util::ERROR("addEvaluator: wrong arguments ", a, ", ", b
        , " for operator+\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(add_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::subEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "subEvaluator: wrong number of arguments\n");
  
  const auto sub_evaluator = Util::Overload {
    [](LIB::NAT a, LIB::NAT b) {
      if (a > b) {
        return ExprBaseType{LIB::NAT{a - b}};
      } else {
        return ExprBaseType{LIB::RATIONAL{static_cast<LIB::INT>(a)}
          - LIB::RATIONAL{static_cast<LIB::INT>(b)}};
      }
    },
    [](LIB::RATIONAL a, LIB::RATIONAL b) { return ExprBaseType{a - b}; },
    [](LIB::NAT a, LIB::RATIONAL b) {
      return ExprBaseType{LIB::RATIONAL{static_cast<LIB::INT>(a)} - b};
    },
    [](LIB::RATIONAL a, LIB::NAT b) {
      return ExprBaseType{a - LIB::RATIONAL{static_cast<LIB::INT>(b)}};
    },
    [](LIB::Expression a, LIB::Expression b) { return ExprBaseType{a - b}; },
    [](auto a, auto b) { 
      Util::ERROR("subEvaluator: wrong arguments ", a, ", ", b
        , " for operator-\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(sub_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::multEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "multEvaluator: wrong number of arguments\n");

  const auto mult_evaluator = Util::Overload {
    [](LIB::NAT a, LIB::NAT b)
    {
      return ExprBaseType{LIB::RATIONAL{static_cast<LIB::INT>(a*b)}};
    },
    [](LIB::RATIONAL a, LIB::RATIONAL b) { return ExprBaseType{a*b}; },
    [](LIB::NAT a, LIB::RATIONAL b)
    {
      return ExprBaseType{LIB::RATIONAL{static_cast<LIB::INT>(a)}*b};
    },
    [](LIB::RATIONAL a, LIB::NAT b)
    {
      return ExprBaseType{a*LIB::RATIONAL{static_cast<LIB::INT>(b)}};
    },
    [](auto a, auto b) { 
      Util::ERROR("multEvaluator: wrong arguments ", a, ", ", b
        , " for operator*\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(mult_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::eqEvaluator(const EBTList& args)
{  
  Util::ERROR_UNLESS(args.size() == 2
    , "eqEvaluator: wrong number of arguments\n");

  const auto eq_evaluator = Util::Overload {
    [](LIB::MD_NAT a, LIB::MD_NAT b) { return a == b; },
    [](LIB::RATIONAL a, LIB::RATIONAL b) { return a == b; },
    [](LIB::Set a, LIB::Set b) { return a == b; },
    [](LIB::Expression a, LIB::Expression b) { return a == b; },
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

  const auto less_evaluator = Util::Overload {
    [](LIB::MD_NAT a, LIB::MD_NAT b) { return a < b; },
    [](LIB::RATIONAL a, LIB::RATIONAL b) { return a < b; },
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

  const auto cap_evaluator = Util::Overload{
    [](LIB::Set a, LIB::Set b) { return ExprBaseType{a.intersection(b)}; },
    [](auto a, auto b) { 
      Util::ERROR("capEvaluator: wrong arguments ", a, ", ", b
        , " for intersection\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(cap_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::cupEvaluator(const EBTList& args)
{ 
  Util::ERROR_UNLESS(args.size() == 2
    , "cupEvaluator: wrong number of arguments\n");

  const auto cup_evaluator = Util::Overload{
    [](LIB::Set a, LIB::Set b) { return ExprBaseType{a.cup(b)}; },
    [](auto a, auto b) {
      Util::ERROR("cupEvaluator: wrong arguments ", a, ", ", b
        , " for union\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(cup_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInOperators::diffEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "diffEvaluator: wrong number of arguments\n");

  const auto diff_evaluator = Util::Overload{
    [](LIB::Set a, LIB::Set b) { return ExprBaseType{a.difference(b)}; },
    [](auto a, auto b) {
      Util::ERROR("diffEvaluator: wrong arguments ", a, ", ", b
        , " for difference\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(diff_evaluator, args[0], args[1]);
}

// Operators evaluators --------------------------------------------------------

UnaryOpEvaluator::UnaryOpEvaluator() {}

ExprBaseType UnaryOpEvaluator::evaluate(EBTList& evaluated_args, AST::UnOp op)
{
  switch (op) {
    case AST::UnOp::oppo: {
      return BuiltInOperators::oppositeEvaluator(evaluated_args);
      break;
    }

    case AST::UnOp::card: {
      return BuiltInOperators::cardinalEvaluator(evaluated_args);
      break;
    }

    case AST::UnOp::comp: {
      return BuiltInOperators::complementEvaluator(evaluated_args);
      break;
    }

    default:
      Util::ERROR("UnaryOpEvaluator: UnaryOp ", op, " unsupported\n");
      return ExprBaseType{};
  }

  return ExprBaseType{}; 
}

BinOpEvaluator::BinOpEvaluator() {}

ExprBaseType BinOpEvaluator::evaluate(EBTList& evaluated_args, AST::Op op)
{
  switch (op) {
    case AST::Op::add: {
      return BuiltInOperators::addEvaluator(evaluated_args);
      break;
    }

    case AST::Op::sub: {
      return BuiltInOperators::subEvaluator(evaluated_args);
      break;
    }

    case AST::Op::mult: {
      return BuiltInOperators::multEvaluator(evaluated_args);
      break;
    }

    case AST::Op::eq: {
      return BuiltInOperators::eqEvaluator(evaluated_args);
      break;
    }

    case AST::Op::less: {
      return BuiltInOperators::lessEvaluator(evaluated_args);
      break;
    }

    case AST::Op::cap: {
      return BuiltInOperators::capEvaluator(evaluated_args);
      break;
    }

    case AST::Op::cup: {
      return BuiltInOperators::cupEvaluator(evaluated_args);
      break;
    }

    case AST::Op::diff: {
      return BuiltInOperators::diffEvaluator(evaluated_args);
      break;
    }

    default: {
      Util::ERROR("BinOpEvaluator: BinOp ", op, " unsupported\n");
      return ExprBaseType{}; 
    }
  } 

  return ExprBaseType{}; 
}

////////////////////////////////////////////////////////////////////////////////
// Built-in functions evaluators -----------------------------------------------
////////////////////////////////////////////////////////////////////////////////
  
ExprBaseType BuiltInFunctions::emptyEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1 
    , "emptyEvaluator: wrong number of arguments\n");

  const auto empty_evaluator = Util::Overload {
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

  const auto min_evaluator = Util::Overload {
    [](LIB::Set a) { return a.minElem(); },
    [](auto a) { 
      Util::ERROR("minEvaluator: wrong argument ", a, " for minElem\n"); 
      return LIB::MD_NAT{};
    }
  };
  return std::visit(min_evaluator, args[0]);
}
 
ExprBaseType BuiltInFunctions::maxEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "maxEvaluator: wrong number of arguments\n");

  const auto max_evaluator = Util::Overload {
    [](LIB::Set a) { return a.maxElem(); },
    [](auto a) { 
      Util::ERROR("maxEvaluator: wrong argument ", a, " for maxElem\n"); 
      return LIB::MD_NAT{}; 
    }
  };
  return std::visit(max_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::restrictEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "restrictEvaluator: wrong number of arguments\n");

  const auto restrict_evaluator = Util::Overload {
    [](LIB::PWMap a, LIB::Set b) { return ExprBaseType{a.restrict(b)}; },
    [](auto a, auto b) {
      Util::ERROR("restrictEvaluator: wrong arguments ", a, ", ", b
        , " for restrict\n"); 
      return ExprBaseType{}; 
     }
  };
  return std::visit(restrict_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::composeEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "composeEvaluator: wrong number of arguments\n");

  const auto compose_evaluator = Util::Overload {
    [](LIB::Expression a, LIB::Expression b)
    {
      return ExprBaseType{a.composition(b)};
    },
    [](LIB::Map a, LIB::Map b) { return ExprBaseType{a.composition(b)}; },
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType{a.composition(b)}; },
    [](auto a, auto b) {
      Util::ERROR("composeEvaluator: wrong arguments ", a, ", ", b
        , " for compose\n"); 
      return ExprBaseType{}; 
     }
  };
  return std::visit(compose_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::inverseEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "inverseEvaluator: wrong number of arguments\n");

  const auto inverse_evaluator = Util::Overload {
    [](LIB::Expression a) { return ExprBaseType{a.inverse()}; },
    [](LIB::PWMap a) { return ExprBaseType{a.inverse()}; },
    [](auto a) { 
      Util::ERROR("inverseEvaluator: wrong arguments ", a, " for inverse\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(inverse_evaluator, args[0]);
}
 
ExprBaseType BuiltInFunctions::imageEvaluator(const EBTList& args)
{
  if (args.size() == 1) {
    const auto image_evaluator = Util::Overload {
      [](LIB::Map a) { return ExprBaseType{a.image()}; },
      [](LIB::PWMap a) { return ExprBaseType{a.image()}; },
      [](auto a) { 
        Util::ERROR("imageEvaluator: wrong argument ", a, " for image\n"); 
        return ExprBaseType{};
      }
    };
    return std::visit(image_evaluator, args[0]);
  }
  else if (args.size() == 2) {
    const auto image2_evaluator = Util::Overload {
      [](LIB::Set a, LIB::Map b) { return ExprBaseType{b.image(a)}; },
      [](LIB::Set a, LIB::PWMap b) { return ExprBaseType{b.image(a)}; },
      [](auto a, auto b) { 
        Util::ERROR("imageEvaluator: wrong arguments ", a, ", ", b
          , " for image\n"); 
        return ExprBaseType{};
      }
    };
    return std::visit(image2_evaluator, args[0], args[1]);
  }

  Util::ERROR_UNLESS("imageEvaluator: wrong number of arguments\n");

  return ExprBaseType{};
}
 
ExprBaseType BuiltInFunctions::preImageEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "preImageEvaluator: wrong number of arguments\n");

  const auto pre_image_evaluator = Util::Overload {
    [](LIB::Set a, LIB::Map b) { return ExprBaseType{b.preImage(a)}; },
    [](LIB::Set a, LIB::PWMap b) { return ExprBaseType{b.preImage(a)}; },
    [](auto a, auto b) { 
      Util::ERROR("preImageEvaluator: wrong arguments ", a, ", ", b
        , " for pre-image2\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(pre_image_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::domEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "domEvaluator: wrong number of arguments\n");

  const auto dom_evaluator = Util::Overload {
    [](LIB::PWMap a) { return ExprBaseType{a.domain()}; },
    [](auto a) {
      Util::ERROR("domEvaluator: wrong arguments ", a, "for dom\n");
      return ExprBaseType{};
    }
  };
  return std::visit(dom_evaluator, args[0]);
}
 
ExprBaseType BuiltInFunctions::combineEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "combineEvaluator: wrong number of arguments\n");

  const auto combine_evaluator = Util::Overload {
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType{a.combine(b)}; },
    [](auto a, auto b) { 
      Util::ERROR("combineEvaluator: wrong arguments ", a, ", ", b
        , " for combine\n"); 
      return ExprBaseType{}; 
    }
  };
  return std::visit(combine_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::minMapEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "minMapEvaluator: wrong number of arguments\n");

  const auto min_map_evaluator = Util::Overload {
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType{a.min(b)}; },
    [](auto a, auto b) {
      Util::ERROR("minMapEvaluator: wrong arguments ", a, ", ", b
        , " for minMap\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(min_map_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInFunctions::minAdjEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "minAdjEvaluator: wrong number of arguments\n");

  const auto min_adj_evaluator = Util::Overload {
    [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType{a.minAdj(b)}; },
    [](auto a, auto b) { 
      Util::ERROR("minAdjEvaluator: wrong arguments ", a, ", ", b
        , " for minAdj\n"); 
      return ExprBaseType{}; 
    }
  };
  return std::visit(min_adj_evaluator, args[0], args[1]);
}
 
ExprBaseType BuiltInFunctions::mapInfEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "mapInfEvaluator: wrong number of arguments\n");

  const auto inf_evaluator = Util::Overload {
    [](LIB::PWMap a) { return ExprBaseType{a.mapInf()}; },
    [](auto a) { 
      Util::ERROR("mapInfEvaluator: wrong argument ", a, " for mapInf\n"); 
      return ExprBaseType{}; 
    }
  };
  return std::visit(inf_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::imageMultEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "imageMultEvaluator: wrong number of arguments\n");

  const auto img_mult_evaluator = Util::Overload {
    [](LIB::PWMap a) { return ExprBaseType{a.imageMultiplicity()}; },
    [](auto a) { 
      Util::ERROR("imageMultEvaluator: wrong argument ", a
        , " for imageMultiplicity\n"); 
      return ExprBaseType{}; 
    }
  };
  return std::visit(img_mult_evaluator, args[0]);
}
  
// Algorithms evaluators ------------------------------------------------------

ExprBaseType BuiltInFunctions::connectedEvaluator(const EBTList& args)
{ 
  Util::ERROR_UNLESS(args.size() == 1
    , "connectedEvaluator: wrong number of arguments\n");

  const auto connected_evaluator = Util::Overload {
    [](LIB::SBG a) { return ExprBaseType{connectedComponents(a)}; },
    [](auto a) {
      Util::ERROR("connectedEvaluator: wrong argument ", a, " for CC\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(connected_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::matchingEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "matchingEvaluator: wrong number of arguments\n");

  LIB::Matching match_impl;
  const auto matching_evaluator = Util::Overload {
    [&match_impl](LIB::BipartiteSBG a, LIB::NAT b) { 
      return ExprBaseType{match_impl.calculate(copy(b, a))};
    },
    [&match_impl](LIB::BipartiteSBG a, LIB::MD_NAT b) { 
      return ExprBaseType{match_impl.calculate(copy(b[0], a))};
    },
    [](auto a, auto b) {
      Util::ERROR("matchingEvaluator: wrong arguments ", a, ", ", b
        , " for matching\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(matching_evaluator, args[0], args[1]);
}

ExprBaseType BuiltInFunctions::sccEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "sccEvaluator: wrong number of arguments\n");

  LIB::SCC scc_impl;
  const auto scc_evaluator = Util::Overload {
    [&scc_impl](LIB::DirectedSBG a) { 
      return ExprBaseType{scc_impl.calculate(a).rmap()};
    },
    [](auto a) {
      Util::ERROR("sccEvaluator: wrong argument ", a, " for scc\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(scc_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::matchSCCEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "matchSCCEvaluator: wrong number of arguments");

  const ExprBaseType match_base_type = matchingEvaluator(args);

  const LIB::MatchData match_result = std::get<LIB::MatchData>(match_base_type);
  LIB::DirectedSBG dsbg = misc::buildLoopDetectionSBG(match_result);
  LIB::SCC scc_impl;
  LIB::SCCData scc_result = scc_impl.calculate(dsbg);

  return ExprBaseType{scc_result.rmap()};
}

ExprBaseType BuiltInFunctions::mfvsEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "mfvsEvaluator: wrong number of arguments\n");

  LIB::MinFeedbackVertexSet mfvs_impl;
  const auto mfvs_evaluator = Util::Overload {
    [&mfvs_impl](LIB::DirectedSBG a) { 
      return ExprBaseType{mfvs_impl.calculate(a)};
    },
    [](auto a) {
      Util::ERROR("mfvsEvaluator: wrong type of argument ", a
        , " for MFVS algorithm\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(mfvs_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::matchSCCMFVSEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "matchSCCMFVSEvaluator: wrong number of arguments");

  const ExprBaseType match_base_type = matchingEvaluator(args);

  const LIB::MatchData match_result = std::get<LIB::MatchData>(match_base_type);
  LIB::DirectedSBG dsbg = misc::buildLoopDetectionSBG(match_result);
  LIB::SCC scc_impl;
  LIB::SCCData scc_result = scc_impl.calculate(dsbg);

  dsbg = misc::buildTearingSBG(scc_result);
  LIB::MinFeedbackVertexSet mfvs_impl;
  return ExprBaseType{mfvs_impl.calculate(dsbg)};
}

ExprBaseType BuiltInFunctions::topoSortEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 1
    , "topoSortEvaluator: wrong number of arguments\n");

  LIB::TopologicalSorting ts_impl;
  const auto ts_evaluator = Util::Overload {
    [&ts_impl](LIB::DirectedSBG a) { 
      return ExprBaseType{ts_impl.calculate(a, SBG::LIB::PWMap{})};
    },
    [](auto a) {
      Util::ERROR("topoSortEvaluator: wrong argument ", a, " for sort\n"); 
      return ExprBaseType{};
    }
  };
  return std::visit(ts_evaluator, args[0]);
}

ExprBaseType BuiltInFunctions::causalizationEvaluator(const EBTList& args)
{
  Util::ERROR_UNLESS(args.size() == 2
    , "causalizationEvaluator: wrong number of arguments");

  const ExprBaseType match_base_type = matchingEvaluator(args);

  const LIB::MatchData match_result = std::get<LIB::MatchData>(match_base_type);
  LIB::DirectedSBG dsbg = misc::buildLoopDetectionSBG(match_result);
  LIB::SCC scc_impl;
  LIB::SCCData scc_result = scc_impl.calculate(dsbg);

  dsbg = misc::buildTearingSBG(scc_result);
  LIB::MinFeedbackVertexSet mfvs_impl;
  LIB::Set mfvs_result = mfvs_impl.calculate(dsbg);

  SBG::LIB::SCCData vs_builder_result = misc::buildVerticalSortingSBG(scc_result
    , mfvs_result);
  dsbg = vs_builder_result.dsbg();
  LIB::TopologicalSorting ts_impl;
  LIB::PWMap ts_result = ts_impl.calculate(dsbg, vs_builder_result.rmap());

  misc::CausalizationResult causalized{match_result.M(), scc_result.rmap()
    , mfvs_result, ts_result};
  misc::toJSON(causalized);

  return ExprBaseType{ts_result};
}

} // namespace detail

} // namespace Eval

} // namespace SBG
