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
  return true;
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
PWMap<Set>::PWMap(Set s) : maps_() {
  if (!isEmpty(s)) {
    unsigned int dims = s[0].size();
    SBGMap<Set> sbg_map(s, Exp(dims));
    maps_ref().emplace(sbg_map);
  }
}
template<typename Set>
PWMap<Set>::PWMap(SBGMap<Set> sm) : maps_() { 
  if (!isEmpty(sm.dom()))
    maps_ref().emplace(sm);
}
template<typename Set>
PWMap<Set>::PWMap(MapSet<Set> maps) : maps_(maps) {}

member_imp_temp(template<typename Set>, PWMap<Set>, MapSet<Set>, maps);

template<typename Set>
typename PWMap<Set>::iterator PWMap<Set>::begin() { return maps_ref().begin(); }
template<typename Set>
typename PWMap<Set>::iterator PWMap<Set>::end() { return maps_ref().end(); }

template<typename Set>
std::size_t PWMap<Set>::size() { return maps_ref().size(); }

template<typename Set>
void PWMap<Set>::emplace(SBGMap<Set> m) { if (!isEmpty(m.dom())) maps_ref().emplace(m); }
template<typename Set>
void PWMap<Set>::emplace_hint(MapSetIt<Set> it, SBGMap<Set> m) { if(!isEmpty(m.dom())) maps_ref().emplace_hint(it, m); }
template<typename Set>
void PWMap<Set>::emplaceBack(SBGMap<Set> m) { maps_ref().emplace_hint(maps_ref().end(), m); }
template<typename Set>
SBGMap<Set> &PWMap<Set>::operator[](std::size_t n) {
  auto first = maps_ref().begin();
  for (unsigned int j = 0; j < n; j++) first++;
  return *first;
}

template<typename Set>
PWMap<Set> PWMap<Set>::operator+(const PWMap<Set> &pw)
{
  PWMap<Set> res;

  PWMap<Set> aux1 = *this, aux2 = pw;
  BOOST_FOREACH (SBGMap<Set> sbgmap1, aux1.maps()) {
    BOOST_FOREACH (SBGMap<Set> sbgmap2, aux2.maps()) {
      Set ith_dom = intersection(sbgmap1.dom(), sbgmap2.dom());
      Exp ith_exp = sbgmap1.exp() + sbgmap2.exp();
      if (!isEmpty(ith_dom)) res.emplaceBack(SBGMap<Set>(ith_dom, ith_exp));
    }
  }

  return res;
}

