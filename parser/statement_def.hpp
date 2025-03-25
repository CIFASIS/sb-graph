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

#ifndef STATEMENT_DEF_PARSER_HPP
#define STATEMENT_DEF_PARSER_HPP

#include <boost/phoenix.hpp>

#include "ast/statement.hpp"

// Adapt structures ------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(
  SBG::AST::Assign, (SBG::AST::VariableName, l_)(SBG::AST::Expr, r_)
)

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::ConfigDims, (SBG::LIB::NAT, nmbr_dims_))

// Statement parser ------------------------------------------------------------

namespace SBG {

namespace Parser {

namespace phx = boost::phoenix;

template <typename Iterator>
StmRule<Iterator>::StmRule(Iterator &it) : StmRule::base_type(stms) 
  , it(it) 
  , expr(it)
  , ASSIGN("=")
  , NMBR_DIMS("dims =") 
{
  cfg_dims = (NMBR_DIMS >> qi::uint_)
    [qi::_val = phx::construct<AST::ConfigDims>(qi::_1)];

  assign = (expr.ident >> ASSIGN >> expr.expr)
    [qi::_val = phx::construct<AST::Assign>(qi::_1, qi::_2)];

  stm = assign >> expr.SEMI;

  stms = -(cfg_dims[phx::push_back(qi::_val, qi::_1)] >> expr.SEMI)
    >> *(stm[phx::push_back(qi::_val, qi::_1)]);
};

} // namespace Parser

} // namespace SBG

#endif
