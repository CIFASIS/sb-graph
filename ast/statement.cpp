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

#include <ast/statement.hpp>

namespace SBG {

namespace AST {

Assign::Assign() : l_(), r_() {}
Assign::Assign(Util::VariableName l, Expr r) : l_(l), r_(r) {}

member_imp(Assign, Util::VariableName, l);
member_imp(Assign, Expr, r);

std::ostream &operator<<(std::ostream &out, const Assign &asgn)
{
  out << asgn.l() << " = " << asgn.r();

  return out;
}

std::ostream &operator<<(std::ostream &out, const StatementList &stml)
{
  BOOST_FOREACH (Statement s, stml)
    out << s << "\n";

  return out;
}

} // namespace AST

} // namespace SBG