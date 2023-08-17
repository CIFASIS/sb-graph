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

LTMap::LTMap() {}

bool LTMap::operator()(const SBGMap &x, const SBGMap &y) const
{
  if (x.exp() < y.exp()) return true;
  
  return x.dom() < y.dom();
}

std::ostream &operator<<(std::ostream &out, const MapSet &ms)
{
  MapSet aux = ms;
  int sz = aux.size();

  out << "<<";
  if (sz > 0) {
    auto it = aux.begin();
    for (int i = 0; i < sz - 1; ++i) {
      if (!isEmpty(it->dom()))
        out << *it << ", ";
      ++it;
    }
    if (!isEmpty(it->dom()))
      out << *it;
  }
  out << ">>";

  return out;
}

// PWMap -----------------------------------------------------------------------

PWMap::PWMap() : maps_() {}
PWMap::PWMap(SBGMap sm) : maps_() { maps_ref().emplace(sm); }
PWMap::PWMap(MapSet maps) : maps_(canonize(maps)) {}

member_imp(PWMap, MapSet, maps);

bool PWMap::operator==(const PWMap &other) const 
{
  if (!(dom(*this) == dom(other))) return false;

  if (maps() == other.maps()) return true;

  BOOST_FOREACH (SBGMap sbgmap1, maps()) {
    BOOST_FOREACH (SBGMap sbgmap2, other.maps()) {
      Set cap_dom = intersection(sbgmap1.dom(), sbgmap2.dom());

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

bool PWMap::operator!=(const PWMap &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &out, const PWMap &pw)
{
  out << pw.maps();

  return out;
}

// PWMap functions -------------------------------------------------------------

Set dom(PWMap pw) 
{
  Set res;

  BOOST_FOREACH (SBGMap sbgmap, pw.maps())
    res = concatenation(res, sbgmap.dom());

  return res;
}

PWMap restrict(Set subdom, PWMap pw)
{
  MapSet res;

  BOOST_FOREACH (SBGMap sbgmap, pw.maps())
    res.emplace(restrict(subdom, sbgmap));

  return PWMap(res);
}

Set image(PWMap pw) 
{
  Set res;

  BOOST_FOREACH (SBGMap sbgmap, pw.maps()) {
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

  BOOST_FOREACH (SBGMap sbgmap, pw.maps()) {
    Set ith = preImage(subcodom, sbgmap);
    res = cup(res, ith);
  }

  return res;
}

PWMap composition(PWMap pwi1, PWMap pwi2)
{
  MapSet res;

  Set im = image(pwi2), new_dom = preImage(im, pwi2);
  PWMap map2 = restrict(new_dom, pwi2);

  BOOST_FOREACH (SBGMap sbgmap1, pwi1.maps()) {
    BOOST_FOREACH (SBGMap sbgmap2, map2.maps()) {
      SBGMap ith = composition(sbgmap1, sbgmap2);
        if (!isEmpty(ith.dom())) 
          res.emplace(ith);
    }
  }

  return PWMap(res);
}

PWMap mapInf(PWMap pw)
{
  PWMap res = pw;

  if (!isEmpty(dom(pw))) {
    PWMap og_res = res, old_res;

    res = reduce(res);
    do {
      old_res = res;
      std::cout << res << "\n";
      res = composition(og_res, res);
      std::cout << res << "\n";

      res = reduce(res);
      std::cout << res << "\n\n";
    } while (old_res != res);
  }

  return res;
}

// Extra operations ------------------------------------------------------------

bool optConds(MapSet mm) { return true; }

MapSet canonize(MapSet mm)
{
  MapSet res;

  unsigned int sz = mm.size();
  if (sz == 1) res = mm;

  if (optConds(mm) && sz > 1) {
    MapSetIt it = mm.begin(), next_it = it;
    ++next_it;
    SBGMap ith = *it;
    for (unsigned int j = 0; j < sz - 1; ++j) {
      MaybeMap m = canonize(ith, *next_it); 
      if (m) ith = *m;

      else {
        if (!isEmpty(ith.dom())) res.emplace_hint(res.cend(), ith);

        if (j < (sz-2)) ith = *next_it;

        else ith = *next_it;
      }

      ++next_it;
      ++it;
    }
    if (!isEmpty(ith.dom())) res.emplace_hint(res.cend(), ith);
  }

  else res = mm;

  return res;
}

// !!! pw1 and pw2 should be domain-disjoint
PWMap concat(PWMap pw1, PWMap pw2)
{
  MapSetIt beg = pw1.maps_ref().begin(), end = pw1.maps_ref().end();
  MapSet res(beg, end); 

  BOOST_FOREACH (SBGMap sbgmap2, pw2.maps()) res.emplace(sbgmap2);

  return PWMap(res);
}

PWMap minMap(SetPiece dom_piece, Exp e1, Exp e2)
{
  MapSet res;

  Util::RATIONAL m1 = e1.slope(), h1 = e1.offset(), m2 = e2.slope(), h2 = e2.offset();

  // Parallel lines
  if (m1 == m2) {
    if (h1 < h2) res.emplace(SBGMap(Set(dom_piece), e1));

    else res.emplace(SBGMap(Set(dom_piece), e2));
  }

  // Non parallel lines
  Util::RATIONAL cap_point = (e2.offset() - e1.offset()) / (e1.slope() - e2.slope());
  Interval before, after;
  if (cap_point < 0) after = dom_piece;

  else {
    Util::NAT beg = dom_piece.begin(), st = dom_piece.step(), end = dom_piece.end();
    double double_cap_point = cap_point.numerator() / cap_point.denominator();
    before = Interval(beg, st, (Util::NAT) floor(double_cap_point));
    before = intersection(before, dom_piece);
    after = Interval((Util::NAT) ceil(double_cap_point), st, end);
    after = intersection(after, dom_piece);
  }

  // Before intersection
  if (!isEmpty(before)) {
    if (m1 > m2) res.emplace(SBGMap(Set(before), e1));

    else res.emplace(SBGMap(Set(before), e2));
  }

  // After intersection
  if (!isEmpty(after)) {
    if (m1 > m2) res.emplace(SBGMap(Set(before), e2));

    else res.emplace(SBGMap(Set(before), e1));
  }

  return PWMap(res);
}

//TODO
PWMap minMap(Set dom, Exp e1, Exp e2)
{
  PWMap res;

  return res;
}

//TODO
PWMap minMap()
{
  PWMap res;

  return res;
}

PWMap reduce(SetPiece dom_piece, Exp e)
{
  PWMap res;

  Set d(dom_piece);
 
  Util::RATIONAL zero(0, 1), one(1, 1);
  if (e.slope() == one && e.offset() != zero) {
    Util::INT h = toInt(e.offset()); 

    Util::NAT st = dom_piece.step();
    if (h == (Util::INT) st) {
      Util::NAT hi = dom_piece.end();
      if (st < Util::Inf - hi) res = PWMap(SBGMap(d, Exp(0, hi + st)));
    }

    if (h == (Util::INT)  -st) {
      Util::NAT lo = dom_piece.begin(); 
      if (lo >= st) res = PWMap(SBGMap(d, Exp(0, lo - st)));
    }

    if (h % (Util::INT) st == 0) {
      // Is convenient the partition of the piece?
      if ((Util::INT) cardinal(dom_piece) > h*h) {
        Util::INT absh = std::abs(h);
        MapSet sm;

        for (int k = 1; k <= absh; ++k) {
          Util::NAT new_begin = dom_piece.begin() + k - 1;
          SetPiece kth_piece(new_begin, (Util::NAT) absh, dom_piece.end());

          Exp kth_exp(0, 0);
          if (h > 0) kth_exp.set_offset(kth_piece.end() + h);

          else kth_exp.set_offset(kth_piece.begin() + h);

          SBGMap kth_map(Set(kth_piece), kth_exp);
          sm.emplace_hint(sm.cend(), kth_map);  
        }

        if (!sm.empty()) return PWMap(sm);
      }
    }
  }

  return PWMap(SBGMap(Set(d), e));
}

PWMap reduce(SBGMap sbgmap)
{
  PWMap res;

  BOOST_FOREACH (SetPiece dom_piece, sbgmap.dom().pieces()) {
    PWMap ith = reduce(dom_piece, sbgmap.exp());
    res = concat(ith, res);
  }

  return res;
}

PWMap reduce(PWMap pw)
{
  PWMap res;

  BOOST_FOREACH (SBGMap sbgmap, pw.maps()) {
    PWMap ith = reduce(sbgmap);
    res = concat(ith, res);
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

} // namespace LIB

} // namespace SBG
