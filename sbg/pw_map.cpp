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
std::ostream &operator<<(std::ostream &out, const MapSet<Set> &ms)
{
  MapSet<Set> aux = ms;
  int sz = aux.size();

  out << "<<";
  if (sz > 0) {
    auto it = aux.begin();
    for (int i = 0; i < sz - 1; ++i) {
      if (!it->dom().isEmpty())
        out << *it << ", ";
      ++it;
    }
    if (!it->dom().isEmpty())
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
  if (!s.isEmpty()) {
    SBGMap<Set> map(s, Exp(s.begin()->size(), LExp()));
    maps_.push_back(map);
  }
}
template<typename Set>
PWMap<Set>::PWMap(Map map) : maps_() {
  if (!map.dom().isEmpty())
    maps_.push_back(map);
}
template<typename Set>
PWMap<Set>::PWMap(MS maps) : maps_(maps) {}

member_imp_temp(template<typename Set>, PWMap<Set>, MapSet<Set>, maps);

template<typename Set>
typename PWMap<Set>::iterator PWMap<Set>::begin() { return maps_.begin(); }
template<typename Set>
typename PWMap<Set>::iterator PWMap<Set>::end() { return maps_.end(); }
template<typename Set>
typename PWMap<Set>::const_iterator PWMap<Set>::begin() const
{
  return maps_.begin();
}
template<typename Set>
typename PWMap<Set>::const_iterator PWMap<Set>::end() const
{
  return maps_.end();
}

template<typename Set>
std::size_t PWMap<Set>::size() const { return maps_.size(); }

template<typename Set>
void PWMap<Set>::emplace(Map map) {
  if (!map.dom().isEmpty())
    maps_.push_back(map);
}
template<typename Set>
void PWMap<Set>::emplaceBack(Map map)
{
  if (!map.dom().isEmpty())
    maps_.push_back(map);
}

template<typename Set>
bool PWMap<Set>::operator==(const PWMap &other) const 
{
  if (dom() != other.dom())
    return false;

  if (maps_ == other.maps_)
    return true;

  for (const Map &map1 : maps_) {
    for (const Map &map2 : other.maps_) {
      Set cap_dom = map1.dom().intersection(map2.dom());

      if (!cap_dom.isEmpty()) {
        // Here we check by image because the same image can be obtained through
        // two different lexps.
        // Example: [1:1:1] -> 10 and [1:1:1] -> x+9
        if (cap_dom.cardinal() == 1) {
          Map m1(cap_dom, map1.exp()), m2(cap_dom, map2.exp());
          if (m1.image() != m2.image())
            return false;
        }

        // When there is more than one element we can't check equality on the
        // image because there are at least two linear maps with the same
        // domain and image.
        // Example: [1:1:10] -> x and [1:1:10] -> -x+10
        else {
          if (map1.exp() != map2.exp())
            return false;
        }
      }
    }
  }

  return true;
}

template<typename Set>
bool PWMap<Set>::operator!=(const PWMap &other) const
{ 
  return !(*this == other);
}

template<typename Set>
PWMap<Set> PWMap<Set>::operator+(const PWMap &other) const
{
  PWMap res;

  for (const Map &map1 : maps_) {
    for (const Map &map2 : other.maps_) {
      Set ith_dom = map1.dom().intersection(map2.dom());
      Exp ith_exp = map1.exp() + map2.exp();
      res.emplaceBack(SBGMap(ith_dom, ith_exp));
    }
  }

  return res;
}

template<typename Set>
std::ostream &operator<<(std::ostream &out, const PWMap<Set> &pw)
{
  out << pw.maps();

  return out;
}

// PWMap functions -------------------------------------------------------------

template<typename Set>
bool PWMap<Set>::isEmpty() const { return maps_.empty(); }

template<typename Set>
Set PWMap<Set>::dom() const
{
  Set res;
  for (const Map &map : maps_)
    res = res.concatenation(map.dom());

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::restrict(const Set &subdom) const
{
  PWMap res;

  for (const Map &map : maps_)
    res.emplaceBack(map.restrict(subdom));

  return PWMap(res);
}

template<typename Set>
Set PWMap<Set>::image() const
{
  Set res;

  for (const Map &map : maps_)
    res = res.cup(map.image());

  return res;
}

template<typename Set>
Set PWMap<Set>::image(const Set &subdom) const
{
  return restrict(subdom).image();
}

template<typename Set>
Set PWMap<Set>::preImage() const { return dom(); }

template<typename Set>
Set PWMap<Set>::preImage(const Set &subcodom) const
{
  Set res;

  for (const Map &map : maps_)
    res = res.cup(map.preImage(subcodom));

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::inverse() const
{
  PWMap res;

  for (const Map &map : maps_) {
    if (!map.exp().isConstant()) {
      Set new_dom = map.image();
      Exp new_exp = map.exp().inverse();
      res.emplaceBack(Map(new_dom, new_exp));
    }
    else if (map.dom().cardinal() == 1) {
      Set new_dom = map.image();
      Exp new_exp(map.dom().minElem());
      res.emplaceBack(Map(new_dom, new_exp));
    }
    else {
      Util::ERROR("LIB::PWMap::inverse: map is not bijective");
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::composition(const PWMap &other) const
{
  PWMap<Set> res;

  Set im = other.image(), new_dom = other.preImage(im);
  PWMap map2 = other.restrict(new_dom);

  // TODO PWMap<Set> aux1 = normalize(), aux2 = map2.normalize();
  for (const Map &map1 : maps_)
    for (const Map &map2 : map2.maps_)
      res.emplaceBack(map1.composition(map2));

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::mapInf(unsigned int n) const
{
  PWMap res = *this, old_res;

  if (!dom().isEmpty()) {
    for (unsigned int j = 0; old_res != res && j < n; ++j) {
      old_res = res;
      res = res.composition(*this);
    }

    if (old_res == res)
      return res;

    res = res.reduce();
    do {
      old_res = res;
      res = res.composition(res);

      res = res.reduce();
    } while (old_res != res);
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::mapInf() const { return mapInf(0); }

// Extra operations ------------------------------------------------------------

// !!! pw1 and pw2 should be domain-disjoint
template<typename Set>
PWMap<Set> PWMap<Set>::concatenation(const PWMap &other) const
{
  PWMap res = *this;

  for (const Map &map2 : other.maps_)
    res.emplaceBack(map2);

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::combine(const PWMap &other) const
{
  if (isEmpty())
    return other;

  if (other.isEmpty())
    return *this;

  PWMap res = *this;
  Set dom1 = dom();
  for (const Map &map2 : other.maps_) {
    Set dom2 = map2.dom(), new_dom = dom2.difference(dom1);
    res.emplaceBack(Map(new_dom, map2.exp()));
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::reduce(const Interval &i, const LExp &le) const
{
  PWMap<Set> res;

  if (!i.isEmpty()) {
    Util::RATIONAL zero(0, 1);
    if (le.slope() == 1 && le.offset() != 0) {
      Util::INT h = le.offset().toInt();

      Util::NAT st = i.step();
      if (h == (Util::INT) st) {
        Util::NAT hi = i.end();
        Util::RATIONAL const_expr(hi + st, 1);
        if (st < Util::Inf - hi)
          res = PWMap(Map(i, LExp(zero, const_expr)));
      }

      else if (h == (Util::INT) -st) {
        Util::NAT lo = i.begin();
        Util::RATIONAL const_expr(lo - st, 1);
        if (lo >= st)
          res = PWMap(Map(i, LExp(zero, const_expr)));
      }

      else if (h % (Util::INT) st == 0) {
        // Is convenient the partition of the piece?
        if ((Util::INT) i.cardinal() > h*h) {
          res = PWMap<Set>();
          Util::INT absh = std::abs(h);

          for (int k = 1; k <= absh; ++k) {
            Util::NAT new_begin = i.begin() + k - 1;
            Interval kth_piece(new_begin, (Util::NAT) absh, i.end());

            LExp kth_exp(0, 0);
            if (h > 0)
              kth_exp.set_offset(kth_piece.end() + h);
            else
              kth_exp.set_offset(kth_piece.begin() + h);

            res.emplaceBack(Map(kth_piece, kth_exp));
          }
        }
      }
    }

    else
      res.emplaceBack(Map(i, le));
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::reduce(const Map &map) const
{
  PWMap<Set> res;

  Set not_reduced;
  Exp e = map.exp();
  for (const SetPiece &dom_piece : map.dom()) {
    SetPiece aux_piece = dom_piece;
    Exp aux_exp = e;
    bool was_reduced = false;
    for (unsigned int j = 0; j < dom_piece.size(); ++j) {
      for (const Map &ith_reduced : reduce(dom_piece[j], e[j])) {
        aux_piece[j] = ith_reduced.dom().begin()->operator[](0);
        aux_exp[j] = ith_reduced.exp()[0];
        if (aux_piece != dom_piece || aux_exp != e) {
          res.emplaceBack(Map(aux_piece, aux_exp));
          was_reduced = true;
        }

        aux_piece = dom_piece;
        aux_exp = e;
      }

      ++j;
    }

    if (!was_reduced)
      not_reduced = not_reduced.concatenation(dom_piece);
  }

  res.emplaceBack(Map(not_reduced, e)); // Add unreduced submaps

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::reduce() const
{
  PWMap res;

  for (const Map &map : maps_) {
    PWMap ith = reduce(map);
    res = res.concatenation(ith);
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::minMap(
  const Interval &i, const LExp &le1
  , const LExp &le2, const LExp &le3, const LExp &le4
) const
{
  PWMap res;

  LExp le21 = le1.composition(le2), le34 = le4.composition(le3);
  Util::RATIONAL m21 = le21.slope(), h21 = le21.offset()
                , m34 = le34.slope(), h34 = le34.offset();

  // Parallel lines
  if (m21 == m34) {
    if (h21 < h34)
      res.emplaceBack(Map(i, le2));
    else
      res.emplaceBack(Map(i, le3));

    return res;
  }

  // Non parallel lines
  Util::RATIONAL cap_point = (h34 - h21) / (m21 - m34);
  Interval before, after;
  if (cap_point < 0)
    after = i;

  else {
    Util::NAT beg = i.begin(), st = i.step(), end = i.end();
    double double_cap_point = cap_point.numerator() / cap_point.denominator();
    before = Interval(beg, st, (Util::NAT) floor(double_cap_point));
    before = before.intersection(i);
    after = Interval(before.end() + st, st, end);
    after = after.intersection(i);
  }

  // Before intersection
  if (!before.isEmpty()) {
    if (m34 > m21)
      res.emplaceBack(Map(before, le3));
    else
      res.emplaceBack(Map(before, le2));
  }

  // After intersection
  if (!after.isEmpty()) {
    if (m21 > m34)
      res.emplaceBack(Map(after, le3));
    else
      res.emplaceBack(Map(after, le2));
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::minMap(
  const Interval &i, const LExp &le1, const LExp &le2
) const
{
  LExp id;
  return minMap(i, id, le1, le2, id);
}

template<typename Set>
PWMap<Set> PWMap<Set>::minMap(
  const SetPiece &dom_piece, const Exp &e1, const Exp &e2
  , const Exp &e3, const Exp &e4
) const
{
  PWMap res;

  if (!dom_piece.isEmpty()) {
    SetPiece aux_dom = dom_piece;

    bool cond1 = dom_piece.size() == e1.size();
    bool cond2 = e1.size() == e2.size(); 
    bool cond3 = e2.size() == e3.size(); 
    bool cond4 = e3.size() == e4.size(); 

    Util::ERROR_UNLESS(cond1 && cond2 && cond3 && cond4
                       , "LIB::PWMap::minMap: dimensions don't match");

    unsigned int j = 0;
    for (; j < dom_piece.size(); ++j) {
      if (e2[j] != e3[j]) {
        PWMap ith = minMap(dom_piece[j], e1[j], e2[j], e3[j], e4[j]);

        if (!ith.isEmpty()) {
          for (const Map &map : ith) {
            aux_dom[j] = map.dom().begin()->operator[](0);

            Map new_ith;
            if (map.exp() == e2[j])
              new_ith = Map(aux_dom, e2);
            else
              new_ith = Map(aux_dom, e3);

            res.emplaceBack(new_ith);
          }

          aux_dom = dom_piece;
        }

        break;
      }
    }

    if (j == dom_piece.size())
      res.emplaceBack(Map(dom_piece, e2));
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::minMap(
  const SetPiece &dom_piece, const Exp &e1, const Exp &e2
) const
{
  Exp id(e1.size(), LExp());
  return minMap(dom_piece, id, e1, e2, id);
}

template<typename Set>
PWMap<Set> PWMap<Set>::minMap(
  const Set &dom, const Exp &e1, const Exp &e2, const Exp &e3, const Exp &e4
) const
{
  PWMap res;
  
  Set d2, d3;

  for (const SetPiece &dom_piece : dom) {
    PWMap ith = minMap(dom_piece, e1, e2, e3, e4);
    for (const Map &map : ith) {
      if (map.exp() == e2)
        d2 = d2.cup(map.dom());
      else
        d3 = d3.cup(map.dom());
    }
  }

  res.emplaceBack(Map(d2, e2));
  res.emplaceBack(Map(d3, e3));

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::minMap(const Set &dom, const Exp &e1, const Exp &e2) const
{
  Exp id(e1.size(), LExp());
  return minMap(dom, id, e1, e2, id);
}

template<typename Set>
PWMap<Set> PWMap<Set>::minMap(
  const PWMap &other2, const PWMap &other3, const PWMap &other4
) const
{
  PWMap<Set> res;
  Set dom1 = dom(), dom2 = other2.dom()
    , dom3 = other3.dom(), dom4 = other4.dom();
  Set dom21 = other2.preImage(dom1), dom34 = other3.preImage(dom4);
  Set cap_dom23 = dom2.intersection(dom3)
      , cap_dom14 = dom21.intersection(dom34);
  Set cap_dom = cap_dom23.intersection(cap_dom14);

  for (const SetPiece &mdi : cap_dom) {
    Set mdi_set(mdi);
    bool flag1 = false, flag2 = false, flag3 = false, flag4 = false;
    Exp e1, e2, e3, e4;

    Set im2;
    for (const Map &map2 : other2) {
      Set d2 = map2.dom(), s2 = d2.intersection(mdi_set);
      if (!s2.isEmpty()) {
        e2 = map2.exp();
        flag2 = true;
        im2 = map2.image(mdi_set);
        break;
      }
    }

    for (const Map &map1 : maps_) {
      Set s1 = map1.dom().intersection(im2);
      if (!s1.isEmpty()) {
        e1 = map1.exp();
        flag1 = true;
        break;
      }
    }

    Set im3;
    for (const Map &map3 : other3) {
      Set d3 = map3.dom(), s3 = d3.intersection(mdi_set);
      if (!s3.isEmpty()) {
        e3 = map3.exp();
        flag3 = true;
        im3 = map3.image(mdi_set);
        break;
      }
    }

    for (const Map &map4 : other4) {
      Set s4 = map4.dom().intersection(im3);
      if (!s4.isEmpty()) {
        e4 = map4.exp();
        flag4 = true;
        break;
      }
    }

    if (flag1 && flag2 && flag3 && flag4) {
      PWMap ith = minMap(mdi_set, e1, e2, e3, e4);
      res = ith.concatenation(res);
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::minMap(const PWMap &other) const
{
  if (isEmpty() || other.isEmpty())
    return PWMap();

  unsigned int nmbr_dims = nmbrDims();
  Exp id(nmbr_dims, LExp());
  Map aux1(image(), id), aux2(other.image(), id);
  PWMap id_map1(aux1), id_map2(aux2);

  return id_map1.minMap(*this, other, id_map2);
}

template<typename Set>
PWMap<Set> PWMap<Set>::minAdjMap(const PWMap &other2, const PWMap &other3) const
{
  PWMap res;

  Set partitioned_dom = dom().intersection(other2.dom());

  Set visited;
  for (const SetPiece &mdi : partitioned_dom) {
    Set dom_res, mdi_set(mdi);
    Exp e_res, e1;

    // Find mdi in pw1 dom
    for (const Map &map1 : maps_) {
      Set dom1 = map1.dom(), s1 = dom1.intersection(mdi_set);
      if (!s1.isEmpty()) {
        dom_res = map1.image(s1);
        e1 = map1.exp();
      }
    }

    // Find mdi in pw2 dom
    for (const Map &map2 : other2) {
      Set dom2 = map2.dom(), s2 = dom2.intersection(mdi_set);
      if (!s2.isEmpty()) {
        Set im2 = map2.image(s2);
        if (!e1.isConstant())
          e_res = map2.exp().composition(e1.inverse());

        else {
          for (const Map &map3 : other3) {
            Set dom3 = map3.dom(), s3 = dom3.intersection(im2);
            if (!s3.isEmpty()) {
              if (!map3.exp().isConstant()) {
                Set min3(map3.image(s3).minElem());
                MD_NAT min2 = map3.preImage(min3).minElem();
                e_res = MDLExp(min2);
              }

              else e_res = MDLExp(im2.minElem());
            }
          }
        }
      }
    }

    if (!dom_res.isEmpty()) {
      Map ith(dom_res, e_res);
      PWMap ith_pw(ith);
      Set again = dom_res.intersection(visited);
      if (!again.isEmpty()) {
        PWMap aux_res = res.restrict(dom_res);
        PWMap min_map = other3.minMap(aux_res, ith_pw, other3);
        res = min_map.combine(ith_pw).combine(res);
        visited = visited.cup(ith_pw.dom());
      }
      else {
        res.emplaceBack(ith);
        visited = visited.concatenation(dom_res);
      }
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::minAdjMap(const PWMap &other) const
{
  if (!other.isEmpty()) {
    Exp id(other.nmbrDims(), LExp());
    Map aux2(other.image(), id);
    PWMap id_map2(aux2);

    return minAdjMap(other, id_map2);
  }

  return PWMap();
}

template<typename Set>
PWMap<Set> PWMap<Set>::firstInv(const Set &allowed) const
{
  PWMap res;

  Set visited;
  for (const Map &map : maps_) {
    Set res_dom = map.image(allowed).difference(visited);
    if (!res_dom.isEmpty()) {
      Map new_map(map.preImage(res_dom), map.exp());
      res.emplaceBack(new_map.minInv());

      visited = visited.cup(map.image(allowed));
    }
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::firstInv() const { return firstInv(dom()); }

template<typename Set>
PWMap<Set> PWMap<Set>::filterMap(bool (*f)(const SBGMap<Set> &)) const
{
  PWMap res;

  for (const Map &map : maps_)
    if (f(map))
      res.emplaceBack(map);

  return res;
}

template<typename Set>
Set PWMap<Set>::equalImage(const PWMap &other) const
{
  Set res;

  for (const Map &map1 : maps_) {
    for (const Map &map2 : other) {
      Set cap_dom = map1.dom().intersection(map2.dom());
      if (!cap_dom.isEmpty()) {
        SBGMap<Set> m1(cap_dom, map1.exp()), m2(cap_dom, map2.exp());
        if (m1 == m2)
          res = res.cup(cap_dom);
      }
    }
  }

  return res; 
}

template<typename Set>
Set PWMap<Set>::gtImage(const PWMap &other) const
{
  Set res;

  for (const Map &map1 : maps_) {
    for (const Map &map2 : other) {
      Set cap_dom = map1.dom().intersection(map2.dom());
      if (!cap_dom.isEmpty()) {
        Exp e1 = map1.exp(), e2 = map2.exp();
        SBGMap<Set> m1(cap_dom, e1), m2(cap_dom, e2);
        if (!(e1 < e2))
          res = res.cup(cap_dom);
      }
    }
  }

  return res; 
}

template<typename Set>
PWMap<Set> PWMap<Set>::offsetDom(const Util::MD_NAT &off) const
{
  PWMap res;

  for (const Map &map : maps_)
    res.emplaceBack(Map(map.dom().offset(off), map.exp()));

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::offsetDom(const PWMap &off) const
{
  PWMap res;

  for (const Map &map : maps_) {
    Set ith_dom = off.image(map.dom());
    res.emplaceBack(Map(ith_dom, map.exp()));
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::offsetImage(const Util::MD_NAT &off) const
{
  PWMap res;

  for (const Map &map : maps_) {
    Exp e = map.exp(), res_e;
    for (unsigned int j = 0; j < e.size(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (Util::RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    res.emplaceBack(Map(map.dom(), res_e));
  }

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::offsetImage(const Exp &off) const
{
  PWMap res;

  for (const Map &map : maps_) 
    res.emplaceBack(Map(map.dom(), off + map.exp()));

  return res;
}

// Function should be called on a non-empty pw
template<typename Set>
unsigned int PWMap<Set>::nmbrDims() const
{
  Util::ERROR_UNLESS(!isEmpty(), "LIB::PWMap::nmbrDims: empty not allowed");

  return begin()->dom().begin()->size();
}

template<typename Set>
PWMap<Set> PWMap<Set>::compact() const
{
  PWMap res;

  if (dom().isEmpty())
    return res;

  Set compacted;
  for (auto it = begin(); it != end(); ++it) {
    auto next_it = it;
    ++next_it;
    Set ith_compacted = compacted.intersection(it->dom());
    if (ith_compacted.isEmpty()) {
      Map new_ith(it->dom().compact(), it->exp());
      for (; next_it != end(); ++next_it) {
        Set next_compacted = compacted.intersection(next_it->dom());
        if (next_compacted.isEmpty()) {
          auto ith = new_ith.compact(*next_it);
          if (ith) {
            new_ith = ith.value();
            compacted = compacted.cup(next_it->dom());
          }
        }
      }

      res.emplaceBack(new_ith);
    }
  }

  return res;
}

template<typename Set>
Set PWMap<Set>::sharedImage()
{
  Set not_present = dom().difference(firstInv().image());
  Set res = preImage(image(not_present));

  return res;
}

template<typename Set>
std::size_t hash_value(const PWMap<Set> &pw)
{
  return boost::hash_range(pw.begin(), pw.end());
}

// Templates -------------------------------------------------------------------

template std::ostream &operator<<(std::ostream &out, const MapSet<UnordSet> &ms);
template struct PWMap<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BasePWMap &pw);
template std::size_t hash_value(const PWMap<UnordSet> &pw);

template std::ostream &operator<<(std::ostream &out, const MapSet<OrdSet> &ms);
template struct PWMap<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonPWMap &pw);
template std::size_t hash_value(const PWMap<OrdSet> &pw);

} // namespace LIB

} // namespace SBG
