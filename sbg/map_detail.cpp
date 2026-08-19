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

#include "sbg/map_detail.hpp"
#include "sbg/natural.hpp"
#include "sbg/ord_unidim_dense_set.hpp"
#include "sbg/rational.hpp"
#include "sbg/set_detail.hpp"
#include "sbg/set_impl.hpp"
#include "sbg/unord_set.hpp"
#include "util/debug.hpp"

#include <tuple>
#include <utility>

namespace SBG {

namespace LIB {

namespace detail {

// Image -----------------------------------------------------------------------

Interval image(const Interval& i, const LinearExpr& linear_expr)
{
  if (i.isEmpty()) {
    return Interval{};
  }

  if (linear_expr.isId()) {
    return i;
  }

  if (linear_expr.isConstant()) {
    NAT value = linear_expr.apply(i.begin());
    return Interval{value, 1, value};
  }

  NAT new_begin = linear_expr.apply(i.begin());
  RATIONAL m = linear_expr.slope();
  NAT step = i.step();
  NAT new_step = m >= 0 ? (m*step).toNat() : (-m*step).toNat();
  NAT new_end = linear_expr.apply(i.end());

  return Interval{new_begin, new_step, new_end};
}

MultiDimInter image(const MultiDimInter& mdi, const ExpressionImpl& expr)
{
  if (mdi.isEmpty()) {
    return MultiDimInter{};
  }

  MultiDimInter result;
  for (unsigned int k = 0; k < mdi.arity(); ++k) {
    result.pushBack(image(mdi[k], expr[k]));
  }
  return result;
}

template<typename CompactSetImplT, typename PieceT, typename ExprImplT>
CompactSetImplT compactImage(const CompactSetImplT& s, const ExprImplT& expr
  , bool is_injective)
{
  CompactSetImplT result;

  if (is_injective) {
    for (const PieceT& p : s) {
      result.pushBack(detail::image(p, expr));
    }
  } else {
    for (const PieceT& p : s) {
      CompactSetImplT jth_image{detail::image(p, expr)};
      result = std::move(result).cup(std::move(jth_image));
    }
  }

  return result;
}

Set MapDetail::image(const Set& s, const Expression& expr)
{
  bool is_injective = expr.isInjective();
  SetAccessKey key = SetAccess::key();
  auto image_evaluator = Util::Overload {
    [&](const UnorderedSet& a)
    {
      return key.createSet(detail::compactImage<UnorderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl, is_injective));
    },
    [&](const OrdUnidimDenseSet& a)
    {
      return key.createSet(detail::compactImage<OrdUnidimDenseSet, Interval
        , LinearExpr>(a, expr._impl[0], is_injective));
    },
    [&](const OrderedSet& a)
    {
      return key.createSet(detail::compactImage<OrderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl, is_injective));
    },
    [&](const auto& a)
    {
      Util::ERROR("MapDetail::image: unsupported Set implementation\n");
      return Set{SetKind::kUnordered};
    }
  };
  return std::visit(image_evaluator, key.impl(s));
}

// Pre-image -------------------------------------------------------------------

Interval preImage(const Interval& i, const LinearExpr& linear_expr)
{
  if (i.isEmpty()) {
    return Interval{};
  }

  if (linear_expr.isConstant()) {
    return Interval{0, 1, Inf};
  }

  return image(i, linear_expr.inverse());
}

MultiDimInter preImage(const MultiDimInter& mdi, const ExpressionImpl& expr)
{
  if (mdi.isEmpty()) {
    return MultiDimInter{};
  }

  MultiDimInter result;
  for (unsigned int k = 0; k < mdi.arity(); ++k) {
    result.pushBack(preImage(mdi[k], expr[k]));
  }
  return result;
}

template<typename CompactSetImplT, typename PieceT, typename ExprImplT>
CompactSetImplT compactPreImage(const CompactSetImplT& s, const ExprImplT& expr)
{
  CompactSetImplT result;

  for (const PieceT& p : s) {
    result.pushBack(detail::preImage(p, expr));
  }

  return result;
}

Set MapDetail::preImage(const Set& s, const Expression& expr)
{
  SetAccessKey key = SetAccess::key();
  auto pre_image_evaluator = Util::Overload {
    [&](const UnorderedSet& a)
    {
      return key.createSet(detail::compactPreImage<UnorderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl));
    },
    [&](const OrdUnidimDenseSet& a)
    {
      return key.createSet(detail::compactPreImage<OrdUnidimDenseSet, Interval
        , LinearExpr>(a, expr._impl[0]));
    },
    [&](const OrderedSet& a)
    {
      return key.createSet(detail::compactPreImage<OrderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl));
    },
    [&](const auto& a)
    {
      Util::ERROR("MapDetail::preImage: unsupported Set implementation\n");
      return Set{SetKind::kUnordered};
    }
  };
  return std::visit(pre_image_evaluator, key.impl(s));
}

