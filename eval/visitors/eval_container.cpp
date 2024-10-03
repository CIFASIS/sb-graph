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

#include "eval/visitors/eval_container.hpp"

namespace SBG {

namespace Eval {

ContainerBaseType EvalContainer::operator()(Util::MD_NAT v) const 
{
  Util::ERROR("EvalContainer: trying to evaluate MD_NAT ", v, "\n");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalContainer: trying to evaluate RATIONAL ", v, "\n");
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(ContainerBaseType v) const
{
  return v;
}

ContainerBaseType EvalContainer::operator()(LinearBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalContainer: trying to evaluate LinearBaseType ", arg, "\n");
  }, v);
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(MapBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalContainer: trying to evaluate MapBaseType ", arg, "\n");
  }, v);
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(SBGBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalContainer: trying to evaluate SBGBaseType ", arg, "\n");
  }, v);
  return ContainerBaseType();
}

ContainerBaseType EvalContainer::operator()(InfoBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalContainer: trying to evaluate InfoBaseType ", arg, "\n");
  }, v);
  return ContainerBaseType();
}

} // namespace Eval

} // namespace SBG
