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

#include "eval/visitors/eval_graph.hpp"

namespace SBG {

namespace Eval {

SBGBaseType EvalGraph::operator()(Util::MD_NAT v) const 
{
  Util::ERROR("EvalGraph: trying to evaluate MD_NAT ", v, "\n");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(Util::RATIONAL v) const 
{
  Util::ERROR("EvalGraph: trying to evaluate RATIONAL ", v, "\n");
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(ContainerBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalGraph: trying to evaluate ContainerBaseType ", arg, "\n");
  }, v);
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(LinearBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalGraph: trying to evaluate LinearBaseType ", arg, "\n");
  }, v);
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(MapBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalGraph: trying to evaluate MapBaseType ", arg, "\n");
  }, v);
  return SBGBaseType();
}

SBGBaseType EvalGraph::operator()(SBGBaseType v) const
{
  return v;
}

SBGBaseType EvalGraph::operator()(InfoBaseType v) const
{
  std::visit([](auto &&arg) {
    Util::ERROR("EvalGraph: trying to evaluate InfoBaseType ", arg, "\n");
  }, v);
  return SBGBaseType();
}

} // namespace Eval

} // namespace SBG