// Inverse ---------------------------------------------------------------------

AtomicMap inverse(const Interval& i, const LinearExpr& linear_expr)
{
  Interval img = image(i, linear_expr);
  if (linear_expr.isConstant()) {
    return AtomicMap{img, LinearExpr{1, i.begin() - img.begin()}};
  } 

  return AtomicMap{img, linear_expr.inverse()};
}

AtomicMDMap inverse(const MultiDimInter& mdi, const ExpressionImpl& expr)
{
  if (mdi.isEmpty()) {
    return AtomicMDMap{};
  }

  MultiDimInter result_domain;
  ExpressionImpl result_law;
  for (unsigned int k = 0; k < mdi.arity(); ++k) {
    AtomicMap atom_map = inverse(mdi[k], expr[k]);
    result_domain.pushBack(atom_map.first);
    result_law.push_back(atom_map.second);
  }

  return AtomicMDMap{result_domain, result_law};
}

template<typename CompactSetImplT, typename PieceT, typename ExprImplT>
std::pair<CompactSetImplT, ExprImplT> compactInverse(const CompactSetImplT& s
  , const ExprImplT& expr, bool is_injective)
{
  CompactSetImplT image = compactImage<CompactSetImplT, PieceT, ExprImplT>(
     s, expr, is_injective);
  Util::ERROR_UNLESS(s.cardinal() == image.cardinal(), "MapDetail/inverse: "
    , "(s, expr) pair is not invertible\n");

  ExprImplT result_law = detail::inverse(*(s.begin()), expr).second;

  return std::pair<CompactSetImplT, ExprImplT>{image, result_law};
}

Map MapDetail::inverse(const Set& s, const Expression& expr)
{
  if (s.isEmpty()) {
    return Map{};
  }

  bool is_injective = expr.isInjective();
  SetAccessKey key = SetAccess::key();
  auto inverse_evaluator = Util::Overload {
    [&](const UnorderedSet& a)
    {
      auto [domain, law] = compactInverse<UnorderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl, is_injective);
      return Map{key.createSet(domain), Expression{law}};
    },
    [&](const OrdUnidimDenseSet& a)
    {
      auto [domain, law] = compactInverse<OrdUnidimDenseSet, Interval
        , LinearExpr>(a, expr._impl[0], is_injective);
      return Map{key.createSet(domain), Expression{ExpressionImpl{law}}};
    },
    [&](const OrderedSet& a)
    {
      auto [domain, law] = compactInverse<OrderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl, is_injective);
      return Map{key.createSet(domain), Expression{law}};
    },
    [&](const auto& a)
    {
      Util::ERROR("MapDetail::preImage: unsupported Set implementation\n");
      return Set{SetKind::kUnordered};
    }
  };
  return std::visit(inverse_evaluator, key.impl(s));
}

// Less image ------------------------------------------------------------------

