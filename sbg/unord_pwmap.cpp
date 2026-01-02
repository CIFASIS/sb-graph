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

#include <forward_list>

#include "sbg/unord_pwmap.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Unordered PWMap Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Auxiliary functions - Unordered Piecewise maps ------------------------------

void pushBack(UnordPWMap::UnordMapCollection& unord_pw, const Map& m)
{
  unord_pw.emplace_back(m);
}

// Member functions - Unordered Piecewise maps ---------------------------------

member_imp(UnordPWMap, UnordPWMap::UnordMapCollection, pieces);

UnordPWMap::UnordPWMap() {}
UnordPWMap::UnordPWMap(const Set& s) : pieces_() {
  if (!s.isEmpty()) {
    SetPiece first = s.begin().operator*();
    pieces_.push_back(Map(s, Exp(first.arity(), LExp())));
  }
}
UnordPWMap::UnordPWMap(const Map& m) : pieces_() {
  if (!m.dom().isEmpty())
    pieces_.push_back(m);
}
UnordPWMap::UnordPWMap(const UnordPWMap::UnordMapCollection& pieces)
  : pieces_(std::move(pieces)) {}

member_imp(UnordPWMap::Iterator, UnordPWMap::UnordMapCollection::const_iterator
  , it);

UnordPWMap::Iterator::Iterator(UnordMapCollection::const_iterator it)
  : it_(it) {}

void UnordPWMap::Iterator::operator++()
{
  ++it_;
  return;
}

bool UnordPWMap::Iterator::operator!=(const PWMapStrategy::Iterator& other)
  const
{
  return it_ != static_cast<const UnordPWMap::Iterator *>(&other)->it_;
}

const Map& UnordPWMap::Iterator::operator*() const { return *it_; }

std::shared_ptr<PWMapStrategy::Iterator> UnordPWMap::begin() const
{
  return std::make_shared<UnordPWMap::Iterator>(pieces_.begin());
}

std::shared_ptr<PWMapStrategy::Iterator> UnordPWMap::end() const
{
  return std::make_shared<UnordPWMap::Iterator>(pieces_.end());
}

void UnordPWMap::emplaceBack(const Map& m)
{
  if (!m.isEmpty()) {
    pieces_.emplace_back(m);
  }
}

bool UnordPWMap::operator==(const PWMapStrategy& other) const 
{
  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);

  if (dom() != othr.dom()) 
    return false;

  if (pieces_ == othr.pieces_)
    return true;

  for (const Map& m1 : pieces_) {
    for (const Map& m2 : othr.pieces_) {
      Set cap_dom = m1.dom().intersection(m2.dom());
      if (!cap_dom.isEmpty()) {
        Exp exp1 = m1.exp();
        Exp exp2 = m2.exp();
        if (exp1 != exp2) {
          return false;
        }

        Map cap_m1(cap_dom, exp1);
        Map cap_m2(cap_dom, exp2);
        if (cap_m1 != cap_m2) {
          return false;
        }
      }
    }
  }

  return true;
}

bool UnordPWMap::operator!=(const PWMapStrategy& other) const
{ 
  return !(*this == other);
}

UnordPWMap& UnordPWMap::operator=(UnordPWMap&& other)
{
  if (this != &other)
    pieces_ = std::move(other.pieces_);

  return *this;
}

std::ostream& UnordPWMap::print(std::ostream& out) const
{
  int sz = pieces_.size();

  out << "<<";
  if (sz > 0) {
    int i = 0;
    for (; i < sz - 1; ++i) {
      out << pieces_[i] << ", ";
    }
    out << pieces_[i];
  }
  out << ">>";

  return out;
}

PWMapStratPtr UnordPWMap::operator+(const PWMapStrategy& other) const
{
  PWMapStratPtr res = std::make_unique<UnordPWMap>();
  
  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map& m1 : pieces_) 
    for (const Map& m2 : othr.pieces_) 
      res->emplaceBack(m1 + m2);

  return res;
}

PWMapStratPtr UnordPWMap::clone() const
{
  return std::make_unique<UnordPWMap>(pieces_);
}

// PWMap functions -------------------------------------------------------------

std::size_t UnordPWMap::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->dom().arity();
}

bool UnordPWMap::isEmpty() const { return pieces_.empty(); }

Set UnordPWMap::dom() const
{
  Set result = SET_FACT.createSet();
  for (const Map& m : pieces_) {
    Set ith_dom = m.dom();
    result = std::move(result).disjointCup(std::move(ith_dom));
  }

  return result;
}

PWMapStratPtr UnordPWMap::restrict(const Set& subdom) const
{
  PWMapStratPtr res = std::make_unique<UnordPWMap>();

  for (const Map& m : pieces_)
    res->emplaceBack(m.restrict(subdom));

  return res;
}

Set UnordPWMap::image() const
{
  Set res = SET_FACT.createSet();

  for (const Map& m : pieces_) {
    Set ith_img = m.image();
    res = std::move(res).cup(std::move(ith_img));
  }

  return res;
}

