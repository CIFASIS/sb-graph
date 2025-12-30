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

#include <iostream>
#include <forward_list>
#include <set>

#include "sbg/map_entry.hpp"
#include "sbg/dom_ord_pwmap.hpp"

namespace SBG {

namespace LIB {

using Internal::calculatePerimeter;
using Internal::doInt;
using Internal::createMapEntry;
using Internal::operator<;
using Internal::emplaceHint;
using Internal::advanceHint;

////////////////////////////////////////////////////////////////////////////////
// Domain Ordered PWMap Implementation -----------------------------------------
////////////////////////////////////////////////////////////////////////////////

// Member functions - Domain Ordered Piecewise maps ----------------------------

member_imp(DomOrdPWMap, DomOrdPWMap::OrdMapCollection, pieces);

DomOrdPWMap::DomOrdPWMap() : pieces_() {}
DomOrdPWMap::DomOrdPWMap(const Set& s) : pieces_() {
  if (!s.isEmpty()) {
    SetPiece first = s.begin().operator*();
    pieces_.push_back(createMapEntry(Map(s, Exp(first.arity()
      , LExp()))));
  }
}
DomOrdPWMap::DomOrdPWMap(const Map& m) : pieces_() {
  if (!m.isEmpty()) {
    pieces_.push_back(createMapEntry(m));
  }
}
//DomOrdPWMap::DomOrdPWMap(const OrdMapCollection& pieces)
//  : pieces_(pieces) {}
DomOrdPWMap::DomOrdPWMap(OrdMapCollection pieces)
  : pieces_(std::move(pieces)) {}
//DomOrdPWMap::DomOrdPWMap(const DomOrdPWMap& pw) : pieces_(pw.pieces_) {}
//DomOrdPWMap::DomOrdPWMap(DomOrdPWMap&& pw) : pieces_(std::move(pw.pieces_)) {}

member_imp(DomOrdPWMap::Iterator, DomOrdPWMap::OrdMapCollection::const_iterator
  , it);

DomOrdPWMap::Iterator::Iterator(OrdMapCollection::const_iterator it)
  : it_(it) {}

void DomOrdPWMap::Iterator::operator++()
{
  ++it_;
  return;
}

bool DomOrdPWMap::Iterator::operator!=(const PWMapStrategy::Iterator& other)
  const
{
  return it_ != static_cast<const DomOrdPWMap::Iterator *>(&other)->it_;
}

const Map& DomOrdPWMap::Iterator::operator*() const { return it_->first; }

std::shared_ptr<PWMapStrategy::Iterator> DomOrdPWMap::begin() const
{ 
  return std::make_shared<DomOrdPWMap::Iterator>(pieces_.begin());
}

std::shared_ptr<PWMapStrategy::Iterator> DomOrdPWMap::end() const
{
  return std::make_shared<DomOrdPWMap::Iterator>(pieces_.end());
}

void DomOrdPWMap::emplaceBack(const Map& m)
{ 
  if (!m.isEmpty()) {
    MapEntry mpe = createMapEntry(m);
    if (pieces_.empty() || pieces_.back().second.first < mpe.second.first) {
      pieces_.emplace_back(mpe);
    }
    else {
      emplaceHint(pieces_, m, 0);
    }
  }
}

bool DomOrdPWMap::operator==(const PWMapStrategy& other) const 
{ 
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);

  if (dom() != othr.dom()) {
    return false;
  }

  if (pieces_ == othr.pieces_) {
    return true;
  }

  OrdMapCollection short_pw = pieces_;
  OrdMapCollection long_pw = othr.pieces_;
  if (othr.pieces_.size() < pieces_.size()) {
    short_pw = othr.pieces_;
    long_pw  = pieces_;
  }

  // Indexes list corresponding to remaining maps in short_pw
  std::forward_list<size_t> indexes;
  int short_size = short_pw.size();
  for (int i = short_size - 1; i >= 0; --i) {
    indexes.push_front(i);
  }

