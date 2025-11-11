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
  if (!m.isEmpty()){
    pieces_.push_back(createMapEntry(m));
  }
}
DomOrdPWMap::DomOrdPWMap(const OrdMapCollection& pieces)
  : pieces_(std::move(pieces)) {}
DomOrdPWMap::DomOrdPWMap(const DomOrdPWMap& pw) : pieces_(pw.pieces_) {}

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
  if (!m.isEmpty()){
    MapEntry mpe = createMapEntry(m);
    if (pieces_.empty() || pieces_.back().second.first < mpe.second.first) 
      pieces_.push_back(mpe);
    else
      emplaceHint(pieces_, m, 0);
  }
}

bool DomOrdPWMap::operator==(const PWMapStrategy& other) const 
{ 
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);

  if (dom() != othr.dom()) 
    return false;

  if (pieces_ == othr.pieces_)
    return true;

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
      if (long_sp.second < short_sp.first)
        break;

      // Comparison between short_map and long_map needed.
      if (doInt(short_sp, long_sp)) {
        Set cap_dom = short_map.dom().intersection(long_map.dom());
        Map short_cap_map(cap_dom, short_map.exp());
        Map long_cap_map(cap_dom, long_map.exp());
        if (short_cap_map != long_cap_map)
          return false;
      }

      ++prev_index;
      ++curr_index;
    }

    if (indexes.empty())
      break;
  }
  
  return true;
}

bool DomOrdPWMap::operator!=(const PWMapStrategy& other) const
{ 
  return !(*this == other);
}

DomOrdPWMap& DomOrdPWMap::operator=(DomOrdPWMap&& other)
{
  if (this != &other)
    pieces_ = std::move(other.pieces_);
  
  return *this;
}

PWMapStratPtr DomOrdPWMap::operator+(const PWMapStrategy& other) const
{
  Set set_in = SET_FACT.createSet();
  Set set_out = SET_FACT.createSet();
  OrdMapCollection res;
  processMapsOrd(other,set_in, set_out, res, &DomOrdPWMap::processAdd, false);
  return std::make_unique<DomOrdPWMap>(res);
}

void DomOrdPWMap::processAdd(const Map& m1, const Map& m2, 
  Set& set_in, Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT global_pos) const
{   
  auto res_add = m1 + m2;
  if (!res_add.dom().isEmpty()) {
    advanceHint(ord_pwmap, calculatePerimeter(m2.dom()).first, global_pos);
    emplaceHint(ord_pwmap, res_add, global_pos);
  }                                                                                  
}

PWMapStratPtr DomOrdPWMap::operator-(const PWMapStrategy& other) const
{
  OrdMapCollection res;

  if (isEmpty() || other.isEmpty())
    return std::make_unique<DomOrdPWMap>( res);

  Interval univ_one_dim(0, 1, Inf);
  Set set_in = SET_FACT.createSet(SetPiece(arity(), univ_one_dim));
  Set set_out = SET_FACT.createSet(SetPiece(arity(), univ_one_dim));
  processMapsOrd(other, set_in, set_out, res, &DomOrdPWMap::processMinus, true);
  std::sort(res.begin(), res.end(), operator<);
  return std::make_unique<DomOrdPWMap>(res);
}

void DomOrdPWMap::processMinus(const Map& m1, const Map& m2, 
  Set& set_in, Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT global_pos) const
{ 
  Set dom = m1.dom().intersection(m2.dom());
  if (dom.isEmpty())
    return;
  
  Exp minus_exp = m1.exp() - m2.exp();
  DomOrdPWMap ith(set_in);
  
  for (unsigned int j = 0, n = arity(); j < n; ++j) {
    RATIONAL m = minus_exp[j].slope();
    RATIONAL h = minus_exp[j].offset();
    
    NAT begin_neg = 0, end_neg = Inf;
    NAT begin_pos = 0, end_pos = Inf;

    // Constant expression 
    if (m == 0) { 
      if (h < 0) {
        begin_pos = 1;
        end_pos = 0;
      } else {
        begin_neg = 1;
        end_neg = 0;
      }
    }
    // Increasing expression
    else if (m > 0) {
      RATIONAL cross = -h/m;
      if (cross > 0 || cross == 0) {
        begin_pos = boost::rational_cast<NAT>(cross.value()) + 1;
        if (begin_pos > 0)
          end_neg = begin_pos - 1;
        else {
          begin_neg = 1;
          end_neg = 0;
        }
      }
      else {
        begin_neg = 1;
        end_neg = 0;
      }
    }
    // Decreasing expression
    else { 
      RATIONAL cross = -h/m;
      if (cross > 0 || cross == 0) {
        end_pos = boost::rational_cast<NAT>(cross.value());
        if (end_pos > 0)
          begin_neg = end_pos + 1;
        else {
          begin_pos = 1;
          end_pos = 0;
        }
      }
      else {
        begin_pos = 1;
        end_pos = 0;
      }
    }
    
    DomOrdPWMap jth;
    Interval neg(begin_neg, 1, end_neg);
    Interval pos(begin_pos, 1, end_pos);
    for (const MapEntry& mpe: ith.pieces_) {
      const Map& m = mpe.first;
      SetPiece mdi = *(m.dom().begin());
      Exp e = m.exp(); 

      if (!neg.isEmpty()) {
        mdi[j] = neg;
        e[j] = LExp(0, 0);
        Internal::emplaceBack(jth.pieces_, Map(mdi, e));
      }

      if (!pos.isEmpty()) {
        mdi[j] = pos;
        e[j] = minus_exp[j];
        Internal::emplaceBack(jth.pieces_, Map(mdi, e));
      }
    }

    ith = std::move(jth);
  }
   
  std::sort(ith.pieces_.begin(), ith.pieces_.end(), operator<);
  PWMapStratPtr new_ith_ptr = ith.restrict(dom); 
  DomOrdPWMapCRef new_ith = static_cast<DomOrdPWMapCRef>(*new_ith_ptr);
  for (const MapEntry& e : new_ith.pieces_)
    Internal::emplaceBack(ord_pwmap, e);
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
  return std::make_unique<DomOrdPWMap>(*this);
}

