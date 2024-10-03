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

#include "eval/visitors/eval_linear.hpp"

namespace SBG {

namespace Eval {

LinearBaseType EvalLinear::operator()(Util::MD_NAT v) const 
{
  Util::ERROR("EvalLinear: trying to evaluate MD_NAT ", v, "\n");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalLinear: trying to evaluate RATIONAL ", v, "\n");
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(ContainerBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalLinear: trying to evaluate ContainerBaseType ", arg, "\n");
  }, v);
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(LinearBaseType v) const
{
  return v;
}

LinearBaseType EvalLinear::operator()(MapBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalLinear: trying to evaluate MapBaseType ", arg, "\n");
  }, v);
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(SBGBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalLinear: trying to evaluate SBGBaseType ", arg, "\n");
  }, v);
  return LinearBaseType();
}

LinearBaseType EvalLinear::operator()(InfoBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalLinear: trying to evaluate InfoBaseType ", arg, "\n");
  }, v);
  return LinearBaseType();
}
} // namespace Eval

} // namespace SBG
