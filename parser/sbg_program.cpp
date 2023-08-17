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

#include "parser/sbg_program.hpp"

// Adapt structures ------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(SBG::AST::Program, (SBG::AST::StatementList, stms_)(SBG::AST::ExprList, exprs_))

// SBG program parser ----------------------------------------------------------

namespace SBG {

namespace Parser {

template <typename Iterator>
SBGProgramRule<Iterator>::SBGProgramRule(Iterator &it) : 
  SBGProgramRule::base_type(program_comments), 
  it(it), 
  expr(it),
  stm(it)
{
  program_comments = (stm.stms_comments 
    >> expr.exprs_comments)[qi::_val = phx::construct<AST::Program>(phx::construct<AST::StatementList>(qi::_1), 
                                                                    phx::construct<AST::ExprList>(qi::_2))];
};

template struct SBGProgramRule<StrIt>;

} // namespace Parser

} // namespace SBG
