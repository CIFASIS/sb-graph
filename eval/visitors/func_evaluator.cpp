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
#include "algorithms/cutvertex/af_cv.hpp"
#include "algorithms/matching/bfs_matching.hpp"
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/af_ts.hpp"
#include "algorithms/misc/causalization_builders.hpp"
#include "algorithms/misc/causalization_json.hpp"
#include "eval/visitors/func_evaluator.hpp"

namespace SBG {

namespace Eval {

////////////////////////////////////////////////////////////////////////////////
// Function evaluators ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

auto& getOpposite()
{
  static const auto kOppositeEvaluator = Overload {
    [](LIB::NAT a) { return LIB::RATIONAL(a, -1); },
    [](LIB::RATIONAL a) { return LIB::RATIONAL(-1)*a; },
    [](auto a) { 
      Util::ERROR("oppo_evaluator: wrong argument ", a, " for - (opposite)\n");
      return LIB::RATIONAL(0);
    }
  };
  return kOppositeEvaluator;
}

auto& getCardinal()
{
  static const auto kCardinalEvaluator = Overload {
    [](LIB::Interval a) { return (LIB::NAT) a.cardinal(); },
    [](LIB::MultiDimInter a) { return (LIB::NAT) a.cardinal(); },
    [](LIB::Set a) { return (LIB::NAT) a.cardinal(); },
    [](auto a) { 
      Util::ERROR("empty_evaluator: wrong argument ", a, " for #\n");
      return (LIB::NAT) 0;
    }
  };
  return kCardinalEvaluator;
}

auto& getComplement()
{
  static const auto kComplementEvaluator = Overload {
    [](LIB::Set a) { return ExprBaseType(a.complement()); },
    [](auto a) { 
      Util::ERROR("empty_evaluator: wrong argument ", a
        , " for \' (complement)\n"); 
      return ExprBaseType();
    }
  };
  return kComplementEvaluator;
}

auto add_evaluator = Overload {
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
    Util::ERROR("add_evaluator: wrong arguments ", a, ", ", b
      , " for operator+\n"); 
    return ExprBaseType();
  }
};

auto sub_evaluator = Overload {
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
    Util::ERROR("sub_evaluator: wrong arguments ", a, ", ", b
      , " for operator-\n"); 
    return ExprBaseType();
  }
};

auto mult_evaluator = Overload {
  [](LIB::NAT a, LIB::NAT b) { return ExprBaseType(a*b); },
  [](LIB::RATIONAL a, LIB::RATIONAL b) { return ExprBaseType(a*b); },
  [](LIB::NAT a, LIB::RATIONAL b) { return ExprBaseType(LIB::RATIONAL(a)*b); },
  [](LIB::RATIONAL a, LIB::NAT b) { return ExprBaseType(a*LIB::RATIONAL(b)); },
  [](auto a, auto b) { 
    Util::ERROR("mult_evaluator: wrong arguments ", a, ", ", b
      , " for operator*\n"); 
    return ExprBaseType();
  }
};

auto eq_evaluator = Overload {
  [](LIB::MD_NAT a, LIB::MD_NAT b) { return a == b; },
  [](LIB::RATIONAL a, LIB::RATIONAL b) { return a == b; },
  [](LIB::Interval a, LIB::Interval b) { return a == b; },
  [](LIB::SetPiece a, LIB::SetPiece b) { return a == b; },
  [](LIB::Set a, LIB::Set b) { return a == b; },
  [](LIB::Exp a, LIB::Exp b) { return a == b; },
  [](LIB::Map a, LIB::Map b) { return a == b; },
  [](LIB::PWMap a, LIB::PWMap b) { return a == b; },
  [](auto a, auto b) {
    Util::ERROR("eq_evaluator: wrong arguments ", a, ", ", b
      , " for operator==\n");
    return false;
  }
};

auto less_evaluator = Overload {
  [](LIB::MD_NAT a, LIB::MD_NAT b) { return a < b; },
  [](LIB::RATIONAL a, LIB::RATIONAL b) { return a < b; },
  [](LIB::Interval a, LIB::Interval b) { return a < b; },
  [](LIB::SetPiece a, LIB::SetPiece b) { return a < b; },
  [](auto a, auto b) { 
    Util::ERROR("less_evaluator: wrong arguments ", a, ", ", b
      , " for operator<\n"); 
    return false;
  }
};

