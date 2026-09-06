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

#include "sbg/linear_expr.hpp"

#include <iostream>
#include <sstream>

namespace SBG {

namespace LIB {

namespace detail {

// Constructors/Destructors ----------------------------------------------------

LinearExpr::LinearExpr() : _slope(1), _offset(0) {}

LinearExpr::LinearExpr(Rational slope, Rational offset)
  : _slope(slope), _offset(offset) {}

// Getters ---------------------------------------------------------------------

const Rational& LinearExpr::slope() const { return _slope; }

const Rational& LinearExpr::offset() const { return _offset; }

// Operators -------------------------------------------------------------------

bool LinearExpr::operator==(const LinearExpr& other) const
{
  return _slope == other._slope && _offset == other._offset;
}

bool LinearExpr::operator!=(const LinearExpr& other) const
{
  return !(*this == other);
}

LinearExpr LinearExpr::operator+(const LinearExpr& other) const
{
  return LinearExpr{_slope + other._slope, _offset + other._offset};
}

LinearExpr LinearExpr::operator-(const LinearExpr& other) const
{
  return LinearExpr{_slope - other._slope, _offset - other._offset};
}

std::ostream& operator<<(std::ostream& out, const LinearExpr& le)
{
  Rational slo = le.slope(), off = le.offset();

  if (slo != 0 && slo != 1) {
    if (slo.numerator() != 1) {
      out << slo.numerator();
    }

    if (slo.denominator() != 1) {
      out << "x/" << slo.denominator();
    } else {
      out << "x";
    }
  }

  if (slo == 1) {
    out << "x";
  }

  if (off != 0) {
    if (off > 0 && slo != 0) {
      out << "+" << off;
    } else {
      out << off;
    }
  }

  if (slo == 0 && off == 0) {
    out << "0";
  }

  return out;
}

// Linear expression functions -------------------------------------------------

Int LinearExpr::apply(const Int& x) const
{
  return (_slope*x + _offset).toInt();
}

LinearExpr LinearExpr::composition(const LinearExpr& other) const
{
  Rational new_slope = other._slope*_slope;
  Rational new_offset = _slope*other._offset + _offset;

  return LinearExpr{new_slope, new_offset};
}

LinearExpr LinearExpr::inverse() const
{
  Rational new_slope{0, 1};
  Rational new_offset{0, 1};
  new_slope = Rational{_slope.denominator(), _slope.numerator()};
  new_offset = (-_offset)/_slope;

  return LinearExpr{new_slope, new_offset};
}

bool LinearExpr::isId() const { return _slope == 1 && _offset == 0; }

bool LinearExpr::isConstant() const { return _slope == 0; }

bool LinearExpr::isInjective() const { return _slope != 0; }

Rational LinearExpr::intersectionPoint(const LinearExpr& other) const
{
  return (other._offset - _offset)/(_slope - other._slope);
}

rapidjson::Value toJSON(LinearExpr le
  , rapidjson::Document::AllocatorType& alloc)
{
  rapidjson::Value result{rapidjson::kArrayType};

  std::stringstream ssm;
  ssm << le.slope();
  rapidjson::Value m;
  m.SetString(ssm.str().c_str(), strlen(ssm.str().c_str()), alloc);
  result.PushBack(m, alloc);

  std::stringstream ssh;
  ssh << le.offset();
  rapidjson::Value h;
  h.SetString(ssh.str().c_str(), strlen(ssh.str().c_str()), alloc);
  result.PushBack(h, alloc);

  return result;
}

} // namespace detail

} // namespace LIB

} // namespace SBG
