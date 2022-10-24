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

#include <sbg/atom_pw_map.hpp>

namespace SBG {

// Atomic piecewise linear maps -------------------------------------------------------------------

APW_TEMPLATE
APW_TEMP_TYPE::AtomPWLMapImp1() : dom_(), lmap_() {}

APW_TEMPLATE
APW_TEMP_TYPE::AtomPWLMapImp1(MI_IMP dom, LM_IMP lmap) : dom_(), lmap_() 
{
  if (dom.ndim() == lmap.ndim()) {
    bool incompatible = false;

    parallel_foreach3 (lmap.gain_ref(), lmap.offset_ref(), dom.inters_ref()) {
      REAL_IMP g = boost::get<0>(items), o = boost::get<1>(items);
      INTER_IMP i = boost::get<2>(items);

      REAL_IMP auxLo = i.lo() * g + o;
      REAL_IMP auxStep = i.step() * g;
      REAL_IMP auxHi = i.hi() * g + o;

      if (g < Inf) {
        if (auxLo != (int)auxLo && i.lo()) incompatible = true;

        if (auxStep != (int)auxStep && i.step()) incompatible = true;

        if (auxHi != (int)auxHi && i.hi()) incompatible = true;
      }
    }

    if (!incompatible) {
      set_dom(dom);
      set_lmap(lmap);
    }
  }
}

APW_TEMPLATE
APW_TEMP_TYPE::AtomPWLMapImp1(MI_IMP dom, MI_IMP image) : dom_(dom), lmap_()
{
  if (!dom.empty()) {
    OrdReals g;
    OrdRealsIt itg = g.begin();
    OrdReals o;
    OrdRealsIt ito = o.begin();

    parallel_foreach2 (dom.inters_ref(), image.inters_ref()) {
      INTER_IMP d = boost::get<0>(items), im = boost::get<1>(items);

      if (d.card() == im.card()) {
        REAL_IMP transformGain = im.step() / d.step();
        REAL_IMP transformOff = im.lo() - transformGain * d.lo();

        itg = g.insert(itg, transformGain);
        ++itg;
        ito = o.insert(ito, transformOff);
        ++ito;
      }

      else
        break;
    }

    set_lmap(LM_IMP(g, o));
  }
}

member_imp_temp(APW_TEMPLATE, APW_TEMP_TYPE, MI_IMP, dom);
member_imp_temp(APW_TEMPLATE, APW_TEMP_TYPE, LM_IMP, lmap);

APW_TEMPLATE
bool APW_TEMP_TYPE::empty() { return dom_ref().empty() || lmap_ref().empty(); }

APW_TEMPLATE
bool APW_TEMP_TYPE::isId()
{
  if (lmap_ref().isId()) return true;

  return image() == dom();
}

APW_TEMPLATE
MI_IMP APW_TEMP_TYPE::image(MI_IMP as)
{
  Intervals res;
  IntervalsIt itres = res.begin();

  Intervals inters = (as.cap(dom())).inters();

  if (dom_ref().empty()) return MI_IMP();

  parallel_foreach3 (lmap_ref().gain_ref(), lmap_ref().offset_ref(), inters) {
    REAL_IMP g = boost::get<0>(items), o = boost::get<1>(items);
    INTER_IMP capi = boost::get<2>(items);

    INT_IMP newLo;
    INT_IMP newStep;
    INT_IMP newHi;

    REAL_IMP auxLo = capi.lo() * g + o;
    REAL_IMP auxStep = capi.step() * g;
    REAL_IMP auxHi = capi.hi() * g + o;

    if (g < 0) {
      auxLo = capi.hi() * g + o;
      auxStep = capi.step() * (-g);
      auxHi = capi.lo() * g + o;
    }

    if (auxLo == auxHi) auxStep = 1;

    if (g < Inf) {
      if (auxLo >= Inf)
        newLo = Inf;
      else
        newLo = (INT_IMP)auxLo;

      if (auxStep >= Inf)
        newStep = Inf;
      else
        newStep = (INT_IMP)auxStep;

      if (auxHi >= Inf)
        newHi = Inf;
      else
        newHi = (INT_IMP)auxHi;
    }

    else {
      newLo = 1;
      newStep = 1;
      newHi = Inf;
    }

    itres = res.insert(itres, INTER_IMP(newLo, newStep, newHi));
    ++itres;
  }

  return MI_IMP(res);
}

APW_TEMPLATE
MI_IMP APW_TEMP_TYPE::image() { return image(dom()); }

APW_TEMPLATE
MI_IMP APW_TEMP_TYPE::preImage(MI_IMP as)
{
  MI_IMP fullIm = image(dom());
  MI_IMP actualIm = fullIm.cap(as);

  if (!actualIm.empty()) {
    AtomPWLMapImp1 inv(actualIm, lmap_ref().invLMap());

    MI_IMP aux = inv.image(actualIm);
    return dom_ref().cap(aux);
  }

  else
    return MI_IMP();
}

APW_TEMPLATE
bool APW_TEMP_TYPE::operator==(const APW_TEMP_TYPE &other) const { return dom() == other.dom() && lmap() == other.lmap(); }

template struct AtomPWLMapImp1<OrdCT, LMap, MultiInterval, Interval, INT, REAL>;

APW_TEMPLATE
std::ostream &operator<<(std::ostream &out, const APW_TEMP_TYPE &atompw)
{
  out << "(" << atompw.dom() << ", " << atompw.lmap() << ")";
  return out;
}

template std::ostream &operator<<(std::ostream &out, const AtomPWLMap &atompw);

}  // namespace SBG
