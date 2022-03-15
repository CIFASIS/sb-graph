/*****************************************************************************

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
#pragma once

#include <sbg/util/defs.hpp>
#include <sbg/interval.hpp>
#include <sbg/multi_interval.hpp>
#include <sbg/lmap.hpp>

namespace SBG {

// Piecewise atomic linear maps -----------------------------------------------------------------

#define APW_TEMPLATE                                                                                                                \
  template <template <typename T, typename = std::allocator<T>> class ORD_CT, typename LM_IMP, typename MI_IMP, typename INTER_IMP, \
            typename INT_IMP, typename REAL_IMP>

#define APW_TEMP_TYPE AtomPWLMapImp1<ORD_CT, LM_IMP, MI_IMP, INTER_IMP, INT_IMP, REAL_IMP>

APW_TEMPLATE
struct AtomPWLMapImp1 {
  member_class(MI_IMP, dom);
  member_class(LM_IMP, lmap);

  AtomPWLMapImp1();
  AtomPWLMapImp1(MI_IMP d, LM_IMP l);
  AtomPWLMapImp1(MI_IMP dom, MI_IMP image);

  bool empty();
  bool isId();

  MI_IMP image(MI_IMP as);
  MI_IMP image();
  MI_IMP preImage(MI_IMP as);

  eq_class(AtomPWLMapImp1);
};

typedef AtomPWLMapImp1<OrdCT, LMap, MultiInterval, Interval, INT, REAL> AtomPWLMap;

printable_temp(APW_TEMPLATE, APW_TEMP_TYPE);

}  // namespace SBG
