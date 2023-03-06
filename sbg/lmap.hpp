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

#include <iostream>

#include <boost/foreach.hpp>

#include <sbg/util/defs.hpp>

namespace SBG {

// LinearMaps ---------------------------------------------------------------------------------------

#define LM_TEMPLATE template <template <typename T, typename = std::allocator<T>> class ORD_CT, typename REAL_IMP>

#define LM_TEMP_TYPE LMapImp1<ORD_CT, REAL_IMP>

LM_TEMPLATE
struct LMapImp1 {
  typedef ORD_CT<REAL_IMP> OrdNumeric;
  typedef typename OrdNumeric::iterator OrdNumericIt;

  member_class(OrdNumeric, gain);
  member_class(OrdNumeric, offset);
  member_class(int, ndim);

  LMapImp1();
  LMapImp1(OrdNumeric g, OrdNumeric o);
  LMapImp1(int dim);  // Constructs the id of LMaps

  void addGO(REAL_IMP g, REAL_IMP o);

  bool empty();
  bool isId();

  LMapImp1 replace(REAL_IMP g, REAL_IMP o, int dim);

  LMapImp1 compose(LMapImp1 lm2);
  LMapImp1 invLMap();
  LMapImp1 addLM(LMapImp1 lm2);
  LMapImp1 diffLM(LMapImp1 lm2);

  eq_class(LMapImp1);
};

typedef LMapImp1<OrdCT, REAL> LMap;

template <typename REAL_IMP>
std::string mapOper(REAL_IMP &cte);

printable_temp(LM_TEMPLATE, LM_TEMP_TYPE);

}  // namespace SBG