Interval lessImage(const LinearExpr& linear_expr1
  , const LinearExpr& linear_expr2)
{
  RATIONAL m1 = linear_expr1.slope();
  RATIONAL m2 = linear_expr2.slope();
  if (m1 == m2) {
    RATIONAL h1 = linear_expr1.offset();
    RATIONAL h2 = linear_expr2.offset();
    if (h1 < h2) {
      return Interval{0, 1, Inf};
    }
  } else {
    RATIONAL point = linear_expr1.intersectionPoint(linear_expr2);
    if (point >= 0) {
      NAT floor = point.floor();
      NAT ceil = point.ceiling();
      if (ceil == floor) {
        NAT floor_minus = floor == 0 ? 0 : floor - 1;
        NAT ceil_plus = ceil == Inf ? Inf : ceil + 1;
        Interval kth = m1 < m2 ? Interval{ceil_plus, 1, Inf}
          : Interval {0, 1, floor_minus};
        return kth;
      } else {
        Interval kth = m1 < m2 ? Interval{ceil, 1, Inf}
          : Interval {0, 1, floor};
        return kth;
      }
    } else {
      if (m1 < m2) {
        return Interval{0, 1, Inf};
      }
    }
  }

  return Interval{};
}

std::vector<MultiDimInter> lessImage(const ExpressionImpl& expr1
  , const ExpressionImpl& expr2)
{
  std::vector<MultiDimInter> result;

  unsigned int arity = expr1.size();
  Interval universe_one_dim{0, 1, Inf};
  MultiDimInter less_image{arity, universe_one_dim};
  for (unsigned int k = 0; k < arity; ++k) {
    LinearExpr linear_expr1 = expr1[k];
    LinearExpr linear_expr2 = expr2[k];
    Interval kth_less = lessImage(linear_expr1, linear_expr2);
    if (!kth_less.isEmpty()) {
      less_image[k] = kth_less;
      result.push_back(less_image);
      if (linear_expr1.slope() == linear_expr2.slope()) {
        if (linear_expr1.offset() < linear_expr2.offset()) {
          break;
        }
      }

      RATIONAL cross = linear_expr1.intersectionPoint(linear_expr2);
      if (cross.floor() == cross.ceiling()) {
        less_image[k] = Interval{cross.toNat(), 1, cross.toNat()};
      } else {
        break;
      }
    } else if (linear_expr1 == linear_expr2) {
      less_image[k] = universe_one_dim;
    } else {
      break;
    }
  }

  return result;
}

template<typename CompactSetImplT, typename PieceT, typename ExprImplT>
void lessImage(const ExprImplT& expr1
  , const ExprImplT& expr2, CompactSetImplT& result)
{
  std::vector<PieceT> less_image = detail::lessImage(expr1, expr2);

  for (const PieceT& p : less_image) {
    result.pushBack(p);
  }
}

Set MapDetail::lessImage(const Expression& expr1, const Expression& expr2)
{
  Set result;

  SetAccessKey key = SetAccess::key();
  auto less_image_evaluator = Util::Overload {
    [&](UnorderedSet&& a)
    {
      detail::lessImage<UnorderedSet, MultiDimInter, ExpressionImpl>(
        expr1._impl, expr2._impl, a);
      return key.createSet(a);
    },
    [&](OrdUnidimDenseSet&& a)
    {
      a.pushBack(detail::lessImage(expr1._impl[0], expr2._impl[0]));
      return key.createSet(a);
    },
    [&](OrderedSet&& a)
    {
      detail::lessImage<OrderedSet, MultiDimInter, ExpressionImpl>(
        expr1._impl, expr2._impl, a);
      return key.createSet(a);
    },
    [&](auto&& a)
    {
      Util::ERROR("MapDetail::lessImage: unsupported Set implementation\n");
      return Set{SetKind::kUnordered};
    }
  };
  return std::visit(less_image_evaluator, key.impl(result));
}

// Reduction -------------------------------------------------------------------

void partition(const Interval& i, const LinearExpr& linear_expr
  , AtomicMapVector& result)
{
  INT h = linear_expr.offset().toInt();
  INT absh = std::abs(h);
  for (int j = 1; j <= absh; ++j) {
    NAT new_begin = i.begin() + j - 1;
    Interval jth_piece{new_begin, (NAT) absh, i.end()};
    RATIONAL jth_off;
    if (h > 0) {
      jth_off = jth_piece.end() + h;
    } else {
      jth_off = jth_piece.begin() + h;
    }
    LinearExpr jth_linear_expr{0, jth_off};
    result.emplace_back(jth_piece, jth_linear_expr);
  }
}

