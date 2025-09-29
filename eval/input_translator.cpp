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

#include "eval/input_translator.hpp"
#include "eval/user_impl_map.hpp"
#include "sbg/af_set.hpp"

namespace SBG {

namespace Eval {

InputTranslator::InputTranslator() {}

void InputTranslator::translate(EvalUserInput& input)
{
  EvalUserInput::MaybeInt s = input.set_impl();
  if (s) {
    int set_impl = *s;
    LIB::SetFactPtr set_fact = std::get<LIB::SetFactPtr>(IMPL_MAP
      .getFactory("set", set_impl));
    LIB::SetFactory::instance().set_set_fact(std::move(set_fact));
  }

  return;
}

} // namespace Eval

} // namespace SBG