  auto short_begin = short_pw.begin();
  for (const MapEntry& long_mpe : long_pw) {
    const Map& long_map = long_mpe.first;
    const SetPerimeter& long_sp = long_mpe.second; 
    
    auto prev_index = indexes.before_begin();
    auto curr_index = indexes.begin();
    while (curr_index != indexes.end()) {
      const size_t idx = *curr_index;
      const MapEntry& short_mpe = *(short_begin + idx);
      const Map& short_map = short_mpe.first;
      const SetPerimeter& short_sp = short_mpe.second; 
    
      // Here short_map is "before" long_map, so it is also "before" all the
      // remaining maps in long_pw, thus it can be discarded. 
      if (short_sp.second < long_sp.first) {
        curr_index = indexes.erase_after(prev_index);
        continue;
      }

      // Here short_map is "after" long_map, so no comparison is needed, and
      // the loop of long_pw continues to check if this short_map interacts
      // with the following elements of long_pw. 
      if (long_sp.second < short_sp.first) {
        break;
      }

      // Comparison between short_map and long_map needed.
      if (doInt(short_sp, long_sp)) {
        Set cap_dom = short_map.dom().intersection(long_map.dom());
        if (!cap_dom.isEmpty()) {
          Exp short_exp = short_map.exp();
          Exp long_exp = long_map.exp();
          if (short_exp != long_exp) {
            return false;
          }

          Map short_cap_map(cap_dom, short_exp);
          Map long_cap_map(cap_dom, long_exp);
          if (short_cap_map != long_cap_map) {
            return false;
          }
        }
      }

      ++prev_index;
      ++curr_index;
    }

    if (indexes.empty()) {
      break;
    }
  }
  
  return true;
}

bool DomOrdPWMap::operator!=(const PWMapStrategy& other) const
{ 
  return !(*this == other);
}

DomOrdPWMap& DomOrdPWMap::operator=(DomOrdPWMap&& other)
{
  pieces_ = std::move(other.pieces_);
  
  return *this;
}

PWMapStratPtr DomOrdPWMap::operator+(const PWMapStrategy& other) const
{
  Set set_in = SET_FACT.createSet();
  Set set_out = SET_FACT.createSet();
  OrdMapCollection result;
  processMapsOrd(other, set_in, set_out, result, &DomOrdPWMap::processAdd
    , false);
  return std::make_unique<DomOrdPWMap>(std::move(result));
}

void DomOrdPWMap::processAdd(const Map& m1, const Map& m2, 
  Set& set_in, Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT& global_pos) const
{   
  Map res_add = m1 + m2;
  advanceHint(ord_pwmap, calculatePerimeter(m2.dom()).first, global_pos);
  emplaceHint(ord_pwmap, res_add, global_pos);
}

std::ostream& DomOrdPWMap::print(std::ostream& out) const
{
  int sz = pieces_.size();

  out << "<<";
  if (sz > 0) {
    int i = 0;
    for (; i < sz - 1; ++i) {
      out << pieces_[i].first << ", ";
    }
    out << pieces_[i].first;
  }
  out << ">>";

  return out;
}

PWMapStratPtr DomOrdPWMap::clone() const
{
  return std::make_unique<DomOrdPWMap>(pieces_);
}

// PWMap functions -------------------------------------------------------------

std::size_t DomOrdPWMap::arity() const
{
  if (isEmpty()) {
    return 0;
  }

  return pieces_.begin()->first.dom().arity();
}

bool DomOrdPWMap::isEmpty() const 
{ 
  return pieces_.empty();
}

Set DomOrdPWMap::dom() const
{
  Set result = SET_FACT.createSet();
  for (const MapEntry& mpe : pieces_) {
    Set ith_dom = mpe.first.dom();
    result = std::move(result).disjointCup(std::move(ith_dom));
  }

  return result;
}

PWMapStratPtr DomOrdPWMap::restrict(const Set& subdom) const
{
  OrdMapCollection result;
  
  if (subdom.isEmpty() || isEmpty()) {
    return std::make_unique<DomOrdPWMap>();
  }

  // Indexes list corresponding to remaining maps in the pw
  std::forward_list<size_t> indexes;
  int sz = pieces_.size();
  for (int i = sz - 1; i >= 0; --i) {
    indexes.push_front(i);
  }

  auto begin = pieces_.begin();
  for (const SetPiece& mdi : subdom) {
    auto prev_index = indexes.before_begin();
    auto curr_index = indexes.begin();
    while (curr_index != indexes.end()) {
      size_t idx = *curr_index;
      const MapEntry& mpe = *(begin + idx);
      const Map& map = mpe.first;
      const SetPerimeter& map_sp = mpe.second; 
    
      // Here map is "before" mdi, so it is also "before" all the
      // remaining set pieces in subdom, thus it can be discarded. 
      if (map_sp.second < mdi.minElem()) {
        curr_index = indexes.erase_after(prev_index);
        continue;
      }

      // Here map is "after" mdi, so no comparison is needed, and
      // the loop of subdom continues to check if this map interacts
      // with the following elements of subdom. 
      if (mdi.maxElem() < map_sp.first) {
        break;
      }

      // Comparison between map and mdi needed.
      Set mdi_set = SET_FACT.createSet(mdi);
      SetPerimeter mdi_sp = calculatePerimeter(mdi_set);
      if (doInt(map_sp, mdi_sp)) {
        Internal::emplaceBack(result, map.restrict(mdi_set));
      }

      ++prev_index;
      ++curr_index;
    }

    if (indexes.empty()) {
      break;
    }
  }

  return std::make_unique<DomOrdPWMap>(std::move(result));
}

Set DomOrdPWMap::image() const
{
  Set result = SET_FACT.createSet();

  for (const MapEntry& mpe : pieces_) {
    Set ith_img = mpe.first.image();
    result = std::move(result).cup(std::move(ith_img));
  }

  return result;
}

Set DomOrdPWMap::image(const Set& subdom) const
{
  return restrict(subdom)->image();
}

Set DomOrdPWMap::preImage(const Set& subcodom) const
{
  Set result = SET_FACT.createSet();
  for (const MapEntry& mpe : pieces_) {
    Set ith_pre = mpe.first.preImage(subcodom);
    result = std::move(result).disjointCup(std::move(ith_pre));
  }

  return result;
}

PWMapStratPtr DomOrdPWMap::inverse() const
{ 
  OrdMapCollection result;

  for (const MapEntry& mpe : pieces_) {
    Internal::emplaceBack(result, mpe.first.minInv());  
  }
  std::sort(result.begin(), result.end(), operator<);

  return std::make_unique<DomOrdPWMap>(result);
}

PWMapStratPtr DomOrdPWMap::composition(const PWMapStrategy& other) const 
{
  OrdMapCollection result; 
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);

