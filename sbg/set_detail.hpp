/** @file set_detail.hpp

 @brief <b>PWMap implementation access</b>

 This module is intended for internal use to access the implementation of
 Sets, for example for the DomOrdPWMap implementation.

 <hr>

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

#ifndef SBGRAPH_SBG_SET_DETAIL_HPP_
#define SBGRAPH_SBG_SET_DETAIL_HPP_

#include "sbg/set.hpp"
#include "sbg/ord_set.hpp"
#include "sbg/ord_unidim_dense_set.hpp"

namespace SBG {

namespace LIB {

namespace detail {

class SetAccessKey {
public:
  SetImpl impl(Set s) const;
  Set createSet(SetImpl s_impl) const;

  OrderedSet::OrdMDICollection pieces(OrderedSet s) const;
  OrdUnidimDenseSet::OrdIntervalCollection pieces(OrdUnidimDenseSet s) const;

private:
  SetAccessKey() = default;

  friend class SetAccess;
};

class SetAccess {
public:
  static SetAccessKey key();
};

} // namespace detail

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_SBG_SET_DETAIL_HPP_
