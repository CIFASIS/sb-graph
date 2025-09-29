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

#include "eval/eval_context.hpp"

namespace SBG {

namespace Eval {

EvalContext::EvalContext() : arity_(1), venv_(), fenv_() {}

// Getters ---------------------------------------------------------------------

unsigned int EvalContext::arity() const
{
  return arity_;
}

VarEnv& EvalContext::venv()
{
  return venv_;
}

FuncEnv& EvalContext::fenv()
{
  return fenv_;
}

// Setters ---------------------------------------------------------------------

void EvalContext::setArity(unsigned int arity)
{
  arity_ = arity;
}

void EvalContext::insertVariable(VarEnv::VKey key, VarEnv::VValue value)
{
  venv_.insert(key, value);
}

void EvalContext::insertFunction(FuncEnv::FKey key, FuncEnv::FValue value)
{
  fenv_.insert(key, value);
}

} // namespace Eval

} // namespace SBG