  NAT global_pos = 0;
  for (const MapEntry& other_mpe : othr.pieces_) {
    const Map& other_map = other_mpe.first;
    Set img = other_map.image();
    
    SetPerimeter image_sp = calculatePerimeter(img);
    MD_NAT image_max = image_sp.second;
    advanceHint(result, other_mpe.second.first, global_pos);

    for (const MapEntry& this_mpe : pieces_) {
      const Map& this_map = this_mpe.first;
      const SetPerimeter& this_sp = this_mpe.second;
      MD_NAT this_min = this_sp.first;
      
      if (doInt(this_sp, image_sp)) {
        Map composition = this_map.composition(other_map);
        if (!composition.isEmpty()) {
          emplaceHint(result, composition, global_pos);
        }
        continue;
      }

      if (image_max < this_min) {
        break;
      }
    }
  }

  return std::make_unique<DomOrdPWMap>(std::move(result));
}

PWMapStratPtr DomOrdPWMap::mapInf(unsigned int n) const
{
  PWMapStratPtr result = std::make_unique<DomOrdPWMap>(pieces_);
  PWMapStratPtr old_res = std::make_unique<DomOrdPWMap>();

  if (!dom().isEmpty()) {
    for (unsigned int j = 0; old_res != result && j < n; ++j) {
      DomOrdPWMap *rs = static_cast<DomOrdPWMap *>(result.get());
      old_res = std::make_unique<DomOrdPWMap>(rs->pieces_);
      PWMapStratPtr new_res = result->composition(*this);
      result = std::move(new_res);
    }

    if (*old_res == *result) {
      return result;
    }
    
    PWMapStratPtr reduced = result->reduce();
    result = std::move(reduced);
    do {
      DomOrdPWMap *rs = static_cast<DomOrdPWMap *>(result.get());
      old_res = std::make_unique<DomOrdPWMap>(rs->pieces_);
      
      PWMapStratPtr new_res = result->composition(*result);
      new_res = new_res->reduce();
      result = std::move(new_res);
    } while (*old_res != *result);
  }
  
  return result;
}

PWMapStratPtr DomOrdPWMap::mapInf() const { return mapInf(0); }