auto cap_evaluator = Overload{
  [](LIB::Interval a, LIB::Interval b) {
    return ExprBaseType(a.intersection(b));
  },
  [](LIB::SetPiece a, LIB::SetPiece b) {
    return ExprBaseType(a.intersection(b));
  },
  [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.intersection(b)); },
  [](auto a, auto b) { 
    Util::ERROR("cap_evaluator: wrong arguments ", a, ", ", b
      , " for intersection\n"); 
    return ExprBaseType();
  }
};

auto cup_evaluator = Overload{
  [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.cup(b)); },
  [](auto a, auto b) { 
    Util::ERROR("cap_evaluator: wrong arguments ", a, ", ", b
      , " for union\n"); 
    return ExprBaseType();
  }
};

auto diff_evaluator = Overload{
  [](LIB::Set a, LIB::Set b) { return ExprBaseType(a.difference(b)); },
  [](auto a, auto b) { 
    Util::ERROR("diff_evaluator: wrong arguments ", a, ", ", b
      , " for difference\n"); 
    return ExprBaseType();
  }
};

auto empty_evaluator = Overload {
  [](LIB::Interval a) { return a.isEmpty(); },
  [](LIB::MultiDimInter a) { return a.isEmpty(); },
  [](LIB::Set a) { return a.isEmpty(); },
  [](auto a) { 
    Util::ERROR("empty_evaluator: wrong argument ", a, " for isEmpty\n"); 
    return false;
  }
};

auto min_evaluator = Overload {
  [](LIB::Interval a) { return LIB::MD_NAT(a.begin()); },
  [](LIB::MultiDimInter a) { return a.minElem(); },
  [](LIB::Set a) { return a.minElem(); },
  [](auto a) { 
    Util::ERROR("min_evaluator: wrong argument ", a, " for minElem\n"); 
    return LIB::MD_NAT();
  }
};

auto max_evaluator = Overload {
  [](LIB::Interval a) { return LIB::MD_NAT(a.end()); },
  [](LIB::MultiDimInter a) { return a.maxElem(); },
  [](LIB::Set a) { return a.maxElem(); },
  [](auto a) { 
    Util::ERROR("max_evaluator: wrong argument ", a, " for maxElem\n"); 
    return LIB::MD_NAT(); 
  }
};

auto compose_evaluator = Overload {
  [](LIB::LExp a, LIB::LExp b) { return ExprBaseType(a.composition(b)); },
  [](LIB::Exp a, LIB::Exp b) { return ExprBaseType(a.composition(b)); },
  [](LIB::Map a, LIB::Map b) { return ExprBaseType(a.composition(b)); },
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.composition(b)); },
  [](auto a, auto b) {
    Util::ERROR("compose_evaluator: wrong arguments ", a, ", ", b
      , " for compose\n"); 
    return ExprBaseType(); 
   }
};

auto inverse_evaluator = Overload {
  [](LIB::LExp a) { return ExprBaseType(a.inverse()); },
  [](LIB::Exp a) { return ExprBaseType(a.inverse()); },
  [](LIB::Map a) { return ExprBaseType(a.minInv()); },
  [](LIB::PWMap a) { return ExprBaseType(a.inverse()); },
  [](auto a) { 
    Util::ERROR("inverse_evaluator: wrong arguments ", a, " for inverse\n"); 
    return ExprBaseType();
  }
};

auto image_evaluator = Overload {
  [](LIB::Map a) { return ExprBaseType(a.image()); },
  [](LIB::PWMap a) { return ExprBaseType(a.image()); },
  [](auto a) { 
    Util::ERROR("image_evaluator: wrong argument ", a, " for image\n"); 
    return ExprBaseType();
  }
};

auto image2_evaluator = Overload {
  [](LIB::Set a, LIB::Map b) { return ExprBaseType(b.image(a)); },
  [](LIB::Set a, LIB::PWMap b) { return ExprBaseType(b.image(a)); },
  [](auto a, auto b) { 
    Util::ERROR("image2_evaluator: wrong arguments ", a, ", ", b
      , " for image2\n"); 
    return ExprBaseType();
  }
};

auto pre_image2_evaluator = Overload {
  [](LIB::Set a, LIB::Map b) { return ExprBaseType(b.preImage(a)); },
  [](LIB::Set a, LIB::PWMap b) { return ExprBaseType(b.preImage(a)); },
  [](auto a, auto b) { 
    Util::ERROR("pre_image2_evaluator: wrong arguments ", a, ", ", b
      , " for pre-image2\n"); 
    return ExprBaseType(); 
  }
};