// PWMap functions -------------------------------------------------------------

std::size_t DomOrdPWMap::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->first.dom().arity();
}

bool DomOrdPWMap::isEmpty() const 
{ 
  return pieces_.empty();
}

Set DomOrdPWMap::dom() const
{
  Set res = SET_FACT.createSet();
  for (const MapEntry& mpe : pieces_) {
    res = res.disjointCup(mpe.first.dom());
  }

  return res;
}

PWMapStratPtr DomOrdPWMap::restrict(const Set& subdom) const
{
  OrdMapCollection res;
  
  if (subdom.isEmpty())
    return std::make_unique<DomOrdPWMap>(res);
    
  NAT global_pos = 0;
  
  SetPerimeter short_sp = calculatePerimeter(subdom);
  auto s_max_per = short_sp.second;
  
  for (const MapEntry& mpe : pieces_) {
    const Map& m = mpe.first;
    const SetPerimeter& m_sp = mpe.second;
    auto m_min_per = m_sp.first;
    
    if (doInt(m_sp, short_sp)) {
      Map res_rest = m.restrict(subdom);
      if (!res_rest.isEmpty()) {
       advanceHint(res, m_min_per, global_pos);
       emplaceHint(res, res_rest, global_pos);
      }
      continue;
    }
    
    if (s_max_per < m_min_per)
      break;
  }

  return std::make_unique<DomOrdPWMap>(res);
}

Set DomOrdPWMap::image() const
{
  Set res = SET_FACT.createSet();

  for (const MapEntry& mpe : pieces_)
    res = res.cup(mpe.first.image());

  return res;
}

Set DomOrdPWMap::image(const Set& subdom) const
{
  return restrict(subdom)->image();
}

Set DomOrdPWMap::preImage(const Set& subcodom) const
{
  Set res = SET_FACT.createSet();

  for (const MapEntry& mpe : pieces_)
    res = res.disjointCup(mpe.first.preImage(subcodom));

  return res;
}

