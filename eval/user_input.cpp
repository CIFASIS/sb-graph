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
  return set_impl_;
}

EvalUserInput::MaybeInt EvalUserInput::pw_impl() const
{
  return pw_impl_;
}

EvalUserInput::MaybeInt EvalUserInput::match_impl() const
{
  return match_impl_;
}

EvalUserInput::MaybeInt EvalUserInput::scc_impl() const
{
  return scc_impl_;
}

EvalUserInput::MaybeInt EvalUserInput::ts_impl() const
{
  return ts_impl_;
}

EvalUserInput::MaybeInt EvalUserInput::cv_impl() const
{
  return cv_impl_;
}

// Setters ---------------------------------------------------------------------

void EvalUserInput::set_set_impl(MaybeInt set_impl)
{
  set_impl_ = set_impl;
}

void EvalUserInput::set_pw_impl(MaybeInt pw_impl)
{
  pw_impl_ = pw_impl;
}

void EvalUserInput::set_match_impl(MaybeInt match_impl)
{
  match_impl_ = match_impl;
}

void EvalUserInput::set_scc_impl(MaybeInt scc_impl)
{
  scc_impl_ = scc_impl;
}

void EvalUserInput::set_ts_impl(MaybeInt ts_impl)
{
  ts_impl_ = ts_impl;
}

void EvalUserInput::set_cv_impl(MaybeInt cv_impl)
{
  cv_impl_ = cv_impl;
}

} // namespace detail

} // namespace Eval

} // namespace SBG
