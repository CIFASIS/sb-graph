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

#include <eval/visitors/operation_visitor.hpp>

namespace SBG {

namespace Eval {

// isEmpty ---------------------------------------------------------------------

EmptyVisitor::EmptyVisitor() {}

bool EmptyVisitor::operator()(Util::INT v) const 
{
  Util::ERROR("EmptyVisitor: trying to check if INT is empty");
  return true;
}

bool EmptyVisitor::operator()(Util::RATIONAL v) const 
{
  Util::ERROR("EmptyVisitor: trying to check if RATIONAL is empty");
  return true;
}

bool EmptyVisitor::operator()(SBG::Interval i) const { return isEmpty(i); }

bool EmptyVisitor::operator()(SBG::InterSet ii) const 
{ 
  Util::ERROR("EmptyVisitor: trying to check if InterSet is empty");
  return true;
}

// Membership ------------------------------------------------------------------

/*
bool MemberVisitor::operator()(Util::INT v) const {
  Util::ERROR("MemberVisitor: trying to check if INT has element");
  return false;
}

bool MemberVisitor::operator()(Util::RATIONAL v) const {
  Util::ERROR("MemberVisitor: trying to check if RATIONAL has element");
  return false;
}

bool MemberVisitor::operator()(SBG::Interval i) const { return isMember(x(), i); }

bool MemberVisitor::operator()(SBG::InterSet ii) const {
  Util::ERROR("MemberVisitor: trying to check if InterSet has element");
  return false;
}*/

MemberVisitor::MemberVisitor() {}

//bool MemberVisitor::operator()(Util::INT x, SBG::Interval i) const { return isMember(x, i); }

template <class T1, class T2>
bool MemberVisitor::operator()(T1 x, T2 i) const 
{
  Util::ERROR("MemberVisitor: is not a container");
  return false;
} 

} // namespace Eval

} // namespace SBG