template<typename Set>
bool PWMap<Set>::operator==(const PWMap &other) const 
{
  if (!(dom(*this) == dom(other))) return false;

  if (maps() == other.maps()) return true;

  BOOST_FOREACH (SBGMap sbgmap1, maps()) {
    BOOST_FOREACH (SBGMap sbgmap2, other.maps()) {
      Set cap_dom = intersection(sbgmap1.dom_ref(), sbgmap2.dom_ref());

      if (!isEmpty(cap_dom)) {
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
  PWMap<Set> res;

  BOOST_FOREACH (SBGMap sbgmap, pw) res.emplaceBack(restrict(subdom, sbgmap));

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
PWMap<Set> composition(PWMap<Set> pw1, PWMap<Set> pw2)
{
  PWMap<Set> res;

  Set im = image(pw2), new_dom = preImage(im, pw2);
  PWMap map2 = restrict(new_dom, pw2);

  PWMap<Set> aux1 = normalize(pw1), aux2 = normalize(map2);
  //PWMap<Set> aux1 = pw1, aux2 = map2;
  BOOST_FOREACH (SBGMap sbgmap1, aux1) {
    BOOST_FOREACH (SBGMap sbgmap2, aux2) {
      SBGMap ith = composition(sbgmap1, sbgmap2);
        if (!isEmpty(ith.dom_ref())) 
          res.emplaceBack(ith);
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> mapInf(PWMap<Set> pw, unsigned int n)
{
  PWMap<Set> res = pw, old_res;

  Set pw_dom = dom(pw);
  if (!isEmpty(pw_dom)) {
    for (unsigned int j = 0; !(old_res == res) && j < n; j++) {
      old_res = res;
      res = composition(res, pw);
    }

    if (old_res == res) return res;

    res = reduce(res);
    do {
      old_res = res;
      res = composition(res, res);

      res = reduce(res);
    } while (old_res != res);
  }

  return res;
}

template<typename Set>
PWMap<Set> mapInf(PWMap<Set> pw) { return mapInf(pw, 0); }

// Extra operations ------------------------------------------------------------

// !!! pw1 and pw2 should be domain-disjoint
template<typename Set>
PWMap<Set> concatenation(PWMap<Set> pw1, PWMap<Set> pw2)
{
  MapSet<Set> res;

  BOOST_FOREACH (SBGMap sbgmap1, pw1.maps()) res.emplace_hint(res.end(), sbgmap1);
  BOOST_FOREACH (SBGMap sbgmap2, pw2.maps()) res.emplace_hint(res.end(), sbgmap2);

  return PWMap(res);
}

template<typename Set>
PWMap<Set> combine(PWMap<Set> pw1, PWMap<Set> pw2)
{
  PWMap res = pw1;

  if (isEmpty(pw1)) return pw2;

  if (isEmpty(pw2)) return pw1;

  Set dom1 = dom(pw1);
  BOOST_FOREACH (SBGMap sbgmap2, pw2) {
    Set dom2 = sbgmap2.dom(), new_dom = difference(dom2, dom1);
    res.emplaceBack(SBGMap(new_dom, sbgmap2.exp()));
  }

  return res;
}

template<typename Set>
PWMap<Set> reduce(Interval i, LExp le)
{
  PWMap<Set> res;

  if (!isEmpty(i)) {
    Util::RATIONAL zero(0, 1);
    if (isOne(le.slope()) && !isZero(le.offset())) {
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
            res.emplaceBack(kth_map);
          }
        }
      }
    }

    else res.emplaceBack(SBGMap<Set>(i, le));
  }

  return res;
}

template<typename Set>
PWMap<Set> reduce(SBGMap<Set> sbgmap)
{
  PWMap<Set> res;

  Set d = sbgmap.dom(), not_reduced;
  Exp e = sbgmap.exp(); 
  BOOST_FOREACH (SetPiece dom_piece, d) {
    if (isEmpty(dom_piece)) return res;

    if (dom_piece.size() == e.size()) {
      unsigned int j = 0;
      SetPiece aux_piece = dom_piece;
      Exp aux_exp = e;
      bool was_reduced = false;
      parallel_foreach2 (dom_piece.intervals_ref(), e.exps_ref()) {
        Interval i = boost::get<0>(items);
        LExp le = boost::get<1>(items);

        PWMap reduced = reduce<Set>(i, le);
        BOOST_FOREACH (SBGMap ith_reduced, reduced) {
          aux_piece[j] = ith_reduced.dom()[0][0];
          aux_exp[j] = ith_reduced.exp()[0];
          if (aux_piece != dom_piece || !(aux_exp == e)) {
            res.emplaceBack(SBGMap<Set>(aux_piece, aux_exp));
            was_reduced = true;
          }

          aux_piece = dom_piece;
          aux_exp = e;
        }

        ++j;
      }

      if (!was_reduced) not_reduced = concatenation(not_reduced, dom_piece);
    }

    else {
      Util::ERROR("LIB::PWMap::reduce: dimensions don't match");
      return PWMap<Set>();
    }
  }

  res.emplaceBack(SBGMap<Set>(not_reduced, e)); // Add unreduced submaps

  return res;
}

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

template<typename Set>
PWMap<Set> minMap(Interval i, LExp le1, LExp le2, LExp le3, LExp le4)
{
  PWMap<Set> res;

  LExp le21 = composition(le1, le2), le34 = composition(le4, le3);
  Util::RATIONAL m21 = le21.slope(), h21 = le21.offset(), m34 = le34.slope(), h34 = le34.offset();

  // Parallel lines
  if (m21 == m34) {
    if (h21 < h34) res.emplaceBack(SBGMap<Set>(i, le2));

    else res.emplaceBack(SBGMap<Set>(i, le3));

    return res;
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
    after = Interval(before.end() + st, st, end);
    after = intersection(after, i);
  }

  // Before intersection
  if (!isEmpty(before)) {
    if (m34 > m21) res.emplaceBack(SBGMap<Set>(before, le3));

    else res.emplaceBack(SBGMap<Set>(before, le2));
  }

  // After intersection
  if (!isEmpty(after)) {
    if (m21 > m34) res.emplaceBack(SBGMap<Set>(after, le3));

    else res.emplaceBack(SBGMap<Set>(after, le2));
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
    SetPiece aux_dom = dom_piece;

    int j = 0;
    
    if (dom_piece.size() == e1.size() && e1.size() == e2.size() && e2.size() == e3.size() && e3.size() == e4.size()) {
      parallel_foreach5 (dom_piece.intervals_ref(), e1.exps_ref(), e2.exps_ref(), e3.exps_ref(), e4.exps_ref()) { 
        Interval i = boost::get<0>(items);
        LExp le1 = boost::get<1>(items), le2 = boost::get<2>(items);
        LExp le3 = boost::get<3>(items), le4 = boost::get<4>(items);
        PWMap ith = minMap<Set>(i, le1, le2, le3, le4);

        if (!isEmpty(ith)) {
          BOOST_FOREACH (SBGMap<Set> sbgmap, ith) {
            aux_dom[j] = sbgmap.dom()[0][0];
            Exp e = sbgmap.exp();
             
            SBGMap<Set> new_ith;
            if (e == le2) new_ith = SBGMap<Set>(aux_dom, e2);
            else new_ith = SBGMap<Set>(aux_dom, e3);

            res.emplaceBack(new_ith);
          }
        }
        
        aux_dom = dom_piece;
        j++;
      }
    }

    return res;
  }

  Util::ERROR("LIB::PWMap::minMap: dimensions don't match");
  return PWMap<Set>();
}

template<typename Set>
PWMap<Set> minMap(SetPiece dom_piece, Exp e1, Exp e2)
{
  Exp id(e1.size());
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
      if (sbgmap.exp() == e2) d2 = cup(d2, sbgmap.dom()); 

      else d3 = cup(d3, sbgmap.dom());
    }
  }

  if (!isEmpty(d2)) res.emplaceBack(SBGMap(d2, e2));

  if (!isEmpty(d3)) res.emplaceBack(SBGMap(d3, e3));

  return res;
}

template<typename Set>
PWMap<Set> minMap(Set dom, Exp e1, Exp e2)
{
  Exp id(e1.size());
  return minMap(dom, id, e1, e2, id);
}

template<typename Set>
PWMap<Set> minMap(PWMap<Set> pw1, PWMap<Set> pw2, PWMap<Set> pw3, PWMap<Set> pw4)
{
  PWMap<Set> res;
  Set dom1 = dom(pw1), dom2 = dom(pw2), dom3 = dom(pw3), dom4 = dom(pw4);
  Set dom21 = preImage(dom1, pw2), dom34 = preImage(dom4, pw3);
  Set cap_dom23 = intersection(dom2, dom3), cap_dom14 = intersection(dom21, dom34);
  Set cap_dom = intersection(cap_dom23, cap_dom14);

  BOOST_FOREACH (SetPiece mdi, cap_dom) {
    Set mdi_set(mdi);
    bool flag1 = false, flag2 = false, flag3 = false, flag4 = false;
    Exp e1, e2, e3, e4;

    Set im2;
    BOOST_FOREACH (SBGMap<Set> sbgmap2, pw2.maps()) {
      Set d2 = sbgmap2.dom(), s2 = intersection(d2, mdi_set);
      if (!isEmpty(s2)) {
        e2 = sbgmap2.exp();
        flag2 = true;
        im2 = image(SBGMap<Set>(d2, e2));
        break;
      }
    }

    BOOST_FOREACH (SBGMap<Set> sbgmap1, pw1.maps()) {
      Set s1 = intersection(sbgmap1.dom(), im2);
      if (!isEmpty(s1)) {
        e1 = sbgmap1.exp();
        flag1 = true;
        break;
      }
    }

    Set im3;
    BOOST_FOREACH (SBGMap<Set> sbgmap3, pw3.maps()) {
      Set d3 = sbgmap3.dom(), s3 = intersection(d3, mdi_set);
      if (!isEmpty(s3)) {
        e3 = sbgmap3.exp();
        flag3 = true;
        im3 = image(SBGMap<Set>(d3, e3));
        break;
      }
    }

    BOOST_FOREACH (SBGMap<Set> sbgmap4, pw4.maps()) {
      Set s4 = intersection(sbgmap4.dom(), im3);
      if (!isEmpty(s4)) {
        e4 = sbgmap4.exp();
        flag4 = true;
        break;
      }
    }

    if (flag1 && flag2 && flag3 && flag4) {
        PWMap ith = minMap(mdi_set, e1, e2, e3, e4);
        res = combine(ith, res);
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> minMap(PWMap<Set> pw1, PWMap<Set> pw2)
{
  if (isEmpty(pw1) || isEmpty(pw2)) return PWMap<Set>();

  unsigned int nmbr_dims = nmbrDims(pw1);
  Exp id(nmbr_dims);
  SBGMap<Set> aux1(image(pw1), id), aux2(image(pw2), id);
  PWMap<Set> id_map1(aux1), id_map2(aux2);

  PWMap<Set> res = minMap<Set>(id_map1, pw1, pw2, id_map2);

  return minMap<Set>(id_map1, pw1, pw2, id_map2);
}

template<typename Set>
PWMap<Set> minAdjMap(PWMap<Set> pw1, PWMap<Set> pw2, PWMap<Set> pw3)
{
  PWMap<Set> res;

  Set partitioned_dom3 = preImage(image(pw3), pw3);
  Set partitioned_dom = preImage(partitioned_dom3, pw2);
  partitioned_dom = intersection(dom(pw1), partitioned_dom);
  partitioned_dom = intersection(dom(pw2), partitioned_dom);

  Set visited;
  BOOST_FOREACH (SetPiece mdi, partitioned_dom) {
    Set dom_res, mdi_set(mdi);
    Exp e_res, e1;

    // Find mdi in pw1 dom
    BOOST_FOREACH (SBGMap<Set> sbgmap1, pw1) {
      Set dom1 = sbgmap1.dom(), s1 = intersection(dom1, mdi_set);
      if (!isEmpty(s1)) {
        dom_res = image(s1, sbgmap1);
        e1 = sbgmap1.exp();
      }
    }

    // Find mdi in pw2 dom
    BOOST_FOREACH (SBGMap<Set> sbgmap2, pw2) {
      Set dom2 = sbgmap2.dom(), s2 = intersection(dom2, mdi_set);
      if (!isEmpty(s2)) {
        Set im2 = image(s2, sbgmap2);
        if (!isConstant(e1)) e_res = composition(sbgmap2.exp(), inverse(e1));

        else {
          BOOST_FOREACH (SBGMap<Set> sbgmap3, pw3) {
            Set dom3 = sbgmap3.dom(), s3 = intersection(dom3, im2);
            if (!isEmpty(s3)) { 
              if (!isConstant(sbgmap3.exp())) {
                Set min3(minElem(image(s3, sbgmap3)));
                MD_NAT min2 = minElem(preImage(min3, sbgmap3));
                e_res = MDLExp(min2);
              }

              else e_res = MDLExp(minElem(im2));
            }
          }
        }
      }
    }

    SBGMap<Set> ith(dom_res, e_res);
    PWMap<Set> ith_pw(ith);
    Set again = intersection(dom_res, visited);
    if (!isEmpty(again)) {
      PWMap r_res = restrict(again, res), r_map = restrict(again, ith_pw);
      res = combine(minMap(res, ith_pw), res);
      res = combine(res, ith_pw); 
    }

    else res.emplaceBack(ith);

    visited = cup(visited, dom_res);
  }

  return res;
}

template<typename Set>
PWMap<Set> minAdjMap(PWMap<Set> pw1, PWMap<Set> pw2)
{
  Set first_dom = pw2.maps().begin()->dom();
  unsigned int nmbr_dims = first_dom.begin()->size();
  Exp id(nmbr_dims);
  SBGMap<Set> aux2(image(pw2), id);
  PWMap<Set> id_map2(aux2);

  return minAdjMap(pw1, pw2, id_map2);
}

template<typename Set>
PWMap<Set> minInv(Set d, PWMap<Set> pw)
{
  PWMap<Set> res;

  if (!isEmpty(pw)) {
    SBGMap<Set> first = *(pw.maps_ref().begin());
    res.emplaceBack(minInv(d, first));
    BOOST_FOREACH (SBGMap<Set> sbgmap, pw) {
      SBGMap<Set> ith = minInv(d, sbgmap);
      Set cap_dom = intersection(ith.dom(), dom(res));

      if (!isEmpty(cap_dom)) {
        PWMap<Set> min = minMap(res, PWMap<Set>(ith));
        res = combine(min, res);
        Set diff = difference(ith.dom(), dom(res));
        res.emplaceBack(SBGMap<Set>(diff, ith.exp()));
      }

      else res.emplaceBack(ith);
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> minInv(PWMap<Set> pw) { return minInv(dom(pw), pw); }

template<typename Set>
PWMap<Set> filterMap(bool (*f)(SBGMap<Set>), PWMap<Set> pw)
{
  PWMap<Set> res;

  BOOST_FOREACH (SBGMap<Set> sbgmap, pw.maps())
    if (f(sbgmap)) res.emplaceBack(sbgmap);

  return res;
}

template<typename Set>
Set equalImage(PWMap<Set> pw1, PWMap<Set> pw2)
{
  Set res;

  BOOST_FOREACH (SBGMap<Set> sbgmap1, pw1.maps()) {
    BOOST_FOREACH (SBGMap<Set> sbgmap2, pw2.maps()) {
      Set cap_dom = intersection(sbgmap1.dom(), sbgmap2.dom());
      if (!isEmpty(cap_dom)) {
        SBGMap<Set> map1(cap_dom, sbgmap1.exp()), map2(cap_dom, sbgmap2.exp());
        if (map1 == map2) res = cup(res, cap_dom);
      }
    }
  }

  return res; 
}

template<typename Set>
PWMap<Set> offsetDom(Util::MD_NAT off, PWMap<Set> pw)
{
  PWMap<Set> res;

  BOOST_FOREACH (SBGMap<Set> sbgmap, pw) {
    SBGMap<Set> ith(offset(off, sbgmap.dom()), sbgmap.exp());
    res.emplaceBack(ith);
  }

  return res;
}

template<typename Set>
PWMap<Set> offsetDom(PWMap<Set> off, PWMap<Set> pw)
{
  PWMap<Set> res;

  BOOST_FOREACH (SBGMap<Set> sbgmap, pw) {
    Set ith_dom = image(sbgmap.dom(), off);
    res.emplaceBack(SBGMap<Set>(ith_dom, sbgmap.exp()));
  }

  return res;
}

template<typename Set>
PWMap<Set> offsetImage(Util::MD_NAT off, PWMap<Set> pw)
{
  PWMap<Set> res;

  BOOST_FOREACH (SBGMap<Set> sbgmap, pw) {
    Exp e = sbgmap.exp(), res_e;
    parallel_foreach2 (off, e.exps_ref()) {
      Util::NAT o = boost::get<0>(items);
      LExp lexp = boost::get<1>(items);
      LExp res_lexp(lexp.slope(), lexp.offset() + (Util::RATIONAL) o);
      res_e.emplaceBack(res_lexp);
    }

    res.emplaceBack(SBGMap<Set>(sbgmap.dom(), res_e));
  }

  return res;
}

template<typename Set>
PWMap<Set> offsetImage(Exp off, PWMap<Set> pw)
{
  PWMap<Set> res;

  BOOST_FOREACH (SBGMap<Set> sbgmap, pw) {
    SBGMap<Set> ith(sbgmap.dom(), off + sbgmap.exp());
    res.emplaceBack(ith);
  }

  return res;
}

// Function should be called on a non-empty pw
template<typename Set>
unsigned int nmbrDims(PWMap<Set> pw) { return nmbrDims(pw[0]); }

template<typename Set>
PWMap<Set> normalize(PWMap<Set> pw)
{
  PWMap<Set> res;

  Set visited;
  BOOST_FOREACH (SBGMap<Set> sbgmap1, pw) {
    if (isEmpty(intersection(sbgmap1.dom(), visited))) {
      Set ith(sbgmap1.dom());
      BOOST_FOREACH (SBGMap<Set> sbgmap2, pw) 
        if (sbgmap1.exp() == sbgmap2.exp()) cup(ith, sbgmap2.dom()); 

      visited = cup(visited, ith);
      res.emplaceBack(SBGMap<Set>(ith, sbgmap1.exp()));
    }
  }

  return res;
}

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
template BasePWMap reduce<UnordSet>(BaseMap sbgmap);
template BasePWMap reduce<UnordSet>(BasePWMap pw);
template BasePWMap minAdjMap<UnordSet>(BasePWMap pw1, BasePWMap pw2, BasePWMap pw3);
template BasePWMap minAdjMap<UnordSet>(BasePWMap pw1, BasePWMap pw2);
template BasePWMap minInv<UnordSet>(UnordSet im, BasePWMap pw);
template BasePWMap minInv<UnordSet>(BasePWMap pw);
template BasePWMap filterMap<UnordSet>(bool (*f)(BaseMap), BasePWMap pw);
template UnordSet equalImage<UnordSet>(BasePWMap pw1, BasePWMap pw2);
template BasePWMap offsetDom<UnordSet>(Util::MD_NAT off, BasePWMap pw);
template BasePWMap offsetDom<UnordSet>(BasePWMap off, BasePWMap pw);
template BasePWMap offsetImage<UnordSet>(Util::MD_NAT off, BasePWMap pw);
template BasePWMap offsetImage<UnordSet>(Exp off, BasePWMap pw);
template unsigned int nmbrDims<UnordSet>(BasePWMap pw);
template BasePWMap normalize<UnordSet>(BasePWMap pw);

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
template CanonPWMap reduce<OrdSet>(CanonMap sbgmap);
template CanonPWMap reduce<OrdSet>(CanonPWMap pw);
template CanonPWMap minAdjMap<OrdSet>(CanonPWMap pw1, CanonPWMap pw2, CanonPWMap pw3);
template CanonPWMap minAdjMap<OrdSet>(CanonPWMap pw1, CanonPWMap pw2);
template CanonPWMap minInv<OrdSet>(OrdSet im, CanonPWMap pw);
template CanonPWMap minInv<OrdSet>(CanonPWMap pw);
template CanonPWMap filterMap<OrdSet>(bool (*f)(CanonMap), CanonPWMap pw);
template OrdSet equalImage<OrdSet>(CanonPWMap pw1, CanonPWMap pw2);
template CanonPWMap offsetDom<OrdSet>(Util::MD_NAT off, CanonPWMap pw);
template CanonPWMap offsetDom<OrdSet>(CanonPWMap off, CanonPWMap pw);
template CanonPWMap offsetImage<OrdSet>(Util::MD_NAT off, CanonPWMap pw);
template CanonPWMap offsetImage<OrdSet>(Exp off, CanonPWMap pw);
template unsigned int nmbrDims<OrdSet>(CanonPWMap pw);
template CanonPWMap normalize<OrdSet>(CanonPWMap pw);

} // namespace LIB

} // namespace SBG
