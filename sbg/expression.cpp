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

#include "sbg/expression.hpp"

#include <iostream>

namespace SBG {

namespace LIB {

// Constructors/Destructors ----------------------------------------------------

Expression::Expression() : _impl() {}

Expression::Expression(const MD_NAT& x)
{
  for (const NAT xj : x) {
    _impl.emplace_back(detail::LinearExpr{0, RATIONAL{static_cast<INT>(xj)}});
  }
}

Expression::Expression(const RATIONAL& slope, const RATIONAL& offset) : _impl()
{
  _impl.emplace_back(detail::LinearExpr{slope, offset});
}

Expression::Expression(std::size_t n, const RATIONAL& slope
  , const RATIONAL& offset)
  : _impl()
{ 
  detail::LinearExpr linear_expr{slope, offset};
  for (unsigned int k = 0; k < n; ++k) {
    _impl.emplace_back(linear_expr);
  }
}

Expression::Expression(const MD_NAT& from, const MD_NAT& to)
{
  for (unsigned int k = 0; k < from.arity(); ++k) {
    _impl.emplace_back(1, to[k] - from[k]);
  }
}

Expression::Expression(const detail::ExpressionImpl& impl) : _impl(impl) {}

Expression::Expression(detail::ExpressionImpl&& impl)
  : _impl(std::move(impl)) {}

// Setters (private) -----------------------------------------------------------

detail::LinearExpr& Expression::operator[](std::size_t n) { return _impl[n]; }

const detail::LinearExpr &Expression::operator[](std::size_t n) const
{
  return _impl[n];
}

// Operators -------------------------------------------------------------------

bool Expression::operator==(const Expression& other) const
{
  return _impl == other._impl;
}

bool Expression::operator!=(const Expression& other) const
{
  return !(*this == other);
}

Expression Expression::operator+(const Expression& other) const
{
  Expression result;

  for (unsigned int k = 0; k < arity(); ++k) {
    result._impl.emplace_back(operator[](k) + other[k]);
  }

  return result;
}

Expression Expression::operator-(const Expression& other) const
{
  Expression result;

  for (unsigned int k = 0; k < arity(); ++k) {
    result._impl.emplace_back(operator[](k) - other[k]);
  }

  return result;
}

std::ostream& Expression::print(std::ostream& out) const
{
  unsigned int sz = arity();

  out << "|";
  if (sz > 0) {
    for (unsigned int k = 0; k < sz-1; ++k)  {
      out << _impl[k] << "|";
    }
    out << _impl[sz-1];
  }
  out << "|";

  return out;
}

std::ostream& operator<<(std::ostream& out, const Expression& expr)
{
  expr.print(out);
  return out;
}

// Linear expression functions -------------------------------------------------

std::size_t Expression::arity() const { return _impl.size(); }

MD_NAT Expression::apply(const MD_NAT& x) const
{
  MD_NAT result;

  for (unsigned int k = 0; k < _impl.size(); ++k) {
    result.pushBack(_impl[k].apply(x[k]));
  }

  return result;
}

Expression Expression::composition(const Expression& other) const
{
  Expression result;

  for (unsigned int k = 0; k < arity(); ++k) {
    result._impl.emplace_back(operator[](k).composition(other[k]));
  }

  return result;
}

Expression Expression::inverse() const
{
  Expression result;

  for (const detail::LinearExpr& le : _impl) {
    result._impl.emplace_back(le.inverse());
  }

  return result;
}

bool Expression::isId() const
{
  for (const detail::LinearExpr& le : _impl) {
    if (!le.isId()) {
      return false;
    }
  }

  return true;
}

bool Expression::isConstant() const
{
  for (const detail::LinearExpr& le : _impl) {
    if (!le.isConstant()) {
      return false;
    }
  }

  return true;
}

bool Expression::isInjective() const
{
  for (const detail::LinearExpr& linear_expr : _impl) {
    if (!linear_expr.isInjective()) {
      return false;
    }
  }

  return true;
}

FixedPointsInfo Expression::fixedPoints() const
{
  std::vector<Solution> result;

  for (unsigned int k = 0; k < arity(); ++k) {
    detail::LinearExpr kth = _impl[k];
    if (kth.isId()) {
      result.emplace_back(Solution{SolutionKind::kFree});
    } else if (kth.isConstant()) {
      result.emplace_back(Solution{SolutionKind::kFixed, kth.offset().toNat()});
    } else {
      return {};
    }
  }

  return FixedPointsInfo{result};
}

Expression Expression::cartesianProduct(const Expression& other) const
{
  Expression result;

  result._impl.insert(result._impl.end(), _impl.begin(), _impl.end());
  result._impl.insert(result._impl.end(), other._impl.begin()
    , other._impl.end());

  return result;
}

rapidjson::Value Expression::toJSON(rapidjson::Document::AllocatorType& alloc)
  const
{
  rapidjson::Value result{rapidjson::kArrayType};

  for (const detail::LinearExpr& le : _impl) {
    rapidjson::Value jth = detail::toJSON(le, alloc);
    result.PushBack(jth, alloc);
  }

  return result;
}

} // namespace LIB

} // namespace SBG
