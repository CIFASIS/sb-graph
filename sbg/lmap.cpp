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
#include <sbg/lmap.hpp>

namespace SBG {

// Linear maps ------------------------------------------------------------------------------------

#define ORD_NUMS_TYPE typename LM_TEMP_TYPE::OrdNumeric

LM_TEMPLATE
LM_TEMP_TYPE::LMapImp1() : ndim_(0), gain_(), offset_() {}

LM_TEMPLATE
LM_TEMP_TYPE::LMapImp1(ORD_NUMS_TYPE gain, ORD_NUMS_TYPE offset) : ndim_(0)
{
  OrdNumeric emptyNum;

  if (gain.size() == offset.size()) {
    gain_ = gain;
    offset_ = offset;
    ndim_ = gain.size();
  }

  else {
    gain_ = emptyNum;
    offset_ = emptyNum;
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

  gain_ = g;
  offset_ = o;
  ndim_ = ndim;
}

member_imp_temp(LM_TEMPLATE, LM_TEMP_TYPE, ORD_NUMS_TYPE, gain);
member_imp_temp(LM_TEMPLATE, LM_TEMP_TYPE, ORD_NUMS_TYPE, offset);
member_imp_temp(LM_TEMPLATE, LM_TEMP_TYPE, int, ndim);

LM_TEMPLATE
void LM_TEMP_TYPE::addGO(REAL_IMP g, REAL_IMP o)
{
  gain_.insert(gain_.end(), g);
  offset_.insert(offset_.end(), o);
  ++ndim_;
}

LM_TEMPLATE
bool LM_TEMP_TYPE::empty()
{
  if (gain_ref().empty() && offset_ref().empty()) return true;

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

  OrdNumericIt itg = gain_ref().begin();
  OrdNumericIt ito = offset_ref().begin();

  for (int i = 1; i <= ndim(); i++) {
    if (i == dim) {
      itresg = resg.insert(itresg, g);
      itreso = reso.insert(itreso, o);
    }

    else {
      itresg = resg.insert(itresg, *itg);
      itreso = reso.insert(itreso, *ito);
    }

    ++itg;
    ++ito;
    ++itresg;
    ++itreso;
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

  OrdNumericIt ito1 = offset_ref().begin();
  OrdNumericIt itg2 = lm2.gain_ref().begin();
  OrdNumericIt ito2 = lm2.offset_ref().begin();

  if (ndim() == lm2.ndim()) {
    BOOST_FOREACH (REAL_IMP g1i, gain()) {
      itresg = resg.insert(itresg, g1i * (*itg2));
      ++itresg;
      itreso = reso.insert(itreso, (*ito2) * g1i + (*ito1));
      ++itreso;

      ++ito1;
      ++itg2;
      ++ito2;
    }
  }

  else {
    LMapImp1 aux;
    return aux;
  }

  return LMapImp1(resg, reso);
}

LM_TEMPLATE
LM_TEMP_TYPE LM_TEMP_TYPE::invLMap()
{
  OrdNumeric resg;
  OrdNumericIt itresg = resg.begin();
  OrdNumeric reso;
  OrdNumericIt itreso = reso.begin();

  OrdNumericIt ito1 = offset_ref().begin();

  BOOST_FOREACH (REAL_IMP g1i, gain()) {
    if (g1i != 0) {
      itresg = resg.insert(itresg, 1 / g1i);
      ++itresg;

      itreso = reso.insert(itreso, -(*ito1) / g1i);
      ++itreso;
    }

    else {
      itresg = resg.insert(itresg, Inf);
      ++itresg;

      itreso = reso.insert(itreso, -Inf);
      ++itreso;
    }

    ++ito1;
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
    OrdNumericIt ito = offset_ref().begin();
    OrdNumericIt itg2 = lm2.gain_ref().begin();
    OrdNumericIt ito2 = lm2.offset_ref().begin();

    BOOST_FOREACH (REAL_IMP gi, gain()) {
      itresg = resg.insert(itresg, gi + *itg2);
      ++itresg;
      itreso = reso.insert(itreso, *ito + *ito2);
      ++itreso;

      ++ito;
      ++itg2;
      ++ito2;
    }
  }

  LMapImp1 aux(resg, reso);
  return LMapImp1(resg, reso);
}

LM_TEMPLATE
LM_TEMP_TYPE LM_TEMP_TYPE::diffLM(LM_TEMP_TYPE lm2)
{
  OrdNumeric resg;
  OrdNumericIt itresg = resg.begin();
  OrdNumeric reso;
  OrdNumericIt itreso = reso.begin();

  if (ndim() == lm2.ndim()) {
    OrdNumericIt ito = offset_ref().begin();
    OrdNumericIt itg2 = lm2.gain_ref().begin();
    OrdNumericIt ito2 = lm2.offset_ref().begin();

    BOOST_FOREACH (REAL_IMP gi, gain()) {
      itresg = resg.insert(itresg, gi - *itg2);
      ++itresg;
      itreso = reso.insert(itreso, *ito - *ito2);
      ++itreso;

      ++ito;
      ++itg2;
      ++ito2;
    }
  }

  LMapImp1 aux(resg, reso);
  return LMapImp1(resg, reso);
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

// ! image and dom should be compatible
// ! Two intervals are compatible if lo2 = lo1 * (st1 / st2)
LMap mapFromInters(Interval dom, Interval image)
{
  LMap res;

  SBG::REAL m = ;
  SBG::REAL h = ;
  res.addGO(m, h);

  return res;  
}

// ! image and dom should be compatible
LMap mapFromMI(MultiInterval dom, MultiInterval image)
{
  LMap res;

  parallel_foreach2 (dom.inters_ref(), image.inters_ref()) {
    Interval i_dom = boost::get<0>(items), i_im = boost::get<1>(items);

    LMap ith = mapFromInters(i_dom, i_im); 
    res.addGO(*(ith.gain().begin()), *(ith.offset().begin));
  }

  return res;
}

}  // namespace SBG