Set DomOrdPWMap::fixedPoints() const
{
  Set result = SET_FACT.createSet();
  for (const MapEntry& mpe : pieces_) {
    Set ith_fixed = mpe.first.fixedPoints();
    result = std::move(result).disjointCup(std::move(ith_fixed));
  }

  return result;
}

// Extra operations ------------------------------------------------------------

PWMapStratPtr DomOrdPWMap::concatenation(const PWMapStrategy& other) const
{
  OrdMapCollection res;
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);
  res.reserve(pieces_.size() + othr.pieces_.size());
  
  if (isEmpty()) {
    return std::make_unique<DomOrdPWMap>(othr.pieces_);
  }

  if (other.isEmpty()) {
    return std::make_unique<DomOrdPWMap>(pieces_);
  }
  
  if (pieces_.back().second.second < othr.pieces_.front().second.first) {
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    res.insert(res.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<DomOrdPWMap>(std::move(res));
  }
  
  if (othr.pieces_.back().second.second < pieces_.front().second.first) {
    res.insert(res.end(), othr.pieces_.begin(), othr.pieces_.end());
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<DomOrdPWMap>(std::move(res));
  }
  
  auto it1 = pieces_.begin(), it2 = othr.pieces_.begin();
  auto end1 = pieces_.end(), end2 = othr.pieces_.end();

  for (; it1 != end1 && it2 != end2;) {
    auto min_per_m1 = it1->second.first;
    auto min_per_m2 = it2->second.first;

    if (min_per_m1 < min_per_m2) {
      Internal::emplaceBack(res, *it1);
      ++it1;
    } else {
      Internal::emplaceBack(res, *it2); 
      ++it2;
    }
  }

  res.insert(res.end(), it1, end1);
  res.insert(res.end(), it2, end2);
  return std::make_unique<DomOrdPWMap>(std::move(res));
}

PWMapStratPtr DomOrdPWMap::combine(const PWMapStrategy& other) const
{
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);
  if (isEmpty())
    return std::make_unique<DomOrdPWMap>(othr.pieces_);

  if (other.isEmpty())
    return std::make_unique<DomOrdPWMap>(pieces_);

  if (pieces_ == othr.pieces_)
    return std::make_unique<DomOrdPWMap>(pieces_);

  Set exclusive_other = other.dom().difference(dom());

  return concatenation(*other.restrict(exclusive_other));
}

PWMapStratPtr DomOrdPWMap::reduce(const Interval& i, const LExp& le) const
{
  OrdMapCollection res;

  if (!i.isEmpty()) {
    RATIONAL zero(0, 1);
    if (le.slope() == 1 && le.offset() != 0) {
      INT h = le.offset().toInt();

      NAT st = i.step();
      if (h == (INT) st) {
        NAT hi = i.end();
        RATIONAL const_expr(hi + st, 1);
        if (st < Inf - hi)
          Internal::emplaceBack(res, Map(i, LExp(zero, const_expr)));
      }

      else if (h == (INT) -st) {
        NAT lo = i.begin();
        RATIONAL const_expr(lo - st, 1);
        if (lo >= st)
          Internal::emplaceBack(res, Map(i, LExp(zero, const_expr)));
      }

      else if (h % (INT) st == 0) {
        // Is convenient the partition of the piece?
        if ((INT) i.cardinal() > h*h) {
          INT absh = std::abs(h);

          for (int k = 1; k <= absh; ++k) {
            NAT new_begin = i.begin() + k - 1;
            Interval kth_piece(new_begin, (NAT) absh, i.end());

            RATIONAL kth_off;
            if (h > 0)
              kth_off = kth_piece.end() + h;
            else
              kth_off = kth_piece.begin() + h;
              
            Internal::emplaceBack(res, Map(kth_piece, LExp(0, kth_off)));
          }
        }
      }
    }

    else
      Internal::emplaceBack(res, Map(i, le));
  }

  return std::make_unique<DomOrdPWMap>(res);
}