AtomicMapVector reduce(const Interval& i, const LinearExpr& linear_expr)
{
  AtomicMapVector result;

  // No partition of the piece is needed
  RATIONAL zero(0, 1);
  INT h = linear_expr.offset().toInt();
  NAT st = i.step();
  if (h == (INT) st) {
    NAT hi = i.end();
    if (st < Inf - hi) {
      LinearExpr convergence_value{zero, static_cast<INT>(hi + st)};
      result.emplace_back(i, convergence_value);
      return result;
    }
  } else if (h == (INT) -st) {
    NAT lo = i.begin();
    if (lo >= st) {
      LinearExpr convergence_value{zero, static_cast<INT>(lo - st)};
      result.emplace_back(i, convergence_value);
      return result;
    }
  }

  // Partition of the piece needed
  if (h % (INT) st == 0) {
    // Is convenient the partition of the piece?
    if ((INT) i.cardinal() > h*h) {
      partition(i, linear_expr, result);
    } else {
      result.emplace_back(i, linear_expr);
    }
  } else {
    result.emplace_back(i, linear_expr);
  }

  return result; 
}

bool isReductionEfficient(const ExpressionImpl& expr)
{
  int count = 0;
  for (const LinearExpr& linear_expr : expr) {
    if (linear_expr.slope() != 1 && linear_expr.slope() != 0) {
      return false;
    }

    if (linear_expr.slope() == 1 && linear_expr.offset() != 0) {
      ++count;
    }
  }

  return count == 1;
}

AtomicMapVector reduce(const MultiDimInter& mdi, const ExpressionImpl& expr
  , unsigned int& k_reduce)
{
  // Identify reducible dimension k_reduce
  std::size_t arity = mdi.arity();
  Interval reducible_interval;
  LinearExpr reducible_expr;
  for (std::size_t k = 0; k < arity; ++k) {
    LinearExpr linear_expr = expr[k];
    if (linear_expr.slope() == 1 && linear_expr.offset() != 0) {
      k_reduce = k; 
      reducible_interval = mdi[k];
      reducible_expr = linear_expr;
      break;
    }
  }

  return reduce(reducible_interval, reducible_expr);
}

template<typename CompactSetImplT>
MapVector MapDetail::compactReduce(const CompactSetImplT& s
  , const ExpressionImpl& expr)
{
  MapVector result;

  SetAccessKey key = SetAccess::key();
  for (const MultiDimInter& mdi : s) {
    unsigned int k_reduce = 0;
    AtomicMapVector reduced = detail::reduce(mdi, expr, k_reduce);
    MultiDimInter mdi_copy = mdi;
    ExpressionImpl expr_copy = expr;
    for (const AtomicMap& r : reduced) {
      CompactSetImplT domain;
      mdi_copy[k_reduce] = r.first;
      domain.pushBack(mdi_copy);
      expr_copy[k_reduce] = r.second;
      result.emplace_back(key.createSet(domain), Expression{expr_copy});
    }
  }

  return result;
}

MapVector MapDetail::reduce(const OrdUnidimDenseSet& s
  , const ExpressionImpl& expr)
{
  MapVector result;

  SetAccessKey key = SetAccess::key();
  for (const Interval& i : s) {
    AtomicMapVector reduced = detail::reduce(i, expr[0]);
    for (const AtomicMap& r : reduced) {
      OrdUnidimDenseSet domain;
      domain.pushBack(r.first);
      result.emplace_back(key.createSet(domain)
        , Expression{ExpressionImpl{r.second}});
    }
  }

  return result;
}

