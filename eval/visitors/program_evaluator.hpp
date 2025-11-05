/** @file program_evaluator.hpp

 @brief <b>SBG Program evaluator</b>

 The program evaluator evaluates all the statements and expressions in the
 desired file.

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

#ifndef PROGRAM_EVALUATOR 
#define PROGRAM_EVALUATOR

#include <boost/variant.hpp>

#include "ast/sbg_program.hpp"
#include "eval/visitors/stm_evaluator.hpp"

namespace SBG {

namespace Eval {

class ProgramEvaluator {
  public:
  ProgramEvaluator();
 
  ProgramIO evaluate(AST::SBGProgram p) const; 
};

} // namespace Eval

} // namespace SBG

#endif
