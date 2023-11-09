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

std::ostream &operator<<(std::ostream &out, const MapSet &ms)
{
  MapSet aux = ms;
  int sz = aux.size();

  out << "<<";
  if (sz > 0) {
    auto it = aux.begin();
    SBGMap aux_map = *it;
    for (int i = 0; i < sz - 1; ++i) {
      if (!isEmpty(aux_map.dom_ref()))
        out << *it << ", ";
      ++it;
      aux_map = *it;
    }
    if (!isEmpty(aux_map.dom_ref()))
      out << *it;
  }
  out << ">>";

  return out;
}

// PWMap -----------------------------------------------------------------------

PWMap::PWMap() : maps_() {}
PWMap::PWMap(Set s) : maps_() {
  if (!isEmpty(s)) {
    unsigned int dims = s[0].size();
    SBGMap sbg_map(s, Exp(dims));
    maps_ref().emplace(sbg_map);
  }
}
PWMap::PWMap(SBGMap sm) : maps_() {
  if (!isEmpty(sm.dom()))
    maps_ref().emplace(sm);
}
PWMap::PWMap(MapSet maps) : maps_(maps) {
  for (SBGMap sbgmap : maps) emplaceBack(sbgmap);
}

member_imp(PWMap, MapSet, maps);

typename PWMap::iterator PWMap::begin() { return maps_ref().begin(); }
typename PWMap::iterator PWMap::end() { return maps_ref().end(); }

std::size_t PWMap::size() { return maps_ref().size(); }

void PWMap::emplace(SBGMap m) { if (!isEmpty(m.dom())) maps_ref().emplace(m); }
void PWMap::emplace_hint(PWMap::iterator it, SBGMap m) {
  if(!isEmpty(m.dom())) maps_ref().emplace_hint(it, m);
}
void PWMap::emplaceBack(SBGMap m) { emplace_hint(maps_ref().end(), m); }

PWMap PWMap::operator+(const PWMap &pw)
{
  PWMap res;

  PWMap aux1 = *this, aux2 = pw;
  for (SBGMap sbgmap1 : aux1) {
    for (SBGMap sbgmap2 : aux2) {
      Set ith_dom = intersection(sbgmap1.dom(), sbgmap2.dom());
      Exp ith_exp = sbgmap1.exp() + sbgmap2.exp();
      res.emplaceBack(SBGMap(ith_dom, ith_exp));
    }
  }

  return res;
}

