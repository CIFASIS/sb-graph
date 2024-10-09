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
    SBGMap<Set> map(s, Exp(s.begin()->arity(), LExp()));
    maps_.emplace_back(map);
  }
}
template<typename Set>
PWMap<Set>::PWMap(Map map) : maps_() {
  if (!map.dom().isEmpty())
    maps_.emplace_back(map);
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
    maps_.emplace_back(map);
}
template<typename Set>
void PWMap<Set>::emplaceBack(Map map)
{
  if (!map.dom().isEmpty())
    maps_.emplace_back(map);
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

  for (const Map &map1 : maps_) 
    for (const Map &map2 : other.maps_) 
      res.emplaceBack(map1 + map2);

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::operator-(const PWMap &other) const
{
  PWMap res;

  if (isEmpty() || other.isEmpty())
    return PWMap();

  auto copies = arity();
  Interval all(0, 1, Util::Inf);
  Set univ(SetPiece(copies, all));
  for (const Map &map1 : maps_) {
    for (const Map &map2 : other.maps_) {
      Set dom = map1.dom().intersection(map2.dom());
      if (!dom.isEmpty()) {
        Exp minus_exp = map1.exp() - map2.exp();
        PWMap ith = univ;
        for (unsigned int j = 0; j < copies; ++j) {
          Util::RATIONAL m = minus_exp[j].slope(), h = minus_exp[j].offset();
          // Positive values in the jth dimension
          Util::NAT begin_neg = 0, end_neg = Util::Inf;
          Util::NAT begin_pos = 0, end_pos = Util::Inf;
          // Constant expression
          if (m == 0) {
            if (h < 0) {
              begin_pos = 1;
              end_pos = 0;
            }
            else {
              begin_neg = 1;
              end_neg = 0;
            }
          }
          else if (m > 0) {
            Util::RATIONAL cross = -h / m;
            if (cross > 0 || cross == 0) {
              begin_pos = boost::rational_cast<Util::NAT>(cross.value());
              if (begin_pos > 0)
                end_neg = begin_pos - 1;
              else {
                begin_neg = 1;
                begin_pos = 0;
              }
            }
          }
          else { 
            Util::RATIONAL cross = -h / m;
            if (cross > 0 || cross == 0) {
              end_pos = boost::rational_cast<Util::NAT>(cross.value());
              if (end_pos > 0)
                begin_neg = end_pos + 1;
            }
            else {
              begin_pos = 1;
              end_pos = 0;
            }
          }


          PWMap jth;
          Interval neg(begin_neg, 1, end_neg);
          Interval pos(begin_pos, 1, end_pos);
          for (const Map &map : ith) {
            SetPiece mdi = *(map.dom().begin());
            Exp e = map.exp(); 

            if (!neg.isEmpty()) {
              mdi[j] = neg;
              e[j] = LExp(0, 0);
              jth.emplaceBack(Map(mdi, e));
            }

            if (!pos.isEmpty()) {
              mdi[j] = pos;
              e[j] = minus_exp[j];
              jth.emplaceBack(Map(mdi, e));
            }
          }

          ith = std::move(jth);
        }
        res = res.concatenation(ith.restrict(dom));
      }
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
std::size_t PWMap<Set>::arity() const
{
  if (isEmpty())
    return 0;

  return begin()->dom().begin()->arity();
}

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
    res = res.concatenation(map.preImage(subcodom));

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::inverse() const
{
  PWMap res;

  for (const Map &map : maps_)
    res.emplaceBack(map.minInv());

  return res;
}

template<typename Set>
PWMap<Set> PWMap<Set>::composition(const PWMap &other) const
{
  PWMap<Set> res;

  Set im = other.image(), new_dom = other.preImage(im.intersection(dom()));
  PWMap map2 = other.restrict(new_dom);

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
    for (unsigned int j = 0; j < dom_piece.arity(); ++j) {
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
PWMap<Set> PWMap<Set>::minMap(const PWMap &other) const
{
  if (isEmpty() || other.isEmpty())
    return PWMap();

  PWMap aux1 = restrict(other.dom()), aux2 = other.restrict(dom());
  Set zero(SetPiece(arity(), Interval(0, 1, 0)));
  Set to_zero = (aux1-aux2).preImage(zero);
  Set not_zero = aux2.dom().difference(to_zero);
  aux1 = aux1.restrict(to_zero);
  aux2 = aux2.restrict(not_zero);

  return aux1.combine(aux2);
}

template<typename Set>
PWMap<Set> PWMap<Set>::minAdjMap(const PWMap &other) const
{
  PWMap res;

  Set visited;
  for (const Map &map1 : maps_) {
    for (const Map &map2 : other.maps_) {
      Set dom_res, ith_dom = map1.dom().intersection(map2.dom());
      if (!ith_dom.isEmpty()) {
        Exp e_res, e1;

        dom_res = map1.image(ith_dom);
        e1 = map1.exp();

        Set im2 = map2.image(ith_dom);
        if (!e1.isConstant())
          e_res = map2.exp().composition(e1.inverse());
        else
          e_res = MDLExp(im2.minElem());

        if (!dom_res.isEmpty()) {
          Map ith(dom_res, e_res);
          PWMap ith_pw(ith);
          Set again = dom_res.intersection(visited);
          if (!again.isEmpty()) {
            PWMap aux_res = res.restrict(dom_res);
            PWMap min_map = aux_res.minMap(ith_pw);
            res = min_map.combine(ith_pw).combine(res);
            visited = visited.cup(ith_pw.dom());
          }
          else {
            res.emplaceBack(ith);
            visited = visited.concatenation(dom_res);
          }
        }
      }
    }
  }

  return res;
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
    for (const Map &map2 : other.maps_) {
      Set cap_dom = map1.dom().intersection(map2.dom());
      if (!cap_dom.isEmpty()) {
        SBGMap<Set> m1(cap_dom, map1.exp()), m2(cap_dom, map2.exp());
        if (m1 == m2)
          res = res.concatenation(cap_dom);
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
    for (unsigned int j = 0; j < e.arity(); ++j) {
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
            new_ith = std::move(ith.value());
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

// Templates -------------------------------------------------------------------

template std::ostream &operator<<(std::ostream &out, const MapSet<UnordSet> &ms);
template struct PWMap<UnordSet>;
template std::ostream &operator<<(std::ostream &out, const BasePWMap &pw);

template std::ostream &operator<<(std::ostream &out, const MapSet<OrdSet> &ms);
template struct PWMap<OrdSet>;
template std::ostream &operator<<(std::ostream &out, const CanonPWMap &pw);

} // namespace LIB

} // namespace SBG