PWMapStratPtr DomOrdPWMap::inverse() const
{ 
  OrdMapCollection res;

  for (const MapEntry& mpe : pieces_){
    const Map& inv = mpe.first.minInv();
    if(!inv.isEmpty())
      Internal::emplaceBack(res, inv);  
  }
  
  std::sort(res.begin(),res.end(), operator<);

  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::composition(const PWMapStrategy& other) const 
{
  OrdMapCollection res; 
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);

  NAT global_pos = 0;

  for (const MapEntry& o_mpe : othr.pieces_) {
    const auto& o_m = o_mpe.first;
    auto img = o_m.image();
    
    SetPerimeter i_sp = calculatePerimeter(img);
    auto i_max_per = i_sp.second;

    advanceHint(res, o_mpe.second.first, global_pos);

    for (const MapEntry& t_mpe : pieces_) {
      const auto& t_m = t_mpe.first;
      const SetPerimeter& t_sp = t_mpe.second;
      auto t_min_per = t_sp.first;
      
      if (doInt(t_sp, i_sp)) {
        auto res_com = t_m.composition(o_m);
        if (!res_com.isEmpty())
          emplaceHint(res, res_com, global_pos);
        continue;
      }

      if (i_max_per < t_min_per)
        break;
    }
  }

  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::mapInf(unsigned int n) const
{
  PWMapStratPtr res = std::make_unique<DomOrdPWMap>(*this);
  PWMapStratPtr old_res = std::make_unique<DomOrdPWMap>();

  if (!dom().isEmpty()) {
    for (unsigned int j = 0; old_res != res && j < n; ++j) {
      DomOrdPWMap *rs = static_cast<DomOrdPWMap *>(res.get());
      old_res = std::make_unique<DomOrdPWMap>(*rs);
      PWMapStratPtr new_res = res->composition(*this);
      res = std::move(new_res);
    }

    if (*old_res == *res)
      return res;
    
    PWMapStratPtr reduced = res->reduce();
    res = std::move(reduced);
    do {
      DomOrdPWMap *rs = static_cast<DomOrdPWMap *>(res.get());
      old_res = std::make_unique<DomOrdPWMap>(*rs);
      
      
      PWMapStratPtr new_res = res->composition(*res);
      
      new_res = new_res->reduce();
      
      res = std::move(new_res);
    } while (*old_res != *res);
  }
  
  return res;
}

PWMapStratPtr DomOrdPWMap::mapInf() const { return mapInf(0); }

Set DomOrdPWMap::fixedPoints() const
{
  Set res = SET_FACT.createSet();

  for (const MapEntry& entry : pieces_)
    res = res.disjointCup(entry.first.fixedPoints());

  return res;
}

// Extra operations ------------------------------------------------------------

PWMapStratPtr DomOrdPWMap::concatenation(const PWMapStrategy& other) const
{
  OrdMapCollection res;
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);
  res.reserve(pieces_.size() + othr.pieces_.size());
  
  if (isEmpty())
    return std::make_unique<DomOrdPWMap>(othr);

  if (other.isEmpty())
    return std::make_unique<DomOrdPWMap>(*this);
  
  if (pieces_.back().second.first < othr.pieces_.front().second.first) {
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    res.insert(res.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<DomOrdPWMap>(res);
  }
  
  if (othr.pieces_.back().second.first < pieces_.front().second.first) {
    res.insert(res.end(), othr.pieces_.begin(), othr.pieces_.end());
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<DomOrdPWMap>(res);
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

  return std::make_unique<DomOrdPWMap>(res);
}


PWMapStratPtr DomOrdPWMap::combine(const PWMapStrategy& other) const
{
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);

  if (isEmpty())
    return std::make_unique<DomOrdPWMap>(othr);

  if (other.isEmpty())
    return std::make_unique<DomOrdPWMap>(*this);

  OrdMapCollection res = pieces_;

  NAT global_pos = 0;
  
  SetPerimeter t_sp = calculatePerimeter(dom());

  for (const MapEntry& o_mpe : othr.pieces_) {
    const Map& o_m = o_mpe.first;
    const SetPerimeter& o_sp = o_mpe.second;
    Map res_comb(o_m.dom(), o_m.exp());
    
    if (doInt(o_sp, t_sp)){
      Set dom_o_m = o_m.dom(), new_dom = dom_o_m.difference(dom());
      if (new_dom.isEmpty())
        continue;
      res_comb = Map(new_dom, o_m.exp());
    }
    
    advanceHint(res, o_sp.first, global_pos);
    emplaceHint(res, res_comb, global_pos);
  }
  
  return  std::make_unique<DomOrdPWMap>(res);
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

    if (!was_reduced)
      not_reduced.emplaceBack(dom_piece);
  }
  
  if(!not_reduced.isEmpty())
    Internal::emplaceBack(res, Map(not_reduced, e)); // Add unreduced subpieces

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

  PWMapStratPtr aux1 = restrict(other.dom()), aux2 = other.restrict(dom());

  Set min_in_pw1 = SET_FACT.createSet();
  PWMapStratPtr off1 = aux1->offsetImage(MD_NAT(arity(), 1)); 
  PWMapStratPtr subt = (*off1 - *aux2);
  SetPiece im(arity(), Interval(0, 1, Inf));
  for (unsigned int k = 0; k < arity(); ++k) {
    im[k] = Interval(0, 1, 0);
    if (k > 0)
      im[k-1] = Interval(1, 1, 1);

    Set kth = subt->preImage(SET_FACT.createSet(im));
    min_in_pw1 = min_in_pw1.disjointCup(kth);
  }

  return aux1->restrict(min_in_pw1)->combine(*aux2);
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
  NAT global_pos) const 
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
        set_in = set_in.cup(ith_pw.dom());
      }
      else {
        Internal::emplaceBack(ord_pwmap, ith);
        set_in = set_in.disjointCup(dom_res);
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
        visited = visited.disjointCup(img);
        continue;
      }
    }
    if (s_max_per < t_min_per) break;
  }

  std::sort(res.begin(), res.end(), operator<);

  return std::make_unique<DomOrdPWMap>(res);
}

