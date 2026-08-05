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

#include "eval/user_input.hpp"

namespace SBG {

namespace Eval {

namespace detail {

// Constructors ----------------------------------------------------------------

EvalUserInput::EvalUserInput() {}

// Getters ---------------------------------------------------------------------

EvalUserInput::MaybeInt EvalUserInput::set_impl() const
{
  return _set_impl;
}

EvalUserInput::MaybeInt EvalUserInput::pw_impl() const
{
  return _pw_impl;
}

EvalUserInput::MaybeInt EvalUserInput::match_impl() const
{
  return _match_impl;
}

EvalUserInput::MaybeInt EvalUserInput::scc_impl() const
{
  return _scc_impl;
}

EvalUserInput::MaybeInt EvalUserInput::mfvs_impl() const
{
  return _mfvs_impl;
}

EvalUserInput::MaybeInt EvalUserInput::ts_impl() const
{
  return _ts_impl;
}

// Setters ---------------------------------------------------------------------

void EvalUserInput::set_set_impl(MaybeInt set_impl)
{
  _set_impl = set_impl;
}

void EvalUserInput::set_pw_impl(MaybeInt pw_impl)
{
  _pw_impl = pw_impl;
}

void EvalUserInput::set_match_impl(MaybeInt match_impl)
{
  _match_impl = match_impl;
}

void EvalUserInput::set_scc_impl(MaybeInt scc_impl)
{
  _scc_impl = scc_impl;
}

void EvalUserInput::set_mfvs_impl(MaybeInt mfvs_impl)
{
  _mfvs_impl = mfvs_impl;
}

void EvalUserInput::set_ts_impl(MaybeInt ts_impl)
{
  _ts_impl = ts_impl;
}

} // namespace detail

} // namespace Eval

} // namespace SBG
