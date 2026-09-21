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

#include <sbgraph/ast/statement.hpp>

namespace SBG {

namespace AST {

Assign::Assign(Name left, Expr right) : _left(left), _right(right) {}

const Name& Assign::left() const { return _left; }

const Expr& Assign::right() const { return _right; }

std::ostream &operator<<(std::ostream &out, const Assign &asgn)
{
  out << asgn.left() << " = " << asgn.right();

  return out;
}

ConfigDims::ConfigDims(std::size_t arity) : _arity(arity) {
  if (arity > 0) {
    _arity = arity;
  }

  else {
    Util::ERROR("ConfigDims::ConfigDims: dimension should be greater than 0\n");
  }
}

const std::size_t ConfigDims::arity() const { return _arity; }

std::ostream &operator<<(std::ostream &out, const ConfigDims &cfg)
{
  out << "arity = " << cfg.arity();

  return out;
}

bool IsConfig::operator()(Assign v) const { return false; }

bool IsConfig::operator()(ConfigDims v) const { return true; }

std::ostream &operator<<(std::ostream &out, const StatementList &stml)
{
  for (Statement s : stml) {
    out << s << ";\n";
  }

  return out;
}

} // namespace AST

} // namespace SBG