bool PWMap::operator==(const PWMap &other) const
{
  if (!(dom(*this) == dom(other))) return false;

  if (maps() == other.maps()) return true;

  for (SBGMap sbgmap1 : maps()) {
    for (SBGMap sbgmap2 : other.maps()) {
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

bool PWMap::operator!=(const PWMap &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &out, const PWMap &pw)
{
  out << pw.maps();

  return out;
}

// PWMap functions -------------------------------------------------------------

bool isEmpty(PWMap pw) { return pw.maps().empty(); }

Set dom(PWMap pw)
{
  Set res;

  if (!isEmpty(pw))
    for (SBGMap sbgmap : pw) res = concatenation(res, sbgmap.dom());

  return res;
}

PWMap restrict(Set subdom, PWMap pw)
{
  PWMap res;

  for (SBGMap sbgmap : pw) res.emplaceBack(restrict(subdom, sbgmap));

  return PWMap(res);
}

Set image(PWMap pw)
{
  Set res;

  for (SBGMap sbgmap : pw) {
    Set ith = image(sbgmap);
    res = cup(res, ith);
  }

  return res;
}

Set image(Set subdom, PWMap pw)
{
  PWMap restricted = restrict(subdom, pw);
  return image(restricted);
}

Set preImage(PWMap pw) { return dom(pw); }

Set preImage(Set subcodom, PWMap pw)
{
  Set res;

  for (SBGMap sbgmap : pw) {
    Set ith = preImage(subcodom, sbgmap);
    res = cup(res, ith);
  }

  return res;
}

PWMap composition(PWMap pw1, PWMap pw2)
{
  PWMap res;

  Set im = image(pw2), new_dom = preImage(im, pw2);
  PWMap map2 = restrict(new_dom, pw2);

  PWMap aux1 = normalize(pw1), aux2 = normalize(map2);
  //PWMap aux1 = pw1, aux2 = map2;
  for (SBGMap sbgmap1 : aux1) {
    for (SBGMap sbgmap2 : aux2) {
      SBGMap ith = composition(sbgmap1, sbgmap2);
        if (!isEmpty(ith.dom_ref())) 
          res.emplaceBack(ith);
    }
  }

  return res;
}

PWMap mapInf(PWMap pw, unsigned int n)
{
  PWMap res = pw, old_res;

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

PWMap mapInf(PWMap pw) { return mapInf(pw, 0); }

// Extra operations ------------------------------------------------------------

// !!! pw1 and pw2 should be domain-disjoint
PWMap concatenation(PWMap pw1, PWMap pw2)
{
  MapSet res;

  for (SBGMap sbgmap1 : pw1) res.emplace_hint(res.end(), sbgmap1);
  for (SBGMap sbgmap2 : pw2) res.emplace_hint(res.end(), sbgmap2);

  return PWMap(res);
}

PWMap combine(PWMap pw1, PWMap pw2)
{
  PWMap res = pw1;

  if (isEmpty(pw1)) return pw2;

  if (isEmpty(pw2)) return pw1;

  Set dom1 = dom(pw1);
  for (SBGMap sbgmap2 : pw2) {
    Set dom2 = sbgmap2.dom(), new_dom = difference(dom2, dom1);
    res.emplaceBack(SBGMap(new_dom, sbgmap2.exp()));
  }

  return res;
}

PWMap reduce(Interval i, LExp le, std::size_t imp)
{
  PWMap res;

  if (!isEmpty(i)) {
    Util::RATIONAL zero(0, 1);
    if (isOne(le.slope()) && !isZero(le.offset())) {
      Util::INT h = toInt(le.offset()); 

      Util::NAT st = i.step();
      if (h == (Util::INT) st) {
        Util::NAT hi = i.end();
        Util::RATIONAL off(hi + st, 1);
        LExp const_expr(zero, off);
        if (st < Util::Inf - hi) res = PWMap(SBGMap(i, const_expr));
      }

      else if (h == (Util::INT) -st) {
        Util::NAT lo = i.begin(); 
        Util::RATIONAL off(lo - st, 1);
        LExp const_expr(zero, off);
        if (lo >= st) res = PWMap(SBGMap(i, const_expr));
      }

      else if (h % (Util::INT) st == 0) {
        // Is convenient the partition of the piece?
        if ((Util::INT) cardinal(i) > h*h) {
          res = PWMap();
          Util::INT absh = std::abs(h);

          for (int k = 1; k <= absh; ++k) {
            Util::NAT new_begin = i.begin() + k - 1;
            Interval kth_piece(new_begin, (Util::NAT) absh, i.end());

            LExp kth_exp(0, 0);
            if (h > 0) kth_exp.set_offset(kth_piece.end() + h);

            else kth_exp.set_offset(kth_piece.begin() + h);

            SBGMap kth_map(kth_piece, kth_exp);
            res.emplaceBack(kth_map);
          }
        }
      }
    }

    else res.emplaceBack(SBGMap(i, le));
  }

  return res;
}

PWMap reduce(SBGMap sbgmap)
{
  PWMap res;

  Set d = sbgmap.dom(), not_reduced;
  std::size_t imp = d.pieces().index();
  Exp e = sbgmap.exp(); 
  auto f = [&](SetPiece dom_piece) {
    if (dom_piece.size() == e.size()) {
      unsigned int j = 0;
      SetPiece aux_piece = dom_piece;
      Exp aux_exp = e;
      bool was_reduced = false;
      parallel_foreach2 (dom_piece.intervals_ref(), e.exps_ref()) {
        Interval i = boost::get<0>(items);
        LExp le = boost::get<1>(items);

        PWMap reduced = reduce(i, le, imp);
        for (SBGMap ith_reduced : reduced) {
          aux_piece[j] = ith_reduced.dom()[0][0];
          aux_exp[j] = ith_reduced.exp()[0];
          if (aux_piece != dom_piece || !(aux_exp == e)) {
            res.emplaceBack(SBGMap(aux_piece, aux_exp));
            was_reduced = true;
          }

          aux_piece = dom_piece;
          aux_exp = e;
        }

        ++j;
      }

      if (!was_reduced) not_reduced.emplace(dom_piece);
    }

    else Util::ERROR("LIB::PWMap::reduce: dimensions don't match");

    return;
  };
  foreach(f, d);

  res.emplaceBack(SBGMap(not_reduced, e)); // Add unreduced submaps

  return res;
}

PWMap reduce(PWMap pw)
{
  PWMap res;

  for (SBGMap sbgmap : pw) {
    PWMap ith = reduce(sbgmap);
    res = concatenation(ith, res);
  }

  return res;
}

PWMap minMap(Interval i, LExp le1, LExp le2, LExp le3, LExp le4)
{
  PWMap res;

  LExp le21 = composition(le1, le2), le34 = composition(le4, le3);
  Util::RATIONAL m21 = le21.slope(), h21 = le21.offset(), m34 = le34.slope(), h34 = le34.offset();

  // Parallel lines
  if (m21 == m34) {
    if (h21 < h34) res.emplaceBack(SBGMap(i, le2));

    else res.emplaceBack(SBGMap(i, le3));

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
    if (m34 > m21) res.emplaceBack(SBGMap(before, le3));

    else res.emplaceBack(SBGMap(before, le2));
  }

  // After intersection
  if (!isEmpty(after)) {
    if (m21 > m34) res.emplaceBack(SBGMap(after, le3));

    else res.emplaceBack(SBGMap(after, le2));
  }

  return res;
}

PWMap minMap(Interval i, LExp le1, LExp le2)
{
  LExp id;
  return minMap(i, id, le1, le2, id);
}

PWMap minMap(SetPiece dom_piece, Exp e1, Exp e2, Exp e3, Exp e4)
{
  PWMap res;

  if (!isEmpty(dom_piece)) {
    SetPiece aux_dom = dom_piece;

    int j = 0;
    
    if (dom_piece.size() == e1.size() && e1.size() == e2.size() && e2.size() == e3.size() && e3.size() == e4.size()) {
      parallel_foreach5 (dom_piece.intervals_ref(), e1.exps_ref(), e2.exps_ref(), e3.exps_ref(), e4.exps_ref()) { 
        Interval i = boost::get<0>(items);
        LExp le1 = boost::get<1>(items), le2 = boost::get<2>(items);
        LExp le3 = boost::get<3>(items), le4 = boost::get<4>(items);
        PWMap ith = minMap(i, le1, le2, le3, le4);

        if (!isEmpty(ith)) {
           for (SBGMap sbgmap : ith) {
            aux_dom[j] = sbgmap.dom()[0][0];
            Exp e = sbgmap.exp();
             
            if (e == le2) res.emplaceBack(SBGMap(aux_dom, e2));
            else res.emplaceBack(SBGMap(aux_dom, e3));
          }
        }
        
        aux_dom = dom_piece;
        j++;
      }
    }

    return res;
  }

  Util::ERROR("LIB::PWMap::minMap: dimensions don't match");
  return PWMap();
}

PWMap minMap(SetPiece dom_piece, Exp e1, Exp e2)
{
  Exp id(e1.size());
  return minMap(dom_piece, id, e1, e2, id);
}

PWMap minMap(Set dom, Exp e1, Exp e2, Exp e3, Exp e4)
{
  PWMap res;

  if (!isEmpty(dom)) {
    Set d2, d3;
    auto f = [&](SetPiece dom_piece) {
      PWMap ith = minMap(dom_piece, e1, e2, e3, e4);
      for (SBGMap sbgmap : ith) {
        if (sbgmap.exp() == e2)
          d2 = cup(d2, sbgmap.dom());

        else
          d3 = cup(d3, sbgmap.dom());
      }
    };
    foreach (f, dom);

    if (!isEmpty(d2)) res.emplaceBack(SBGMap(d2, e2));

    if (!isEmpty(d3)) res.emplaceBack(SBGMap(d3, e3));
  }

  return res;
}

PWMap minMap(Set dom, Exp e1, Exp e2)
{
  Exp id(e1.size());
  return minMap(dom, id, e1, e2, id);
}

PWMap minMap(PWMap pw1, PWMap pw2, PWMap pw3, PWMap pw4)
{
  PWMap res;
  Set dom1 = dom(pw1), dom2 = dom(pw2), dom3 = dom(pw3), dom4 = dom(pw4);
  Set dom21 = preImage(dom1, pw2), dom34 = preImage(dom4, pw3);
  Set cap_dom23 = intersection(dom2, dom3), cap_dom14 = intersection(dom21, dom34);
  Set cap_dom = intersection(cap_dom23, cap_dom14);

  auto f = [&](SetPiece mdi) {
    Set mdi_set(mdi);
    bool flag1 = false, flag2 = false, flag3 = false, flag4 = false;
    Exp e1, e2, e3, e4;

    Set im2;
    for (SBGMap sbgmap2 : pw2) {
      Set d2 = sbgmap2.dom(), s2 = intersection(d2, mdi_set);
      if (!isEmpty(s2)) {
        e2 = sbgmap2.exp();
        flag2 = true;
        im2 = image(SBGMap(d2, e2));
        break;
      }
    }

    for (SBGMap sbgmap1 : pw1) {
      Set s1 = intersection(sbgmap1.dom(), im2);
      if (!isEmpty(s1)) {
        e1 = sbgmap1.exp();
        flag1 = true;
        break;
      }
    }

    Set im3;
    for (SBGMap sbgmap3 : pw3) {
      Set d3 = sbgmap3.dom(), s3 = intersection(d3, mdi_set);
      if (!isEmpty(s3)) {
        e3 = sbgmap3.exp();
        flag3 = true;
        im3 = image(SBGMap(d3, e3));
        break;
      }
    }

    for (SBGMap sbgmap4 : pw4) {
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
  };
  foreach(f, cap_dom);

  return res;
}

PWMap minMap(PWMap pw1, PWMap pw2)
{
  if (isEmpty(pw1) || isEmpty(pw2)) return PWMap();

  unsigned int nmbr_dims = nmbrDims(pw1);
  Exp id(nmbr_dims);
  SBGMap aux1(image(pw1), id), aux2(image(pw2), id);
  PWMap id_map1(aux1), id_map2(aux2);

  PWMap res = minMap(id_map1, pw1, pw2, id_map2);

  return minMap(id_map1, pw1, pw2, id_map2);
}

PWMap minAdjMap(PWMap pw1, PWMap pw2, PWMap pw3)
{
  PWMap res;

  Set partitioned_dom3 = preImage(image(pw3), pw3);
  Set partitioned_dom = preImage(partitioned_dom3, pw2);
  partitioned_dom = intersection(dom(pw1), partitioned_dom);
  partitioned_dom = intersection(dom(pw2), partitioned_dom);

  Set visited;
  auto f = [&](SetPiece mdi) {
    Set dom_res, mdi_set(mdi);
    Exp e_res, e1;

    // Find mdi in pw1 dom
    for (SBGMap sbgmap1 : pw1) {
      Set dom1 = sbgmap1.dom(), s1 = intersection(dom1, mdi_set);
      if (!isEmpty(s1)) {
        dom_res = image(s1, sbgmap1);
        e1 = sbgmap1.exp();
      }
    }

    // Find mdi in pw2 dom
    for (SBGMap sbgmap2 : pw2) {
      Set dom2 = sbgmap2.dom(), s2 = intersection(dom2, mdi_set);
      if (!isEmpty(s2)) {
        Set im2 = image(s2, sbgmap2);
        if (!isConstant(e1)) e_res = composition(sbgmap2.exp(), inverse(e1));

        else {
          for (SBGMap sbgmap3 : pw3) {
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

    SBGMap ith(dom_res, e_res);
    PWMap ith_pw(ith);
    Set again = intersection(dom_res, visited);
    if (!isEmpty(again)) {
      PWMap r_res = restrict(again, res), r_map = restrict(again, ith_pw);
      res = combine(minMap(res, ith_pw), res);
      res = combine(res, ith_pw); 
    }

    else res.emplaceBack(ith);

    visited = cup(visited, dom_res);
  };
  foreach(f, partitioned_dom);

  return res;
}

PWMap minAdjMap(PWMap pw1, PWMap pw2)
{
  if (!isEmpty(pw2)) {
    Set first_dom = pw2.maps_ref().begin()->dom();
    unsigned int nmbr_dims = std::visit([](auto v) { return v.begin()->size(); }, first_dom.pieces());
    Exp id(nmbr_dims);
    SBGMap aux2(image(pw2), id);
    PWMap id_map2(aux2);

    return minAdjMap(pw1, pw2, id_map2);
  }

  return PWMap();
}

PWMap minInv(Set d, PWMap pw)
{
  PWMap res;

  if (!isEmpty(pw)) {
    SBGMap first = *(pw.maps_ref().begin());
    res.emplaceBack(minInv(d, first));
    for (SBGMap sbgmap : pw) {
      SBGMap ith = minInv(d, sbgmap);
      Set cap_dom = intersection(ith.dom(), dom(res));

      if (!isEmpty(cap_dom)) {
        PWMap min = minMap(res, PWMap(ith));
        res = combine(min, res);
        Set diff = difference(ith.dom(), dom(res));
        res.emplaceBack(SBGMap(diff, ith.exp()));
      }

      else res.emplaceBack(ith);
    }
  }

  return res;
}

PWMap minInv(PWMap pw) { return minInv(dom(pw), pw); }

PWMap filterMap(bool (*f)(SBGMap), PWMap pw)
{
  PWMap res;

  for (SBGMap sbgmap : pw)
    if (f(sbgmap)) res.emplaceBack(sbgmap);

  return res;
}

Set equalImage(PWMap pw1, PWMap pw2)
{
  Set res;

  for (SBGMap sbgmap1 : pw1) {
    for (SBGMap sbgmap2 : pw2) {
      Set cap_dom = intersection(sbgmap1.dom(), sbgmap2.dom());
      if (!isEmpty(cap_dom)) {
        SBGMap map1(cap_dom, sbgmap1.exp()), map2(cap_dom, sbgmap2.exp());
        if (map1 == map2) res = cup(res, cap_dom);
      }
    }
  }

  return res; 
}

PWMap offsetDom(Util::MD_NAT off, PWMap pw)
{
  PWMap res;

  for (SBGMap sbgmap : pw) {
    SBGMap ith(offset(off, sbgmap.dom()), sbgmap.exp());
    res.emplaceBack(ith);
  }

  return res;
}

PWMap offsetDom(PWMap off, PWMap pw)
{
  PWMap res;

  for (SBGMap sbgmap : pw) {
    Set ith_dom = image(sbgmap.dom(), off);
    res.emplaceBack(SBGMap(ith_dom, sbgmap.exp()));
  }

  return res;
}

PWMap offsetImage(Util::MD_NAT off, PWMap pw)
{
  PWMap res;

  for (SBGMap sbgmap : pw) {
    Exp e = sbgmap.exp(), res_e;
    parallel_foreach2 (off, e.exps_ref()) {
      Util::NAT o = boost::get<0>(items);
      LExp lexp = boost::get<1>(items);
      LExp res_lexp(lexp.slope(), lexp.offset() + (Util::RATIONAL) o);
      res_e.emplaceBack(res_lexp);
    }

    res.emplaceBack(SBGMap(sbgmap.dom(), res_e));
  }

  return res;
}

PWMap offsetImage(Exp off, PWMap pw)
{
  PWMap res;

  for (SBGMap sbgmap : pw) {
    SBGMap ith(sbgmap.dom(), off + sbgmap.exp());
    res.emplaceBack(ith);
  }

  return res;
}

// Function should be called on a non-empty pw
unsigned int nmbrDims(PWMap pw)
{
  Set first_dom = pw.begin()->dom();
  return std::visit([](auto v) { return v.begin()->size(); }, first_dom.pieces());
}

PWMap normalize(PWMap pw)
{
  PWMap res;

  Set visited;
  for (SBGMap sbgmap1 : pw) {
    if (isEmpty(intersection(sbgmap1.dom(), visited))) {
      Set ith(sbgmap1.dom());
      for (SBGMap sbgmap2 : pw)
        if (sbgmap1.exp() == sbgmap2.exp()) cup(ith, sbgmap2.dom()); 

      visited = cup(visited, ith);
      res.emplaceBack(SBGMap(ith, sbgmap1.exp()));
    }
  }

  return res;
}

} // namespace LIB

} // namespace SBG
