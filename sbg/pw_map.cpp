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

#include "sbg/pw_map.hpp"

namespace SBG {

namespace LIB {

// Type definitions ------------------------------------------------------------

template<typename Set>
LTMap<Set>::LTMap() {}

template<typename Set>
bool LTMap<Set>::operator()(const SBGMap<Set> &x, const SBGMap<Set> &y) const
{
  if (x.exp() < y.exp()) return true;
  
  return x.dom() < y.dom();
}

template<typename Set>
std::ostream &operator<<(std::ostream &out, const MapSet<Set> &ms)
{
  MapSet<Set> aux = ms;
  int sz = aux.size();

  out << "<<";
  if (sz > 0) {
    auto it = aux.begin();
    for (int i = 0; i < sz - 1; ++i) {
      if (!isEmpty(it->dom_ref()))
        out << *it << ", ";
      ++it;
    }
    if (!isEmpty(it->dom_ref()))
      out << *it;
  }
  out << ">>";

  return out;
}

// PWMap -----------------------------------------------------------------------

template<typename Set>
PWMap<Set>::PWMap() : maps_() {}
template<typename Set>
PWMap<Set>::PWMap(SBGMap<Set> sm) : maps_() { maps_ref().emplace(sm); }
template<typename Set>
PWMap<Set>::PWMap(MapSet<Set> maps) : maps_(maps) {}

member_imp_temp(template<typename Set>, PWMap<Set>, MapSet<Set>, maps);

template<typename Set>
std::size_t PWMap<Set>::size() { return maps().size(); }

template<typename Set>
void PWMap<Set>::emplace(SBGMap<Set> m) { maps_ref().emplace(m); }
template<typename Set>
void PWMap<Set>::emplace_hint(MapSetIt<Set> it, SBGMap<Set> m) { maps_ref().emplace_hint(it, m); }

template<typename Set>
typename PWMap<Set>::iterator PWMap<Set>::begin() { return maps_ref().begin(); }
template<typename Set>
typename PWMap<Set>::iterator PWMap<Set>::end() { return maps_ref().end(); }

template<typename Set>
bool PWMap<Set>::operator==(const PWMap &other) const 
{
  if (!(dom(*this) == dom(other))) return false;

  if (maps() == other.maps()) return true;

  BOOST_FOREACH (SBGMap sbgmap1, maps()) {
    BOOST_FOREACH (SBGMap sbgmap2, other.maps()) {
      Set cap_dom = intersection(sbgmap1.dom_ref(), sbgmap2.dom_ref());

      // Here we check by image because the same image can be obtained through
      // two different lexps.
      // Example: [1:1:1] -> 10 and [1:1:1] -> x+9
      if (cardinal(cap_dom) == 1) {
        SBGMap map1(cap_dom, sbgmap1.exp()), map2(cap_dom, sbgmap2.exp());
        if (!(image(map1) == image(map2))) return false;
      }

      // When there is more than one element we can't check equality on the
      // image because there are at least two linear maps with the same
      // domain and image.
      // Example: [1:1:10] -> x and [1:1:10] -> -x+10
      else {
        if (!(sbgmap1.exp() == sbgmap2.exp())) return false;
      }
    }
  }

  return true;
}

template<typename Set>
bool PWMap<Set>::operator!=(const PWMap &other) const { return !(*this == other); }

template<typename Set>
std::ostream &operator<<(std::ostream &out, const PWMap<Set> &pw)
{
  out << pw.maps();

  return out;
}

// PWMap functions -------------------------------------------------------------

template<typename Set>
bool isEmpty(PWMap<Set> pw) { return pw.maps().empty(); }

template<typename Set>
Set dom(PWMap<Set> pw) 
{
  Set res;

  BOOST_FOREACH (SBGMap<Set> sbgmap, pw)
    res = concatenation(res, sbgmap.dom());

  return res;
}

template<typename Set>
PWMap<Set> restrict(Set subdom, PWMap<Set> pw)
{
  MapSet<Set> res;

  BOOST_FOREACH (SBGMap sbgmap, pw)
    res.emplace(restrict(subdom, sbgmap));

  return PWMap(res);
}

template<typename Set>
Set image(PWMap<Set> pw)
{
  Set res;

  BOOST_FOREACH (SBGMap sbgmap, pw) {
    Set ith = image(sbgmap);
    res = cup(res, ith);
  } 

  return res;
}

template<typename Set>
Set image(Set subdom, PWMap<Set> pw)
{
  PWMap restricted = restrict(subdom, pw);
  return image(restricted);
}

template<typename Set>
Set preImage(PWMap<Set> pw) { return dom(pw); }

template<typename Set>
Set preImage(Set subcodom, PWMap<Set> pw)
{
  Set res;

  BOOST_FOREACH (SBGMap sbgmap, pw) {
    Set ith = preImage(subcodom, sbgmap);
    res = cup(res, ith);
  }

  return res;
}

template<typename Set>
PWMap<Set> composition(PWMap<Set> pwi1, PWMap<Set> pwi2)
{
  PWMap<Set> res;

  Set im = image(pwi2), new_dom = preImage(im, pwi2);
  PWMap map2 = restrict(new_dom, pwi2);

  BOOST_FOREACH (SBGMap sbgmap1, pwi1) {
    BOOST_FOREACH (SBGMap sbgmap2, map2) {
      SBGMap ith = composition(sbgmap1, sbgmap2);
        if (!isEmpty(ith.dom_ref())) 
          res.emplace(ith);
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> mapInf(PWMap<Set> pw)
{
  PWMap res = pw;

  Set pw_dom = dom(pw);
  if (!isEmpty(pw_dom)) {
    PWMap og_res = res, old_res = res;

    res = reduce(res);
    do {
      old_res = res;
      res = composition(og_res, res);

      res = reduce(res);
    } while (old_res != res);
  }

  return res;
}

// Extra operations ------------------------------------------------------------

// !!! pw1 and pw2 should be domain-disjoint
template<typename Set>
PWMap<Set> concatenation(PWMap<Set> pw1, PWMap<Set> pw2)
{
  MapSetIt<Set> beg = pw1.maps_ref().begin(), end = pw1.maps_ref().end();
  MapSet<Set> res(beg, end); 

  BOOST_FOREACH (SBGMap sbgmap2, pw2.maps()) res.emplace(sbgmap2);

  return PWMap(res);
}

template<typename Set>
PWMap<Set> combine(PWMap<Set> pw1, PWMap<Set> pw2)
{
  PWMap res = pw1;

  if (isEmpty(pw1) || isEmpty(pw2)) return res;

  Set dom1 = dom(pw1);
  BOOST_FOREACH (SBGMap sbgmap2, pw2) {
    Set dom2 = sbgmap2.dom(), new_dom = difference(dom2, dom1);
    res.emplace_hint(res.end(), SBGMap(new_dom, sbgmap2.exp()));
  }

  return res;
}

template<typename Set>
PWMap<Set> minMap(Interval i, LExp le1, LExp le2, LExp le3, LExp le4)
{
  PWMap<Set> res;

  LExp le21 = composition(le1, le2), le34 = composition(le4, le3);
  Util::RATIONAL m21 = le21.slope(), h21 = le21.offset(), m34 = le34.slope(), h34 = le34.offset();

  // Parallel lines
  if (m21 == m34) {
    if (h21 < h34) res.emplace(SBGMap<Set>(i, le2));

    else res.emplace(SBGMap<Set>(i, le3));
  }

  // Non parallel lines
  Util::RATIONAL cap_point = (h34 - h21) / (m21 - m34);
  Interval before, after;
  if (cap_point < 0) after = i;

  else {
    Util::NAT beg = i.begin(), st = i.step(), end = i.end();
    double double_cap_point = cap_point.numerator() / cap_point.denominator();
    before = Interval(beg, st, (Util::NAT) floor(double_cap_point));
    before = intersection(before, i);
    after = Interval((Util::NAT) ceil(double_cap_point), st, end);
    after = intersection(after, i);
  }

  // Before intersection
  if (!isEmpty(before)) {
    if (m34 > m21) res.emplace(SBGMap<Set>(before, le1));

    else res.emplace(SBGMap<Set>(before, le2));
  }

  // After intersection
  if (!isEmpty(after)) {
    if (m21 > m34) res.emplace(SBGMap<Set>(before, le2));

    else res.emplace(SBGMap<Set>(before, le1));
  }

  return res;
}

template<typename Set>
PWMap<Set> minMap(Interval i, LExp le1, LExp le2)
{
  LExp id;
  return minMap<Set>(i, id, le1, le2, id);
}

template<typename Set>
PWMap<Set> minMap(SetPiece dom_piece, Exp e1, Exp e2, Exp e3, Exp e4)
{
  PWMap<Set> res;

  if (!isEmpty(dom_piece)) {
    SetPiece aux = dom_piece;

    int j = 0;
    parallel_foreach5 (dom_piece.intervals(), e1.exps(), e2.exps(), e3.exps(), e4.exps()) { 
      Interval i = boost::get<0>(items);
      LExp le1 = boost::get<1>(items), le2 = boost::get<2>(items);
      LExp le3 = boost::get<3>(items), le4 = boost::get<4>(items);
      PWMap ith = minMap<Set>(i, le1, le2, le3, le4);

      if (!isEmpty(ith)) {
        BOOST_FOREACH (SBGMap<Set> sbgmap, ith) {
          aux[j] = sbgmap.dom()[0][0];
          SBGMap<Set> new_ith(aux, sbgmap.exp());
          res.emplace_hint(res.end(), new_ith);
        }
      }
      
      aux = dom_piece;
      j++;
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> minMap(SetPiece dom_piece, Exp e1, Exp e2)
{
  Exp id(dom_piece.dimensions());
  return minMap<Set>(dom_piece, id, e1, e2, id);
}

template<typename Set>
PWMap<Set> minMap(Set dom, Exp e1, Exp e2, Exp e3, Exp e4)
{
  PWMap<Set> res;
  
  Set d2, d3;

  BOOST_FOREACH (SetPiece dom_piece, dom) {
    PWMap ith = minMap<Set>(dom_piece, e1, e2, e3, e4);
    BOOST_FOREACH (SBGMap sbgmap, ith) {
      if (sbgmap.exp() == e2) d2 = concatenation(d2, sbgmap.dom());

      else d3 = concatenation(d3, sbgmap.dom());
    }
  }

  if (!isEmpty(d2)) res.emplace(SBGMap(d2, e2));

  if (!isEmpty(d3)) res.emplace(SBGMap(d3, e3));

  return res;
}

template<typename Set>
PWMap<Set> minMap(Set dom, Exp e1, Exp e2)
{
  Exp id(dom.dimensions());
  return minMap(dom, id, e1, e2, id);
}

template<typename Set>
PWMap<Set> minMap(PWMap<Set> pw1, PWMap<Set> pw2, PWMap<Set> pw3, PWMap<Set> pw4)
{
  PWMap<Set> res;
  Set dom1 = dom(pw1), dom2 = dom(pw2), dom3 = dom(pw3), dom4 = dom(pw4);
  Set cap12 = intersection(dom1, dom2), cap34 = intersection(dom3, dom4);
  Set cap_dom = intersection(cap12, cap34);

  BOOST_FOREACH (SetPiece mdi, cap_dom) {
    Set mdi_set(mdi);
    bool flag1 = false, flag2 = false, flag3 = false, flag4 = false;
    Exp e1, e2, e3, e4;
    parallel_foreach4 (pw1.maps(), pw2.maps(), pw3.maps(), pw4.maps()) {
      SBGMap sbgmap1 = boost::get<0>(items), sbgmap2 = boost::get<1>(items);
      SBGMap sbgmap3 = boost::get<2>(items), sbgmap4 = boost::get<3>(items);

      Set dom1 = sbgmap1.dom(), dom2 = sbgmap2.dom(), dom3 = sbgmap3.dom(), dom4 = sbgmap4.dom();

      Set s1 = intersection(dom1, mdi_set), s2 = intersection(dom2, mdi_set);
      Set s3 = intersection(dom3, mdi_set), s4 = intersection(dom4, mdi_set);

      if (!isEmpty(s1)) {
        e1 = sbgmap1.exp();
        flag1 = true;
      }
      if (!isEmpty(s2)) {
        e2 = sbgmap2.exp();
        flag2 = true;
      }
      if (!isEmpty(s3)) {
        e3 = sbgmap3.exp();
        flag3 = true;
      }
      if (!isEmpty(s4)) {
        e4 = sbgmap4.exp();
        flag4 = true;
      }

      if (flag1 && flag2 && flag3 && flag4) {
        PWMap ith = minMap(mdi_set, e1, e2, e3, e4);
        res = combine(ith, res);
        break;
      }
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> minMap(PWMap<Set> pw1, PWMap<Set> pw2)
{
  if (isEmpty(pw1) || isEmpty(pw2)) return PWMap<Set>();

  unsigned nmbr_dims = pw1.maps().begin()->dom().dimensions();
  Exp id(nmbr_dims);
  SBGMap<Set> id_map1(dom(pw1), id), id_map2(dom(pw2), id);

  return minMap<Set>(id_map1, pw1, pw2, id_map2);
}

template<typename Set>
PWMap<Set> reduce(Interval i, LExp le)
{
  PWMap<Set> res = PWMap(SBGMap<Set>(i, le));

  if (!isEmpty(i)) {
    Util::RATIONAL zero(0, 1), one(1, 1);
    if (le.slope() == one && le.offset() != zero) {
      Util::INT h = toInt(le.offset()); 

      Util::NAT st = i.step();
      if (h == (Util::INT) st) {
        Util::NAT hi = i.end();
        Util::RATIONAL const_expr(hi + st, 1);
        if (st < Util::Inf - hi) res = PWMap(SBGMap<Set>(i, LExp(zero, const_expr)));
      }

      else if (h == (Util::INT) -st) {
        Util::NAT lo = i.begin(); 
        Util::RATIONAL const_expr(lo - st, 1);
        if (lo >= st) res = PWMap(SBGMap<Set>(i, LExp(zero, const_expr)));
      }

      else if (h % (Util::INT) st == 0) {
        // Is convenient the partition of the piece?
        if ((Util::INT) cardinal(i) > h*h) {
          res = PWMap<Set>();
          Util::INT absh = std::abs(h);

          for (int k = 1; k <= absh; ++k) {
            Util::NAT new_begin = i.begin() + k - 1;
            Interval kth_piece(new_begin, (Util::NAT) absh, i.end());

            LExp kth_exp(0, 0);
            if (h > 0) kth_exp.set_offset(kth_piece.end() + h);

            else kth_exp.set_offset(kth_piece.begin() + h);

            SBGMap<Set> kth_map(kth_piece, kth_exp);
            res.emplace_hint(res.end(), kth_map);  
          }
        }
      }
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> reduce(SetPiece dom_piece, Exp e)
{
  PWMap<Set> res;

  unsigned int j = 0;
  SetPiece aux_piece = dom_piece;
  Exp aux_exp = e;
  parallel_foreach2 (dom_piece.intervals_ref(), e.exps_ref()) {
    Interval i = boost::get<0>(items);
    LExp le = boost::get<1>(items);

    PWMap reduced = reduce<Set>(i, le);
    BOOST_FOREACH (SBGMap ith_reduced, reduced) {
      aux_piece[j] = ith_reduced.dom()[0][0];
      aux_exp[j] = ith_reduced.exp()[0];
      res.emplace_hint(res.end(), SBGMap<Set>(aux_piece, aux_exp));

      aux_piece = dom_piece;
      aux_exp = e;
    }

    ++j;
  }

  return res;
}

//TODO
template<typename Set>
PWMap<Set> reduce(SBGMap<Set> sbgmap)
{
  PWMap<Set> res;

  Set dom = sbgmap.dom();
  BOOST_FOREACH (SetPiece dom_piece, dom) {
    PWMap ith = reduce<Set>(dom_piece, sbgmap.exp());
    res = concatenation(ith, res);
  }

  return res;
}

//TODO
template<typename Set>
PWMap<Set> reduce(PWMap<Set> pw)
{
  PWMap<Set> res;

  BOOST_FOREACH (SBGMap sbgmap, pw.maps()) {
    PWMap ith = reduce(sbgmap);
    res = concatenation(ith, res);
  }

  return res;
}

/*
SBGMap minAdjMap(SBGMap sbgmap1, PWMap pw2)
{
  SBGMap res;

  Set d1 = sbgmap1.dom(), d2 = sbgmap2.dom();
  Exp e1 = sbgmap1.exp(), e2 = sbgmap2.exp();
  if (d1 == d2) {
    Exp e;
    if (isConstant(sbgmap1.exp())) e = Exp(0, minElem(image(sbgmap2)));

    else e = composition(e2, inverse(e1));

    res = SBGMap(d1, e);
  }

  else Util::ERROR("map/minAdjMap: maps should have the same domain");

  return res;
}

PWMap minAdjMap(PWMap pw1, PWMap pw2)
{
  MapSet res;

  Set dom1 = dom(pw1), dom2 = dom(pw2);
  if (dom1 == dom2) {
    BOOST_FOREACH (SBGMap sbgmap1, pw1.maps()) {
      BOOST_FOREACH (SBGMap sbgmap2, pw2.maps()) {
        SBGMap ith_min_adj = minAdjMap(sbgmap1, sbgmap2);
      }
    }
  }

  else Util::ERROR("pw_map/minAdjMap: maps should have the same domain");

  return PWMap(res);
}
*/

// Template instantiations -----------------------------------------------------

template std::ostream &operator<<(std::ostream &out, const MapSet<UnordSet> &ms);
template struct PWMap<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BasePWMap &pw);
template bool isEmpty<UnordSet>(BasePWMap pw);
template UnordSet dom<UnordSet>(BasePWMap pw);
template BasePWMap restrict<UnordSet>(UnordSet subdom, BasePWMap pw);
template UnordSet image<UnordSet>(BasePWMap pw);
template UnordSet image<UnordSet>(UnordSet subdom, BasePWMap pw);
template UnordSet preImage<UnordSet>(BasePWMap pw);
template UnordSet preImage<UnordSet>(UnordSet subcodom, BasePWMap pw);
template BasePWMap mapInf<UnordSet>(BasePWMap pw);
template BasePWMap composition<UnordSet>(BasePWMap pw1, BasePWMap pw2);
template BasePWMap concatenation<UnordSet>(BasePWMap pw1, BasePWMap pw2);
template BasePWMap combine<UnordSet>(BasePWMap pw1, BasePWMap pw2);
template BasePWMap minMap<UnordSet>(Interval i, LExp le1, LExp le2, LExp le3, LExp le4);
template BasePWMap minMap<UnordSet>(Interval i, LExp le1, LExp le2);
template BasePWMap minMap<UnordSet>(SetPiece dom_piece, Exp e1, Exp e2, Exp e3, Exp e4);
template BasePWMap minMap<UnordSet>(SetPiece dom_piece, Exp e1, Exp e2);
template BasePWMap minMap<UnordSet>(UnordSet dom, Exp e1, Exp e2, Exp e3, Exp e4);
template BasePWMap minMap<UnordSet>(UnordSet dom, Exp e1, Exp e2);
template BasePWMap minMap<UnordSet>(BasePWMap pw1, BasePWMap pw2, BasePWMap pw3, BasePWMap pw4);
template BasePWMap minMap<UnordSet>(BasePWMap pw1, BasePWMap pw2);
template BasePWMap reduce<UnordSet>(Interval i, LExp e);
template BasePWMap reduce<UnordSet>(SetPiece dom_piece, Exp e);
template BasePWMap reduce<UnordSet>(BaseMap sbgmap);
template BasePWMap reduce<UnordSet>(BasePWMap pw);
//template BasePWMap minAdjMap<UnordSet>(BasePWMap pw1, BasePWMap pw2);

template std::ostream &operator<<(std::ostream &out, const MapSet<OrdSet> &ms);
template struct PWMap<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonPWMap &pw);
template bool isEmpty<OrdSet>(CanonPWMap pw);
template OrdSet dom<OrdSet>(CanonPWMap pw);
template CanonPWMap restrict<OrdSet>(OrdSet subdom, CanonPWMap pw);
template OrdSet image<OrdSet>(CanonPWMap pw);
template OrdSet image<OrdSet>(OrdSet subdom, CanonPWMap pw);
template OrdSet preImage<OrdSet>(CanonPWMap pw);
template OrdSet preImage<OrdSet>(OrdSet subcodom, CanonPWMap pw);
template CanonPWMap mapInf<OrdSet>(CanonPWMap pw);
template CanonPWMap composition<OrdSet>(CanonPWMap pw1, CanonPWMap pw2);
template CanonPWMap concatenation<OrdSet>(CanonPWMap pw1, CanonPWMap pw2);
template CanonPWMap combine<OrdSet>(CanonPWMap pw1, CanonPWMap pw2);
template CanonPWMap minMap<OrdSet>(Interval i, LExp le1, LExp le2, LExp le3, LExp le4);
template CanonPWMap minMap<OrdSet>(Interval i, LExp le1, LExp le2);
template CanonPWMap minMap<OrdSet>(SetPiece dom_piece, Exp e1, Exp e2, Exp e3, Exp e4);
template CanonPWMap minMap<OrdSet>(SetPiece dom_piece, Exp e1, Exp e2);
template CanonPWMap minMap<OrdSet>(OrdSet dom, Exp e1, Exp e2, Exp e3, Exp e4);
template CanonPWMap minMap<OrdSet>(OrdSet dom, Exp e1, Exp e2);
template CanonPWMap minMap<OrdSet>(CanonPWMap pw1, CanonPWMap pw2, CanonPWMap pw3, CanonPWMap pw4);
template CanonPWMap minMap<OrdSet>(CanonPWMap pw1, CanonPWMap pw2);
template CanonPWMap reduce<OrdSet>(Interval i, LExp e);
template CanonPWMap reduce<OrdSet>(SetPiece dom_piece, Exp e);
template CanonPWMap reduce<OrdSet>(CanonMap sbgmap);
template CanonPWMap reduce<OrdSet>(CanonPWMap pw);
//template CanonPWMap minAdjMap<OrdSet>(CanonPWMap pw1, CanonPWMap pw2);

} // namespace LIB

} // namespace SBG