PWMapStratPtr DomOrdPWMap::firstInv() const { return firstInv(dom()); }

PWMapStratPtr DomOrdPWMap::filterMap(bool (*f)(const Map& )) const
{ 
  OrdMapCollection res;
  for (const MapEntry& mpe : pieces_){
    if (f(mpe.first))
      Internal::emplaceBack(res, mpe);
  }
  
  return std::make_unique<DomOrdPWMap>(res);
}

Set DomOrdPWMap::equalImage(const PWMapStrategy& other) const
{ 
  Set set_in = SET_FACT.createSet();
  Set set_out = SET_FACT.createSet();
  OrdMapCollection no_used;
  processMapsOrd(other, set_in, set_out, no_used, &DomOrdPWMap::processEqualImage
    , false);
  return set_out;
}

void DomOrdPWMap::processEqualImage(const Map& m1, const Map& m2, 
  Set& set_in, Set& set_out, 
  OrdMapCollection& ord_pwmap,
  NAT global_pos) const 
{ 
  Set cap_dom = m1.dom().intersection(m2.dom());
  if (!cap_dom.isEmpty()) {
    Map m1_cap(cap_dom, m1.exp());
    Map m2_cap(cap_dom, m2.exp());
    if (m1_cap == m2_cap)
      set_out = set_out.disjointCup(cap_dom);
  }
}

void DomOrdPWMap::processMapsOrd(
  const PWMapStrategy& other,
  Set& set_in,
  Set& set_out,
  OrdMapCollection & ord_map,
  ProcessFunc process,
  bool order_mts
  ) const
{
  DomOrdPWMapCRef othr = static_cast<DomOrdPWMapCRef>(other);

  const DomOrdPWMap *short_pw = this;
  const DomOrdPWMap *long_pw  = &othr;
  if (!order_mts && othr.pieces_.size() < pieces_.size()) {
    short_pw = &othr;
    long_pw  = this;
  }

  std::forward_list<size_t> indexes;
  auto si_it = indexes.before_begin();

  const size_t short_size = short_pw->pieces_.size();
  for (size_t i = 0; i < short_size; ++i)
    si_it = indexes.insert_after(si_it, i);

  auto short_begin = short_pw->pieces_.begin();
  NAT global_pos = 0;
  
  for(const MapEntry& long_mpe : long_pw->pieces_ ) {
    const Map& long_map = long_mpe.first;
    const SetPerimeter& long_sp = long_mpe.second; 
    
    auto prev_index = indexes.before_begin();
    auto curr_index = indexes.begin();

    while (curr_index != indexes.end()) {
      size_t idx = *curr_index;
      const MapEntry& short_mpe = *(short_begin + idx);
      const Map& s_m = short_mpe.first;
      const SetPerimeter& short_sp = short_mpe.second; 
      
      if (short_sp.second < long_sp.first) {
        curr_index = indexes.erase_after(prev_index);
        continue;
      }

      if (long_sp.second < short_sp.first)
        break;

      // Process overlapping perimeters
      if (doInt(short_sp, long_sp)) {
        (this->*process)(s_m, long_map, set_in, set_out, ord_map, global_pos);
      }

      ++prev_index;
      ++curr_index;
    }
    if (indexes.empty())
      break;
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
    if(!map.isEmpty())
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
  OrdMapCollection res;
  if (dom().isEmpty())
    return std::make_unique<DomOrdPWMap>(res);
  
  std::forward_list<size_t> indexes;
  auto li_it = indexes.before_begin();

  const size_t size = pieces_.size();
  for (size_t i = 0; i < size; ++i)
    li_it = indexes.insert_after(li_it, i);

  auto begin = pieces_.begin();
  auto li_prev = indexes.before_begin();
  auto li_curr = indexes.begin();
  
  while (li_curr != indexes.end()) {
      size_t id = *li_curr;
      const MapEntry& mpe = *(begin + id);
      const Map& m = mpe.first;
      Map new_m(m.dom().compact(), m.exp());

      li_curr = indexes.erase_after(li_prev);
    
      while (li_curr != indexes.end()) {
        size_t idx = *li_curr;
        const MapEntry& next_mpe = *(begin + idx);
        const Map& next_map = next_mpe.first;

        auto comp = new_m.compact(next_map);
        if (comp) {
          new_m = comp.value();
          li_curr = indexes.erase_after(li_prev);
          continue;
        }
    
        ++li_prev;
        ++li_curr;
      }
      
      Internal::emplaceBack(res, new_m);
      li_prev = indexes.before_begin();
      li_curr = indexes.begin();
  }

  return std::make_unique<DomOrdPWMap>(res);
}

} // namespace LIB

} // namespace SBG;