Set UnordPWMap::image(const Set& subdom) const
{
  return restrict(subdom)->image();
}

Set UnordPWMap::preImage(const Set& subcodom) const
{
  Set res = SET_FACT.createSet();

  for (const Map& m : pieces_)
    res = res.disjointCup(m.preImage(subcodom));

  return res;
}

PWMapStratPtr UnordPWMap::inverse() const
{
  PWMapStratPtr res = std::make_unique<UnordPWMap>();

  for (const Map& m : pieces_)
    res->emplaceBack(m.minInv());

  return res;
}

PWMapStratPtr UnordPWMap::composition(const PWMapStrategy& other) const
{
  PWMapStratPtr res = std::make_unique<UnordPWMap>();

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);

  for (const Map& m1 : pieces_) { 
    for (const Map& m2 : othr.pieces_) {
      res->emplaceBack(m1.composition(m2));
    }
  }
  return res;
}

PWMapStratPtr UnordPWMap::mapInf(unsigned int n) const
{
  PWMapStratPtr result = std::make_unique<UnordPWMap>(pieces_);

  if (!dom().isEmpty()) {
    for (unsigned int j = 0; j < n; ++j) {
      PWMapStratPtr new_res = result->composition(*this);
      result = std::move(new_res);
    }

    PWMapStratPtr reduced = result->reduce();
    result = std::move(reduced);
    PWMapStratPtr old_res = result->clone();
    do {
      old_res = result->clone();

      PWMapStratPtr new_res = result->composition(*result);
      new_res = new_res->reduce();
      result = std::move(new_res);
    } while (*old_res != *result);
  }

  return result;
}

PWMapStratPtr UnordPWMap::mapInf() const { return mapInf(0); }

Set UnordPWMap::fixedPoints() const
{
  Set res = SET_FACT.createSet();

  for (const Map& m : pieces_)
    res = res.disjointCup(m.fixedPoints());

  return res;
}

// Extra operations ------------------------------------------------------------

PWMapStratPtr UnordPWMap::concatenation(const PWMapStrategy& other) const
{
  PWMapStratPtr result = std::make_unique<UnordPWMap>(pieces_);

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map& m2 : othr.pieces_) {
    result->emplaceBack(m2);
  }

  return result;
}

PWMapStratPtr UnordPWMap::combine(const PWMapStrategy& other) const
{ 
  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  if (isEmpty()) {
    return std::make_unique<UnordPWMap>(othr.pieces_);
  }

  if (other.isEmpty()) {
    return std::make_unique<UnordPWMap>(pieces_);
  }

  if (pieces_ == othr.pieces_) {
    return std::make_unique<UnordPWMap>(pieces_);
  }

  Set exclusive_other = other.dom().difference(dom());
  return concatenation(*other.restrict(exclusive_other));
}

PWMapStratPtr UnordPWMap::reduce() const
{
  UnordMapCollection result;
  for (const Map& m : pieces_) {
    std::vector<Map> reduced = m.reduce();
    for(const Map& reduced_map : reduced) {
      result.emplace_back(reduced_map);
    }
  }

  return std::make_unique<UnordPWMap>(std::move(result));
}

PWMapStratPtr UnordPWMap::minMap(const PWMapStrategy& other) const
{
  if (isEmpty() || other.isEmpty())
    return std::make_unique<UnordPWMap>();

  Set min_in_pw1 = lessImage(other);
  return restrict(min_in_pw1)->combine(*other.restrict(dom())); 
}  

PWMapStratPtr UnordPWMap::minAdjMap(const PWMapStrategy& other) const
{
  PWMapStratPtr result = std::make_unique<UnordPWMap>();

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  Set visited = SET_FACT.createSet();
  for (const Map& m1 : pieces_) {
    for (const Map& m2 : othr.pieces_) {
      Set dom_res = SET_FACT.createSet();
      Set ith_dom = m1.dom().intersection(m2.dom());
      if (!ith_dom.isEmpty()) {
        Exp e_res, e1;

        dom_res = m1.image(ith_dom);
        e1 = m1.exp();

        Set im2 = m2.image(ith_dom);
        if (!e1.isConstant()) {
          e_res = m2.exp().composition(e1.inverse());
        } else {
          e_res = MDLExp(im2.minElem());
        }

        if (!dom_res.isEmpty()) {
          Map ith(dom_res, e_res);
          UnordPWMap ith_pw(ith);
          Set again = dom_res.intersection(visited);
          if (!again.isEmpty()) {
            PWMapStratPtr aux_res = result->restrict(dom_res);
            PWMapStratPtr min_map = aux_res->minMap(ith_pw);
            PWMapStratPtr new_res = min_map->combine(ith_pw)->combine(*result);
            result = std::move(new_res);
            visited = visited.cup(ith_pw.dom());
          } else {
            result->emplaceBack(ith);
            visited = std::move(visited).disjointCup(std::move(dom_res));
          }
        }
      }
    }
  }

  return result;
}