MapVector MapDetail::reduce(const Map& m)
{
  MapVector result;

  Set domain = m.domain();
  Expression law = m.law();
  if (domain.cardinal() == 1 || !isReductionEfficient(law._impl)) {
    result.push_back(m);
    return result;
  }

  SetAccessKey key = SetAccess::key();
  auto reduce_evaluator = Util::Overload {
    [&](const UnorderedSet& a)
    {
      return compactReduce<UnorderedSet>(a, law._impl);
    },
    [&](const OrdUnidimDenseSet& a)
    {
      return reduce(a, law._impl);
    },
    [&](const OrderedSet& a)
    {
      return compactReduce<OrderedSet>(a, law._impl);
    },
    [&](const auto& a)
    {
      Util::ERROR("MapDetail::reduce: unsupported Set implementation\n");
      return Set{SetKind::kUnordered};
    }
  };
  return std::visit(reduce_evaluator, key.impl(m.domain()));
}

// Image multiplicity ----------------------------------------------------------

AtomicMap imageMultiplicity(const Interval& i, const LinearExpr& linear_expr)
{
  Interval image = detail::image(i, linear_expr);
  if (linear_expr.isConstant()) {
    return AtomicMap{image, LinearExpr{0, i.cardinal()}};
  }

  return AtomicMap{image, LinearExpr{0, 1}};
}

AtomicMDMap imageMultiplicity(const MultiDimInter& mdi
  , const ExpressionImpl& expr)
{
  MultiDimInter result_mdi;

  unsigned int card = 1;
  std::size_t arity = mdi.arity();
  for (std::size_t j = 0; j < arity; ++j) {
    AtomicMap jth = imageMultiplicity(mdi[j], expr[j]);
    result_mdi.pushBack(std::get<0>(jth));
    card *= std::get<0>(jth).cardinal();
  }

  ExpressionImpl result_expr{arity, LinearExpr{0, card}};
  return AtomicMDMap{result_mdi, result_expr};
}

template<typename CompactSetImplT, typename PieceT, typename ExprImplT>
MapVector MapDetail::imageMultiplicity(const CompactSetImplT& s
  , const ExprImplT& expr)
{
  MapVector result;

  SetAccessKey key = SetAccess::key();
  for (const PieceT& p : s) {
    MapVector jth_result;

    bool repeated = false;
    AtomicMDMap jth = detail::imageMultiplicity(p, expr);
    Map p_multiplicity{key.createSet(CompactSetImplT{std::get<0>(jth)})
      , std::get<1>(jth)};
    for (const Map& m : result) {
      Set m_domain = m.domain();
      Set cap = m_domain.intersection(p_multiplicity.domain());
      if (!cap.isEmpty()) {
        jth_result.push_back(p_multiplicity + m);
        jth_result.push_back(m.restrict(m_domain.difference(cap)));
        repeated = true;
      }
    }

    if (!repeated) {
      jth_result.push_back(p_multiplicity);
    }

    std::swap(result, jth_result);
  }

  return result;
}

MapVector MapDetail::imageMultiplicity(const Map& m)
{
  MapVector result;

  Set domain = m.domain();
  Expression law = m.law();

  if (m.isEmpty()) {
    return result;
  }

  if (law.isInjective()) {
    Set result_domain = m.image();
    Expression result_expr{MD_NAT{domain.arity(), 1}};
    result.emplace_back(result_domain, result_expr);
    return result;
  }

  if (law.isConstant()) {
    result.emplace_back(m.image(), Expression{MD_NAT{domain.arity()
      , m.domain().cardinal()}});
    return result;
  }

  SetAccessKey key = SetAccess::key();
  auto img_mult_evaluator = Util::Overload {
    [&](const UnorderedSet& a)
    {
      return imageMultiplicity<UnorderedSet, MultiDimInter, ExpressionImpl>(
        a, law._impl);
    },
    [&](const OrdUnidimDenseSet& a)
    {
      Util::ERROR("MapDetail::imageMultiplicity: OrdUnidimDenseSet already "
        , "solved\n");
      return MapVector{};
    },
    [&](const OrderedSet& a)
    {
      return imageMultiplicity<OrderedSet, MultiDimInter, ExpressionImpl>(
        a, law._impl);
    },
    [&](const auto& a)
    {
      Util::ERROR("MapDetail::imageMultiplicity: unsupported Set "
        , "implementation\n");
      return MapVector{};
    }
  };
  return std::visit(img_mult_evaluator, key.impl(m.domain()));
}

} // namespace detail

} // namespace LIB

} // namespace SBG
