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
#include <boost/foreach.hpp>

#include <sbg/util/defs.hpp>
#include <sbg/lmap.hpp>

namespace SBG {

// Linear maps ------------------------------------------------------------------------------------

#define ORD_NUMS_TYPE typename LM_TEMP_TYPE::OrdNumeric

LM_TEMPLATE
LM_TEMP_TYPE::LMapImp1() : ndim_(0), gain_(), offset_() {}

LM_TEMPLATE
LM_TEMP_TYPE::LMapImp1(ORD_NUMS_TYPE gain, ORD_NUMS_TYPE offset) : ndim_(0), gain_(), offset_()
{
  OrdNumeric emptyNum;

  if (gain.size() == offset.size()) {
    set_gain(gain);
    set_offset(offset);
    set_ndim(gain.size());
  }
}

LM_TEMPLATE
LM_TEMP_TYPE::LMapImp1(int ndim)
{
  OrdNumeric g;
  OrdNumericIt itg = g.begin();
  OrdNumeric o;
  OrdNumericIt ito = o.begin();

  for (int i = 0; i < ndim; i++) {
    itg = g.insert(itg, 1.0);
    ++itg;
    ito = o.insert(ito, 0);
    ++ito;
  }

  set_gain(g);
  set_offset(o);
  set_ndim(ndim);
}

member_imp_temp(LM_TEMPLATE, LM_TEMP_TYPE, ORD_NUMS_TYPE, gain);
member_imp_temp(LM_TEMPLATE, LM_TEMP_TYPE, ORD_NUMS_TYPE, offset);
member_imp_temp(LM_TEMPLATE, LM_TEMP_TYPE, int, ndim);

LM_TEMPLATE
void LM_TEMP_TYPE::addGO(REAL_IMP g, REAL_IMP o)
{
  gain().insert(gain_ref().end(), g);
  offset().insert(offset_ref().end(), o);
  set_ndim(ndim() + 1);
}

LM_TEMPLATE
bool LM_TEMP_TYPE::empty()
{
  if (gain_ref().empty() || offset_ref().empty()) return true;

  return false;
}

LM_TEMPLATE
bool LM_TEMP_TYPE::isId()
{
  bool res = true;

  BOOST_FOREACH (REAL_IMP gi, gain())
    if (gi != 1) res = false;

  BOOST_FOREACH (REAL_IMP oi, offset())
    if (oi != 0) res = false;

  return res;
}

LM_TEMPLATE
LM_TEMP_TYPE LM_TEMP_TYPE::replace(REAL_IMP g, REAL_IMP o, int dim)
{
  OrdNumeric resg;
  OrdNumericIt itresg = resg.begin();
  OrdNumeric reso;
  OrdNumericIt itreso = reso.begin();

  int i = 1;
  parallel_foreach2 (gain_ref(), offset_ref()) {
    REAL_IMP origg = boost::get<0>(items), origo = boost::get<1>(items);

    if (i == dim) {
      itresg = resg.insert(itresg, g);
      itreso = reso.insert(itreso, o);
    }

    else {
      itresg = resg.insert(itresg, origg);
      itreso = reso.insert(itreso, origo);
    }

    ++itresg;
    ++itreso;
    ++i;
  }

  return LMapImp1(resg, reso);
}

LM_TEMPLATE
LM_TEMP_TYPE LM_TEMP_TYPE::compose(LM_TEMP_TYPE lm2)
{
  OrdNumeric resg;
  OrdNumericIt itresg = resg.begin();
  OrdNumeric reso;
  OrdNumericIt itreso = reso.begin();

  if (ndim() == lm2.ndim()) {
    parallel_foreach2(gain_ref(), lm2.gain_ref()) {
      REAL_IMP g1 = boost::get<0>(items), g2 = boost::get<1>(items);

      itresg = resg.insert(itresg, g1 * g2);
      ++itresg;
    }

    parallel_foreach3(gain_ref(), offset_ref(), lm2.offset_ref()) {
      REAL_IMP g1 = boost::get<0>(items);
      REAL_IMP o1 = boost::get<1>(items), o2 = boost::get<2>(items);
     
      itreso = reso.insert(itreso, o2 * g1 + o1);
      ++itreso;
    }
  }

  else return LMapImp1();

  return LMapImp1(resg, reso);
}

LM_TEMPLATE
LM_TEMP_TYPE LM_TEMP_TYPE::invLMap()
{
  OrdNumeric resg;
  OrdNumericIt itresg = resg.begin();
  OrdNumeric reso;
  OrdNumericIt itreso = reso.begin();

  parallel_foreach2 (gain_ref(), offset_ref()) {
    REAL_IMP origg = boost::get<0>(items), origo = boost::get<1>(items);
 
    if (origg != 0) {
      itresg = resg.insert(itresg, 1 / origg);
      itreso = reso.insert(itreso, -origo / origg);
    }

    else {
      itresg = resg.insert(itresg, Inf);
      itreso = reso.insert(itreso, -Inf);
    }

    ++itresg;
    ++itreso;
  }

  return LMapImp1(resg, reso);
}

LM_TEMPLATE
LM_TEMP_TYPE LM_TEMP_TYPE::addLM(LM_TEMP_TYPE lm2)
{
  OrdNumeric resg;
  OrdNumericIt itresg = resg.begin();
  OrdNumeric reso;
  OrdNumericIt itreso = reso.begin();

  if (ndim() == lm2.ndim()) {
    parallel_foreach4 (gain_ref(), offset_ref(), lm2.gain_ref(), lm2.offset_ref()) {
      REAL_IMP g1 = boost::get<0>(items), o1 = boost::get<1>(items);
      REAL_IMP g2 = boost::get<2>(items), o2 = boost::get<3>(items);

      itresg = resg.insert(itresg, g1 + g2);
      ++itresg;
      itreso = reso.insert(itreso, o1 + o2);
      ++itreso;
    }
  }

  return LMapImp1(resg, reso);
}

LM_TEMPLATE
LM_TEMP_TYPE LM_TEMP_TYPE::diffLM(LM_TEMP_TYPE lm2)
{
  LMapImp1 negativeLm2;

  parallel_foreach2 (lm2.gain_ref(), lm2.offset_ref()) {
    REAL_IMP g2 = boost::get<0>(items), o2 = boost::get<1>(items);

    negativeLm2.addGO(-g2, -o2);
  }

  return this->addLM(negativeLm2);
}

LM_TEMPLATE
bool LM_TEMP_TYPE::operator==(const LM_TEMP_TYPE &other) const { return gain() == other.gain() && offset() == other.offset(); }

template struct LMapImp1<OrdCT, REAL>;

template <typename REAL_IMP>
std::string mapOper(REAL_IMP &cte)
{
  return (cte >= 0) ? "+ " : "";
}

template std::string mapOper<REAL>(REAL &cte);

LM_TEMPLATE
std::ostream &operator<<(std::ostream &out, const LM_TEMP_TYPE &lm)
{
  ORD_NUMS_TYPE g = lm.gain();
  ORD_NUMS_TYPE::iterator itg = g.begin();
  ORD_NUMS_TYPE o = lm.offset();
  ORD_NUMS_TYPE::iterator ito = o.begin();

  if (g.size() == 0) return out;

  if (g.size() == 1) {
    out << "[" << *itg << " * x " << mapOper(*ito) << *ito << "]";
    return out;
  }

  out << "[" << *itg << " * x " << mapOper(*ito) << *ito;
  ++itg;
  ++ito;
  while (next(itg, 1) != g.end()) {
    out << ", " << *itg << " * x " << mapOper(*ito) << *ito;

    ++itg;
    ++ito;
  }
  out << ", " << *itg << " * x " << mapOper(*ito) << *ito << "]";

  return out;
}

template std::ostream &operator<<(std::ostream &out, const LMap &lm);

}  // namespace SBG
