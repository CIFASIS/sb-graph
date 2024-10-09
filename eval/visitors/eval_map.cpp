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

#include "eval/visitors/eval_map.hpp"

namespace SBG {

namespace Eval {

MapBaseType EvalMap::operator()(Util::MD_NAT v) const 
{
  Util::ERROR("EvalMap: trying to evaluate MD_NAT ", v, "\n");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(Util::RATIONAL v) const 
{ 
  Util::ERROR("EvalMap: trying to evaluate RATIONAL ", v, "\n");
  return MapBaseType();
}

MapBaseType EvalMap::operator()(ContainerBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalMap: trying to evaluate ContainerBaseType ", arg, "\n");
  }, v);
  return MapBaseType();
}

MapBaseType EvalMap::operator()(LinearBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalMap: trying to evaluate LinearBaseType ", arg, "\n");
  }, v);
  return MapBaseType();
}

MapBaseType EvalMap::operator()(MapBaseType v) const
{
  return v;
}

MapBaseType EvalMap::operator()(SBGBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalMap: trying to evaluate SBGBaseType ", arg, "\n");
  }, v);
  return MapBaseType();
}

MapBaseType EvalMap::operator()(InfoBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalMap: trying to evaluate InfoBaseType ", arg, "\n");
  }, v);
  return MapBaseType();
}

} // namespace Eval

} // namespace SBG
