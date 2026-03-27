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
#include "sbg/set_fact.hpp"
#include "sbg/unord_set.hpp"

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

template<typename CompactSetImpl, typename Piece, typename Expr>
CompactSetImpl compactImage(const CompactSetImpl& s, const Expr& expr
  , bool is_injective)
{
  CompactSetImpl result;

  if (is_injective) {
    for (const Piece& p : s) {
      result.pushBack(detail::image(p, expr));
    }
  } else {
    for (const Piece& p : s) {
      CompactSetImpl jth_image{detail::image(p, expr)};
      result = std::move(result).cup(std::move(jth_image));
    }
  }

  return result;
}

Set MapDetail::image(const Set& s, const Expression& expr)
{
  auto image_evaluator = Overload {
    [&](const UnorderedSet& a)
    {
      return Set{detail::compactImage<UnorderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl, expr.isInjective())};
    },
    [&](const OrdUnidimDenseSet& a)
    {
      return Set{detail::compactImage<OrdUnidimDenseSet, Interval, LinearExpr>(
        a, expr._impl[0], expr.isInjective())};
    },
    [&](const OrderedSet& a)
    {
      return Set{detail::compactImage<OrderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl, expr.isInjective())};
    },
    [&](const auto& a) { return Set{SetKind::kUnordered}; }
  };
  return std::visit(image_evaluator, s._impl);
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

template<typename CompactSetImpl, typename Piece, typename Expr>
CompactSetImpl compactPreImage(const CompactSetImpl& s, const Expr& expr)
{
  CompactSetImpl result;

  for (const Piece& p : s) {
    result.pushBack(detail::preImage(p, expr));
  }

  return result;
}

Set MapDetail::preImage(const Set& s, const Expression& expr)
{
  auto pre_image_evaluator = Overload {
    [&](const UnorderedSet& a)
    {
      return Set{detail::compactPreImage<UnorderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl)};
    },
    [&](const OrdUnidimDenseSet& a)
    {
      return Set{detail::compactPreImage<OrdUnidimDenseSet, Interval
        , LinearExpr>(a, expr._impl[0])};
    },
    [&](const OrderedSet& a)
    {
      return Set{detail::compactPreImage<OrderedSet, MultiDimInter
        , ExpressionImpl>(a, expr._impl)};
    },
    [&](const auto& a) { return Set{SetKind::kUnordered}; }
  };
  return std::visit(pre_image_evaluator, s._impl);
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
    } else {
      break;
    }
  }

  return result;
}

template<typename CompactSetImpl, typename Piece, typename Expr>
void lessImage(const Expr& expr1
  , const Expr& expr2, CompactSetImpl& result)
{
  std::vector<Piece> less_image = detail::lessImage(expr1, expr2);

  for (const Piece& p : less_image) {
    result.pushBack(p);
  }
}

Set MapDetail::lessImage(const Expression& expr1, const Expression& expr2)
{
  Set result = SET_FACT.createSet();

  auto less_image_evaluator = Overload {
    [&](UnorderedSet& a)
    {
      detail::lessImage<UnorderedSet, MultiDimInter, ExpressionImpl>(
        expr1._impl, expr2._impl, a);
      return Set{a};
    },
    [&](OrdUnidimDenseSet& a)
    {
      a.pushBack(detail::lessImage(expr1._impl[0], expr2._impl[0]));
      return Set{a};
    },
    [&](OrderedSet& a)
    {
      detail::lessImage<OrderedSet, MultiDimInter, ExpressionImpl>(
        expr1._impl, expr2._impl, a);
      return Set{a};
    },
    [&](auto& a) { return Set{SetKind::kUnordered}; }
  };
  return std::visit(less_image_evaluator, result._impl);

  return result;
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
  unsigned int arity = mdi.arity();
  Interval reducible_interval;
  LinearExpr reducible_expr;
  for (auto k = 0; k < arity; ++k) {
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

template<typename CompactSetImpl>
MapVector MapDetail::MDICollectionReduce(const CompactSetImpl& s
  , const ExpressionImpl& expr)
{
  MapVector result;

  for (const MultiDimInter& mdi : s) {
    unsigned int k_reduce = 0;
    AtomicMapVector reduced = detail::reduce(mdi, expr, k_reduce);
    MultiDimInter mdi_copy = mdi;
    ExpressionImpl expr_copy = expr;
    for (const AtomicMap& r : reduced) {
      CompactSetImpl domain;
      mdi_copy[k_reduce] = r.first;
      domain.pushBack(mdi_copy);
      expr_copy[k_reduce] = r.second;
      result.emplace_back(Set{domain}
        , Expression{expr_copy});
    }
  }

  return result;
}

MapVector MapDetail::reduce(const OrdUnidimDenseSet& s
  , const ExpressionImpl& expr)
{
  MapVector result;

  for (const Interval& i : s) {
    AtomicMapVector reduced = detail::reduce(i, expr[0]);
    for (const AtomicMap& r : reduced) {
      OrdUnidimDenseSet domain;
      domain.pushBack(r.first);
      result.emplace_back(Set{domain}
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

  auto reduce_evaluator = Overload {
    [&](const UnorderedSet& a)
    {
      return MDICollectionReduce<UnorderedSet>(a, law._impl);
    },
    [&](const OrdUnidimDenseSet& a)
    {
      return reduce(a, law._impl);
    },
    [&](const OrderedSet& a)
    {
      return MDICollectionReduce<OrderedSet>(a, law._impl);
    },
    [&](const auto& a) { return Set{SetKind::kUnordered}; }
  };
  return std::visit(reduce_evaluator, m.domain()._impl);
}

} // namespace detail

} // namespace LIB

} // namespace SBG
