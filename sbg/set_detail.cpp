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

#include "sbg/set_detail.hpp"
#include "util/defs.hpp"

#include <cmath>

namespace SBG {

namespace LIB {

namespace detail {

// SetAccessKey ----------------------------------------------------------------

SetImpl SetAccessKey::impl(Set s) const { return s._impl; }

Set SetAccessKey::createSet(SetImpl s_impl) const { return Set{s_impl}; }

template<typename SetImplT>
std::vector<MD_NAT> SetAccessKey::flatten(const SetImplT& s) const
{
  std::vector<MD_NAT> result;
  auto pieces = s._pieces;
  result.reserve(pieces.size());

  std::size_t arity = s.arity();
  for (const MultiDimInter& mdi : pieces) {
    MD_NAT min_elem = mdi.minElem();
    MD_NAT x = min_elem;
    unsigned int mdi_sz = mdi.cardinal();
    unsigned int div = mdi_sz; 
    for (unsigned int j = 0; j < mdi_sz; ++j) {
      unsigned int rem = j;
      for (std::size_t k = arity; k > 0; --k) {
        Interval i = mdi[k-1];
        unsigned int kth_card = i.cardinal();
        x[k] = i.begin() + i.step()*fmod(rem, kth_card);
        rem /= kth_card;
      }
    }
  }

  return result;
}

std::vector<MD_NAT> SetAccessKey::flatten(const OrdUnidimDenseSet& s) const
{
  std::vector<MD_NAT> result;

  for (const Interval& i : s._pieces) {
    for (NAT j = i.begin(); j <= i.end(); j += i.step()) {
      result.emplace_back(j);
    }
  }

  return result;
}

std::vector<MD_NAT> SetAccessKey::flatten(const Set& s) const
{
  auto flatten_evaluator = Util::Overload {
    [&](const UnorderedSet& a)
    {
      return flatten<UnorderedSet>(a);
    },
    [&](const OrdUnidimDenseSet& a)
    {
      return flatten(a);
    },
    [&](const OrderedSet& a)
    {
      return flatten<OrderedSet>(a);
    },
    [&](const auto& a) { return std::vector<NAT>{}; }
  };
  return std::visit(flatten_evaluator, s._impl);
}

OrderedSet::OrdMDICollection SetAccessKey::pieces(OrderedSet s) const
{
  return s._pieces;
}

OrdUnidimDenseSet::OrdIntervalCollection SetAccessKey::pieces(
  OrdUnidimDenseSet s) const
{
  return s._pieces;
}

SetAccessKey SetAccess::key() { return SetAccessKey{}; };

} // namespace detail

} // namespace LIB

} // namespace SBG
