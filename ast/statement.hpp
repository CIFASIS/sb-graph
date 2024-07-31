/** @file statement.hpp

 @brief <b>Statements AST definition</b>

 <hr>

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

#ifndef PARSER_STATEMENT_AST_HPP
#define PARSER_STATEMENT_AST_HPP

#include "ast/expr.hpp"

namespace SBG {

namespace AST {

struct Assign {
  member_class(Util::VariableName, l);
  member_class(Expr, r);

  Assign();
  Assign(Util::VariableName l, Expr r);
};
std::ostream &operator<<(std::ostream &out, const Assign &asgn);

struct ConfigDims {
  member_class(Util::NAT, nmbr_dims);
  
  ConfigDims();
  ConfigDims(Util::NAT nmbr_dims);
};
std::ostream &operator<<(std::ostream &out, const ConfigDims &cfg);

struct IsConfig : public boost::static_visitor<bool> {
  public:
  IsConfig();

  bool operator()(Assign v) const;
  bool operator()(ConfigDims v) const;
};

typedef boost::variant<Assign, ConfigDims> Statement;
typedef std::vector<Statement> StatementList;
std::ostream &operator<<(std::ostream &out, const StatementList &stm);

} // namespace AST

} // namespace SBG

#endif