PWMapStratPtr DomOrdPWMap::reduce(const Map& map) const
{
  OrdMapCollection res;
  Set not_reduced = SET_FACT.createSet();
  Exp e = map.exp();
  for (const SetPiece& dom_piece : map.dom()) {
    SetPiece aux_piece = dom_piece;
    Exp aux_exp = e;
    bool was_reduced = false;
    for (unsigned int j = 0; j < dom_piece.arity(); ++j) {
      PWMapStratPtr aux = reduce(dom_piece[j], e[j]); 
      DomOrdPWMap *jth_red = static_cast<DomOrdPWMap *>(aux.get());
      for (const MapEntry& mpe_ith_reduced : jth_red->pieces_) {
        const Map& ith_reduced = mpe_ith_reduced.first;
        aux_piece[j] = ith_reduced.dom().begin().operator*().operator[](0);
        aux_exp[j] = ith_reduced.exp()[0];
        if (aux_piece != dom_piece || aux_exp != e) {
          Internal::emplaceBack(res, Map(aux_piece, aux_exp));
          was_reduced = true;
        }

        aux_piece = dom_piece;
        aux_exp = e;
      }

      ++j;
    }

    if (!was_reduced) {
      not_reduced.emplaceBack(dom_piece);
    }
  }
 
  // Add not reduced subpieces 
  if (!not_reduced.isEmpty()) {
    Internal::emplaceBack(res, Map(not_reduced, e));
  }

  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::reduce() const
{ 
  OrdMapCollection res;

  for (const MapEntry& mpe : pieces_) {
    PWMapStratPtr ith = reduce(mpe.first);
    DomOrdPWMap *ith_c = static_cast<DomOrdPWMap *>(ith.get());
    for (const MapEntry& mpe_ith_elem : ith_c->pieces_) 
      Internal::emplaceBack(res, mpe_ith_elem);
  } 
  std::sort(res.begin(), res.end(), operator<);
  
  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::minMap(const PWMapStrategy& other) const
{
  if (isEmpty() || other.isEmpty())
    return std::make_unique<DomOrdPWMap>();

  Set min_in_pw1 = lessImage(other);
  return restrict(min_in_pw1)->combine(*other.restrict(dom())); 
}

PWMapStratPtr DomOrdPWMap::minAdjMap(const PWMapStrategy& other) const
{
  Set set_in = SET_FACT.createSet();
  Set set_out = SET_FACT.createSet();
  OrdMapCollection res;
  processMapsOrd(other, set_in, set_out, res, &DomOrdPWMap::processMinAdjMap, true);
  std::sort(res.begin(), res.end(), operator<); 
  return std::make_unique<DomOrdPWMap>(res);
}

void DomOrdPWMap::processMinAdjMap(const Map& m1, const Map& m2, 
  Set& set_in, Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT& global_pos) const 
{ 
  Set dom_res = SET_FACT.createSet();
  Set ith_dom = m1.dom().intersection(m2.dom());
  if (!ith_dom.isEmpty()) {
    Exp e_res, e1;

    dom_res = m1.image(ith_dom);
    e1 = m1.exp();

    Set im2 = m2.image(ith_dom);
    if (!e1.isConstant())
      e_res = m2.exp().composition(e1.inverse());
    else
      e_res = MDLExp(im2.minElem());

    if (!dom_res.isEmpty()) {
      Map ith(dom_res, e_res);
      DomOrdPWMap ith_pw(ith);
      Set again = dom_res.intersection(set_in);
      if (!again.isEmpty()) {
        DomOrdPWMap ord_pwmap_aux(ord_pwmap);
        std::sort(ord_pwmap_aux.pieces_.begin(), ord_pwmap_aux.pieces_.end(), operator<);
        PWMapStratPtr aux_res = ord_pwmap_aux.restrict(dom_res);
        PWMapStratPtr min_map = aux_res->minMap(ith_pw);
        PWMapStratPtr new_resPtr = min_map->combine(ith_pw)->combine(ord_pwmap_aux);
        DomOrdPWMapCRef new_res_c = static_cast<DomOrdPWMapCRef>(*new_resPtr);
        ord_pwmap = std::move(new_res_c.pieces_);
        Set ith_dom = ith_pw.dom();
        set_in = std::move(set_in).cup(std::move(ith_dom));
      }
      else {
        Internal::emplaceBack(ord_pwmap, ith);
        set_in = std::move(set_in).disjointCup(std::move(dom_res));
      }
    }
  }
}

PWMapStratPtr DomOrdPWMap::firstInv(const Set& subdom) const
{
  OrdMapCollection res;
  if (isEmpty() || subdom.isEmpty() )
    return std::make_unique<DomOrdPWMap>(res);

  SetPerimeter short_sp = calculatePerimeter(subdom);
  auto s_max_per = short_sp.second;
  Set visited = SET_FACT.createSet();
  
  for (const MapEntry& t_mpe : pieces_) {
    const Map& t_m = t_mpe.first;
    const SetPerimeter& t_sp = t_mpe.second;
    auto t_min_per = t_sp.first;

    if (doInt(t_sp, short_sp)) {
      Set img = t_m.image(subdom);
      
      if (!img.isEmpty()) {
        if (!visited.isEmpty()) {
          SetPerimeter i_sp = calculatePerimeter(img);
          SetPerimeter v_sp = calculatePerimeter(visited);

          if (doInt(i_sp, v_sp)) {
            Set res_dom = img.difference(visited);
            if (!res_dom.isEmpty())
              img = res_dom;
            else
              continue;
          }
        }
        Map new_map(t_m.preImage(img), t_m.exp());
        Internal::emplaceBack(res, new_map.minInv());
        visited = std::move(visited).disjointCup(std::move(img));
        continue;
      }
    }
    if (s_max_per < t_min_per) {
      break;
    }
  }

  std::sort(res.begin(), res.end(), operator<);

  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::firstInv() const { return firstInv(dom()); }

PWMapStratPtr DomOrdPWMap::filterMap(bool (*f)(const Map&)) const
{ 
  OrdMapCollection res;
  for (const MapEntry& mpe : pieces_){
    Internal::emplaceBack(res, mpe);
  }
  
  return std::make_unique<DomOrdPWMap>(res);
}

Set DomOrdPWMap::equalImage(const PWMapStrategy& other) const
{ 
  if (isEmpty() || other.isEmpty())
    return SET_FACT.createSet();

  Set set_in = SET_FACT.createSet();
  Set set_out = SET_FACT.createSet();
  OrdMapCollection unused;
  processMapsOrd(other, set_in, set_out, unused, &DomOrdPWMap::processEqualImage
    , false);
  return set_out;
}

void DomOrdPWMap::processEqualImage(const Map& m1, const Map& m2, 
  Set& set_in, Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT& global_pos) const 
{ 
  Set cap_dom = m1.dom().intersection(m2.dom());
  if (!cap_dom.isEmpty()) {
    Map m1_cap(cap_dom, m1.exp());
    Map m2_cap(cap_dom, m2.exp());
    if (m1_cap == m2_cap) {
      set_out = std::move(set_out).disjointCup(std::move(cap_dom));
    }
  }
}

Set DomOrdPWMap::lessImage(const PWMapStrategy& other) const
{
  Set set_in = SET_FACT.createSet();
  Set set_out = SET_FACT.createSet();
  OrdMapCollection unused;
  processMapsOrd(other, set_in, set_out, unused, &DomOrdPWMap::processLessImage
    , true);
  return set_out;
} 

void DomOrdPWMap::processLessImage(const Map& m1, const Map& m2, 
  Set& set_in, Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT& global_pos) const 
{
  Set ith_less = m1.lessImage(m2);
  set_out = std::move(set_out).disjointCup(std::move(ith_less));
}

void DomOrdPWMap::processMapsOrd(
  const PWMapStrategy& other,
  Set& set_in,
  Set& set_out,
  OrdMapCollection& ord_map,
  ProcessFunc process,
  bool order_mts
  ) const
{
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);

  int sz = pieces_.size();
  int othr_sz = othr.pieces_.size(); 
  ord_map.reserve(2*(sz + othr_sz));

  const DomOrdPWMap *short_pw = this;
  const DomOrdPWMap *long_pw  = &othr;
  if (!order_mts && othr_sz < sz) {
    short_pw = &othr;
    long_pw  = this;
  }

  std::forward_list<size_t> indexes;
  auto si_it = indexes.before_begin();

  const size_t short_size = short_pw->pieces_.size();
  for (size_t i = 0; i < short_size; ++i) {
    si_it = indexes.insert_after(si_it, i);
  }

  auto short_begin = short_pw->pieces_.begin();
  NAT global_pos = 0;
  for (const MapEntry& long_mpe : long_pw->pieces_ ) {
    const Map& long_map = long_mpe.first;
    const SetPerimeter& long_sp = long_mpe.second; 
    
    auto prev_index = indexes.before_begin();
    auto curr_index = indexes.begin();
    while (curr_index != indexes.end()) {
      size_t idx = *curr_index;
      const MapEntry& short_mpe = *(short_begin + idx);
      const Map& short_map = short_mpe.first;
      const SetPerimeter& short_sp = short_mpe.second; 

      // Here short_map is "before" long_map, so it is also "before" all the
      // remaining maps in long_pw, thus it can be discarded. 
      if (short_sp.second < long_sp.first) {
        curr_index = indexes.erase_after(prev_index);
        continue;
      }

      // Here short_map is "after" long_map, so no comparison is needed, and
      // the loop of long_pw continues to check if this short_map interacts
      // with the following elements of long_pw. 
      if (long_sp.second < short_sp.first) {
        break;
      }

      // Comparison between short_map and long_map needed.
      if (doInt(short_sp, long_sp)) {
        (this->*process)(short_map, long_map, set_in, set_out, ord_map
          , global_pos);
      }

      ++prev_index;
      ++curr_index;
    }

    if (indexes.empty()) {
      break;
    }
  }
}

Set DomOrdPWMap::sharedImage() const
{
  Set not_present = dom().difference(firstInv()->image());
  Set res = preImage(image(not_present));

  return res;
}

PWMapStratPtr DomOrdPWMap::offsetDom(const MD_NAT& off) const
{
  OrdMapCollection res;

  for (const MapEntry& mpe : pieces_){
    Map map(mpe.first.dom().offset(off), mpe.first.exp());
    Internal::emplaceBack(res, map);
  }

  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::offsetDom(const PWMapStrategy& off) const
{
  OrdMapCollection res;
  const SetPerimeter o_sp = calculatePerimeter(off.dom());
  const auto o_max_per = o_sp.second;
  for (const MapEntry& t_mpe : pieces_) {
    const Map& t_m = t_mpe.first;
    const SetPerimeter& t_sp = t_mpe.second;
    const auto m_min_per = t_sp.first;
    
    if (doInt(t_sp, o_sp)) {
      Set ith_dom = off.image(t_m.dom());
      
      if (!ith_dom.isEmpty()){
        Map res_map(ith_dom, t_m.exp());
        Internal::emplaceBack(res, res_map);
      }
      continue;
    }
    
    if (o_max_per <  m_min_per)
      break;
  }

  std::sort(res.begin(),res.end(), operator<);
  
  return std::make_unique<DomOrdPWMap>(res);

}

PWMapStratPtr DomOrdPWMap::offsetImage(const MD_NAT& off) const
{
    OrdMapCollection res;

  for (const MapEntry& mpe : pieces_) {
    Exp e = mpe.first.exp(), res_e;
    for (unsigned int j = 0; j < e.arity(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    Internal::emplaceBack(res, Map(mpe.first.dom(), res_e));
  }

  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::offsetImage(const Exp& off) const
{
  OrdMapCollection res;

  for (const MapEntry& mpe : pieces_) 
    Internal::emplaceBack(res, Map(mpe.first.dom(), off + mpe.first.exp()));

  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::compact() const
{
  OrdMapCollection result;

  if (!isEmpty()) {
    std::set<MapEntry> prev(pieces_.begin(), pieces_.end());
    std::set<MapEntry> actual = prev;
    do {
      prev = actual;
      actual = std::set<MapEntry>();

      std::set<MapEntry>::iterator ith = prev.begin();
      std::set<MapEntry>::iterator last = prev.end();
      std::set<MapEntry> to_erase;
      for (; ith != last; ++ith) {
        Map ith_compact = ith->first;
        std::set<MapEntry>::iterator next = ith;
        ++next;
        for (; next != last; ++next) {
          MaybeMap new_compact = ith_compact.compact(next->first);
          if (new_compact) {
            ith_compact = new_compact.value();
            to_erase.insert(*next);
          }
        }

        if (to_erase.find(*ith) == to_erase.end())
          actual.insert(createMapEntry(ith_compact));
      }
    } while (actual != prev);

    for (const MapEntry& mdi : actual) {
      result.emplace_back(mdi);
    }
  }

  return std::make_unique<DomOrdPWMap>(result);
}

} // namespace LIB

} // namespace SBG;