auto dom_evaluator = Overload {
  [](LIB::PWMap a) { return ExprBaseType(a.dom()); },
  [](auto a) {
    Util::ERROR("dom_evaluator: wrong arguments ", a, "for dom\n");
    return ExprBaseType();
  }
};

auto combine_evaluator = Overload {
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.combine(b)); },
  [](auto a, auto b) { 
    Util::ERROR("combine_evaluator: wrong arguments ", a, ", ", b
      , " for combine\n"); 
    return ExprBaseType(); 
  }
};

auto first_inv_evaluator = Overload {
  [](LIB::PWMap a) { return ExprBaseType(a.firstInv()); },
  [](auto a) {
    Util::ERROR("first_inv_evaluator: wrong argument ", a, " for firstInv\n"); 
    return ExprBaseType();
  }
};

auto min_map_evaluator = Overload {
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.minMap(b)); },
  [](auto a, auto b) {
    Util::ERROR("min_map_evaluator: wrong arguments ", a, ", ", b
      , " for minMap\n"); 
    return ExprBaseType();
  }
};

auto reduce_evaluator = Overload {
  [](LIB::PWMap a) { return ExprBaseType(a.reduce()); },
  [](auto a) {
    Util::ERROR("reduce_evaluator: wrong argument ", a, " for reduce\n"); 
    return ExprBaseType();
  }
};

auto min_adj_evaluator = Overload {
  [](LIB::PWMap a, LIB::PWMap b) { return ExprBaseType(a.minAdjMap(b)); },
  [](auto a, auto b) { 
    Util::ERROR("min_adj_evaluator: wrong arguments ", a, ", ", b
      , " for minAdj\n"); 
    return ExprBaseType(); 
  }
};

auto inf_evaluator = Overload {
  [](LIB::PWMap a) { return ExprBaseType(a.mapInf()); },
  [](auto a) { 
    Util::ERROR("inf_evaluator: wrong argument ", a, " for mapInf\n"); 
    return ExprBaseType(); 
  }
};

// Algorithms evaluators ------------------------------------------------------

auto connected_evaluator = Overload {
  [](LIB::SBG a) { return ExprBaseType(connectedComponents(a)); },
  [](auto a) {
    Util::ERROR("connected_evaluator: wrong argument ", a, " for CC\n"); 
    return ExprBaseType();
  }
};

auto ts_evaluator = Overload {
  [](LIB::DSBG a) { 
    LIB::TopoSort ts = LIB::MinVertexTSAF().createTSAlgorithm(a.fact());
    return ExprBaseType(ts.calculate(a));
  },
  [](auto a) {
    Util::ERROR("ts_evaluator: wrong argument ", a, " for sort\n"); 
    return ExprBaseType();
  }
};

/*
auto match_scc_evaluator = Overload {
  [](LIB::SBG a, LIB::NAT b, LIB::SCC c, bool d) { 
    LIB::BFSMatching match(a.copy(b), d);
    match.calculate();
    LIB::DSBG dsbg = MISC::buildSCCFromMatching(match);
    return ExprBaseType(c.calculate(dsbg).rmap());
  },
  [](LIB::SBG a, LIB::MD_NAT b, LIB::SCC c, bool d) { 
    LIB::BFSMatching match(a.copy(b[0]), d);
    match.calculate();
    LIB::DSBG dsbg = MISC::buildSCCFromMatching(match);
    return ExprBaseType(c.calculate(dsbg).rmap());
  },
  [](auto a, auto b, auto c, auto d) {
    Util::ERROR("match_scc_evaluator: wrong arguments ", a, ", ", b
      , " for matchSCC\n"); 
    return ExprBaseType();
  }
};

auto match_scc_ts_evaluator = Overload {
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
*/

auto cut_evaluator = Overload {
  [](LIB::DSBG a) { 
    LIB::MinReachSCCFact scc_fact;
    LIB::CutVertex cv = LIB::MaxDegCVAF().createCVAlgorithm(a.fact(), scc_fact);
    return ExprBaseType(cv.calculate(a));
  },
  [](auto a) {
    Util::ERROR("cut_evaluator: wrong argument ", a, " for cut\n"); 
    return ExprBaseType();
  }
};

} // namespace Eval

} // namespace SBG
