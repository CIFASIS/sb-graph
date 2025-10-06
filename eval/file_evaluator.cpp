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

#include <iostream>

#include "ast/sbg_program.hpp"
#include "eval/visitors/program_evaluator.hpp"
#include "parser/file_parser.hpp"

namespace SBG {

namespace Eval {

ProgramIO parseEvalFile(std::string fname)
{
  SBG::AST::SBGProgram parser_result = SBG::Parser::parseFile(fname);

  SBG::Eval::ProgramEvaluator program_visit; 
  SBG::Eval::ProgramIO visit_result = program_visit.evaluate(parser_result);

  return visit_result;
}

} // namespace Eval

} // namespace SBG
