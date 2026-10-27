/** @file input_translator.hpp

 @brief <b>User input translator</b>

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

#ifndef EVAL_INPUT_TRANSLATOR_HPP
#define EVAL_INPUT_TRANSLATOR_HPP

#include "eval/user_input.hpp"

namespace SBG {

namespace Eval {

class InputTranslator {
  public:
  InputTranslator();

  void translate(EvalUserInput& input);
};

} // namespace Eval

} // namespace SBG

#endif
