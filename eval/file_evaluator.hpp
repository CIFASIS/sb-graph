/** @file file_evaluator.hpp

 @brief <b>File Evaluator</b>

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

#ifndef SBGRAPH_EVAL_FILE_EVALUATOR_HPP_
#define SBGRAPH_EVAL_FILE_EVALUATOR_HPP_

#include "eval/pretty_print.hpp"

#include <string>

namespace SBG {

namespace Eval {

ProgramIO parseEvalFile(std::string fname);

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_FILE_EVALUATOR_HPP_
