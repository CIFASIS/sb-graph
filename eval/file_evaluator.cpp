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

#include "ast/sbg_program.hpp"
#include "eval/pretty_print.hpp"
#include "eval/visitors/program_evaluator.hpp"
#include "parser/file_parser.hpp"

#include <iostream>
#include <string>

namespace SBG {

namespace Eval {

ProgramIO parseEvalFile(std::string fname)
{
  AST::SBGProgram parser_result = Parser::parseFile(fname);

  detail::ProgramEvaluator program_visit; 
  ProgramIO visit_result = program_visit.evaluate(parser_result);

  return visit_result;
}

} // namespace Eval

} // namespace SBG
