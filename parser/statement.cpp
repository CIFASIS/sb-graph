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

#include "parser/statement.hpp"

// Adapt structures ------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Assign, (SBG::Util::VariableName, l_)(SBG::AST::Expr, r_))

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::ConfigDims, (SBG::Util::NAT, nmbr_dims_))

// Statement parser ------------------------------------------------------------

namespace SBG {

namespace Parser {

template <typename Iterator>
StmRule<Iterator>::StmRule(Iterator &it) : 
  StmRule::base_type(stms_comments), 
  it(it), 
  expr(it),
  ASSIGN("="),
  NMBR_DIMS("dims =") 
{
  ident = qi::lexeme[(qi::char_('_') | qi::alpha) >> *(qi::alnum | qi::char_('_'))] 
    | qi::lexeme[qi::char_('\'') >> *(qi::alnum | qi::char_('_')) > qi::char_('\'')];

  cfg_dims = (NMBR_DIMS >> qi::uint_)[qi::_val = phx::construct<AST::ConfigDims>(qi::_1)];

  assign = (expr.ident >> ASSIGN >> expr.expr)[qi::_val = phx::construct<AST::Assign>(qi::_1, qi::_2)];

  stm = assign;

  stms_comments = *(expr.comment | stm);
};

template struct StmRule<StrIt>;

} // namespace Parser

} // namespace SBG
