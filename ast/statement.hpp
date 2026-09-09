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

#ifndef SBGRAPH_AST_STATEMENT_HPP_ 
#define SBGRAPH_AST_STATEMENT_HPP_

#include "ast/expr.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace AST {

struct Assign {
  member_class(Name, l);
  member_class(Expr, r);

  Assign();
  Assign(Name l, Expr r);
};
std::ostream &operator<<(std::ostream &out, const Assign &asgn);

struct ConfigDims {
  member_class(std::size_t, arity);
  
  ConfigDims();
  ConfigDims(std::size_t arity);
};
std::ostream &operator<<(std::ostream &out, const ConfigDims &cfg);

struct IsConfig : public boost::static_visitor<bool> {
  public:
  IsConfig();

  bool operator()(Assign v) const;
  bool operator()(ConfigDims v) const;
};

using Statement = boost::variant<Assign, ConfigDims>;
using StatementList = std::vector<Statement>;
std::ostream &operator<<(std::ostream &out, const StatementList &stm);

} // namespace AST

} // namespace SBG

#endif // SBGRAPH_AST_STATEMENT_HPP_