PWMapStratPtr UnordPWMap::firstInv(const Set& subdom) const
{
  UnordMapCollection result;

  Set visited = SET_FACT.createSet();
  for (const Map& m : pieces_) {
    Set res_dom = m.image(subdom).difference(visited);
    if (!res_dom.isEmpty()) {
      Map new_map(m.preImage(res_dom), m.exp());
      result.emplace_back(new_map.minInv());

      for (const SetPiece& mdi : res_dom)
        visited.emplaceBack(mdi);
    }
  }

  return std::make_unique<UnordPWMap>(result);
}

PWMapStratPtr UnordPWMap::firstInv() const { return firstInv(dom()); }

PWMapStratPtr UnordPWMap::filterMap(bool (*f)(const Map&)) const
{
  PWMapStratPtr res = std::make_unique<UnordPWMap>();

  for (const Map& m : pieces_)
    if (f(m))
      res->emplaceBack(m);

  return res;
}

Set UnordPWMap::equalImage(const PWMapStrategy& other) const
{
  Set res = SET_FACT.createSet();

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map& m1 : pieces_) {
    for (const Map& m2 : othr.pieces_) {
      Set cap_dom = m1.dom().intersection(m2.dom());
      if (!cap_dom.isEmpty()) {
        Map m1_cap(cap_dom, m1.exp());
        Map m2_cap(cap_dom, m2.exp());
        if (m1_cap == m2_cap)
          res = res.disjointCup(cap_dom);
      }
    }
  }

  return res;
}

Set UnordPWMap::lessImage(const PWMapStrategy& other) const
{
  if (isEmpty() || other.isEmpty())
    return SET_FACT.createSet();

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  Set min_in_pw1 = SET_FACT.createSet();
  for (const Map& m1 : pieces_) {
    for (const Map& m2 : othr.pieces_) {
      min_in_pw1 = min_in_pw1.disjointCup(m1.lessImage(m2));
    }
  }

  return min_in_pw1; 
}  

Set UnordPWMap::sharedImage() const
{
  Set not_present = dom().difference(firstInv()->image());
  Set res = preImage(image(not_present));

  return res;
}

PWMapStratPtr UnordPWMap::offsetDom(const MD_NAT& off) const
{
  UnordMapCollection res;

  for (const Map& m : pieces_)
    pushBack(res, Map(m.dom().offset(off), m.exp()));

  return std::make_unique<UnordPWMap>(res);
}

PWMapStratPtr UnordPWMap::offsetDom(const PWMapStrategy& off) const
{
  PWMapStratPtr res = std::make_unique<UnordPWMap>();

  for (const Map& m : pieces_) {
    Set ith_dom = off.image(m.dom());
    res->emplaceBack(Map(ith_dom, m.exp()));
  }

  return res;
}

PWMapStratPtr UnordPWMap::offsetImage(const MD_NAT& off) const
{
  UnordMapCollection res;

  for (const Map& m : pieces_) {
    Exp e = m.exp(), res_e;
    for (unsigned int j = 0; j < e.arity(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    pushBack(res, Map(m.dom(), res_e));
  }

  return std::make_unique<UnordPWMap>(res);
}

PWMapStratPtr UnordPWMap::offsetImage(const Exp& off) const
{
  UnordMapCollection res;

  for (const Map& m : pieces_) 
    pushBack(res, Map(m.dom(), off + m.exp()));

  return std::make_unique<UnordPWMap>(res);
}

PWMapStratPtr UnordPWMap::compact() const
{
  UnordMapCollection res;
  if (dom().isEmpty())
    return std::make_unique<UnordPWMap>(res);
  
  std::forward_list<size_t> indices;
  auto liIt = indices.before_begin();

  const size_t lSize = pieces_.size();
  for (size_t i = 0; i < lSize; ++i)
    liIt = indices.insert_after(liIt, i);
  
  auto begin = pieces_.begin();
  auto liPrev = indices.before_begin();
  auto liCurr = indices.begin();
  while (liCurr != indices.end()) {
    size_t id = *liCurr;
    const Map& it = *(begin + id);
    Map new_ith(it.dom().compact(), it.exp());

    liCurr = indices.erase_after(liPrev);

    while (liCurr != indices.end()) {
      size_t idx = *liCurr;
      const Map& nextMap = *(begin + idx);

      auto ith = new_ith.compact(nextMap);
      if (ith) {
        new_ith = ith.value();
        liCurr = indices.erase_after(liPrev);
        continue;
      }
    

      ++liPrev;
      ++liCurr;
    }
    
    pushBack(res, new_ith);
    liPrev = indices.before_begin();
    liCurr = indices.begin();
  }

  return std::make_unique<UnordPWMap>(res);
}

} // namespace LIB

} // namespace SBG;

