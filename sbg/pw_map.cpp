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

////////////////////////////////////////////////////////////////////////////////
// PWMap Delegate Constructors -------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMapDelegate::PWMapDelegate(const MapAF &fact) : fact_(std::move(fact)) {}



// Auxiliary functions - Unordered Piecewise maps ---------------------------------------------------------

void pushBack(UnordMapCollection &unord_pw, const Map &m)
{
  unord_pw.emplace_back(m);
}

////////////////////////////////////////////////////////////////////////////////
// Unordered PWMap Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_move_imp(UnordPWMap, UnordMapCollection, pieces);

UnordPWMap::UnordPWMap(const MapAF &fact) : PWMapDelegate(fact) {}
UnordPWMap::UnordPWMap(const MapAF &fact, const Set &s)
  : PWMapDelegate(fact), pieces_() {
  if (!s.isEmpty()) {
    SetPiece first = s.begin().operator*();
    pieces_.push_back(fact_.createMap(s, Exp(first.arity(), LExp())));
  }
}
UnordPWMap::UnordPWMap(const MapAF &fact, const Map &m)
  : PWMapDelegate(fact), pieces_() {
  if (!m.dom().isEmpty())
    pieces_.push_back(m);
}
UnordPWMap::UnordPWMap(const MapAF &fact, const UnordMapCollection &pieces)
  : PWMapDelegate(fact), pieces_(std::move(pieces)) {}
UnordPWMap::UnordPWMap(const UnordPWMap &pw)
  : PWMapDelegate(pw.fact_), pieces_(pw.pieces_) {}

PWMapDelegPtr UnordPWMap::clone() const
{
  return std::make_unique<UnordPWMap>(*this);
}

member_imp(UnordPWMap::Iterator, UnordMapCollection::const_iterator, it);

UnordPWMap::Iterator::Iterator(UnordMapCollection::const_iterator it)
  : it_(it) {}

void UnordPWMap::Iterator::operator++()
{
  ++it_;
  return;
}

bool UnordPWMap::Iterator::operator!=(const PWMapDelegate::Iterator &other)
  const
{
  return it_ != static_cast<const UnordPWMap::Iterator *>(&other)->it_;
}

const Map &UnordPWMap::Iterator::operator*() const { return *it_; }

std::shared_ptr<PWMapDelegate::Iterator> UnordPWMap::begin() const
{
  return std::make_shared<UnordPWMap::Iterator>(pieces_.begin());
}

std::shared_ptr<PWMapDelegate::Iterator> UnordPWMap::end() const
{
  return std::make_shared<UnordPWMap::Iterator>(pieces_.end());
}

void UnordPWMap::emplaceBack(const Map &m)
{
  if (!m.dom().isEmpty())
    pieces_.push_back(m);
}

bool UnordPWMap::operator==(const PWMapDelegate &other) const 
{
  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);

  if (dom() != othr.dom()) 
    return false;

  if (pieces_ == othr.pieces_)
    return true;

  for (const Map &m1 : pieces_) {
    for (const Map &m2 : othr.pieces_) {
      Set dom1 = m1.dom(), dom2 = m2.dom();
      Set cap_dom = dom1.intersection(dom2);

      if (!cap_dom.isEmpty()) {
        // Here we check by image because the same image can be obtained through
        // two different lexps.
        // Example: [1:1:1] -> 10 and [1:1:1] -> x+9
        if (cap_dom.cardinal() == 1) {
          Map map1 = fact_.createMap(cap_dom, m1.exp());
          Map map2 = fact_.createMap(cap_dom, m2.exp());
          if (map1.image() != map2.image())
            return false;
        }

        // When there is more than one element we can't check equality on the
        // image because there are at least two linear pieces with the same
        // domain and image.
        // Example: [1:1:10] -> x and [1:1:10] -> -x+10
        else {
          if (m1.exp() != m2.exp())
            return false;
        }
      }
    }
  }

  return true;
}

bool UnordPWMap::operator!=(const PWMapDelegate &other) const
{ 
  return !(*this == other);
}

UnordPWMap &UnordPWMap::operator=(UnordPWMap &&other)
{
  if (this != &other)
    pieces_ = std::move(other.pieces_);

  return *this;
}

PWMapDelegPtr UnordPWMap::operator+(const PWMapDelegate &other) const
{
  UnordMapCollection res;

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map &m1 : pieces_) 
    for (const Map &m2 : othr.pieces_) 
      res.push_back(m1 + m2);

  return std::make_unique<UnordPWMap>(fact_, res);
}

PWMapDelegPtr UnordPWMap::operator-(const PWMapDelegate &other) const
{ 

  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  if (isEmpty() || other.isEmpty())
    return res;

  Interval all(0, 1, Inf);
  Set univ = fact_.createSet(SetPiece(arity(), all));

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map &m1 : pieces_) {
    for (const Map &m2 : othr.pieces_) {
      Set dom1 = m1.dom(), dom2 = m2.dom();  
      Set dom = dom1.intersection(dom2);
      if (!dom.isEmpty()) {
        Exp minus_exp = m1.exp() - m2.exp();
        UnordPWMap ith(fact_, univ);
        for (unsigned int j = 0; j < arity(); ++j) {
          RATIONAL m = minus_exp[j].slope(), h = minus_exp[j].offset();
          // Negative values in the jth dimension
          NAT begin_neg = 0, end_neg = Inf;
          // Positive values in the jth dimension
          NAT begin_pos = 0, end_pos = Inf;
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
          // Increasing expression
          else if (m > 0) {
            RATIONAL cross = -h / m;
            if (cross > 0 || cross == 0) {
              begin_pos = boost::rational_cast<NAT>(cross.value());
              if (begin_pos > 0)
                end_neg = begin_pos - 1;
              else {
                begin_neg = 1;
                begin_pos = 0;
              }
            }
          }
          // Decresing expression
          else { 
            RATIONAL cross = -h / m;
            if (cross > 0 || cross == 0) {
              end_pos = boost::rational_cast<NAT>(cross.value());
              if (end_pos > 0)
                begin_neg = end_pos + 1;
            }
            else {
              begin_pos = 1;
              end_pos = 0;
            }
          }

          UnordPWMap jth(fact_);
          Interval neg(begin_neg, 1, end_neg);
          Interval pos(begin_pos, 1, end_pos);
          for (const Map &m : ith.pieces_) {
            SetPiece mdi = *(m.dom().begin());
            Exp e = m.exp(); 

            if (!neg.isEmpty()) {
              mdi[j] = neg;
              e[j] = LExp(0, 0);
              jth.emplaceBack(fact_.createMap(mdi, e));
            }

            if (!pos.isEmpty()) {
              mdi[j] = pos;
              e[j] = minus_exp[j];
              jth.emplaceBack(fact_.createMap(mdi, e));
            }
          }

          ith = std::move(jth);
        }

        res = res->concatenation(*ith.restrict(dom));
      }
    }
  }
  
  

  
  return res;
}

std::ostream &UnordPWMap::print(std::ostream &out) const
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
  Set res = fact_.createSet();
  for (const Map &m : pieces_) {
    Set d = m.dom();
    res = res.disjointCup(d);
  }

  return res;
}

PWMapDelegPtr UnordPWMap::restrict(const Set &subdom) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  for (const Map &m : pieces_)
    res->emplaceBack(m.restrict(subdom));

  return res;
}

Set UnordPWMap::image() const
{
  Set res = fact_.createSet();

  for (const Map &m : pieces_) {
    res = res.cup(m.image());
  }

  return res;
}

Set UnordPWMap::image(const Set &subdom) const
{
  return restrict(subdom)->image();
}

Set UnordPWMap::preImage(const Set &subcodom) const
{
  Set res = fact_.createSet();

  for (const Map &m : pieces_)
    res = res.disjointCup(m.preImage(subcodom));

  return res;
}

PWMapDelegPtr UnordPWMap::inverse() const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  for (const Map &m : pieces_)
    res->emplaceBack(m.minInv());

  return res;
}

PWMapDelegPtr UnordPWMap::composition(const PWMapDelegate &other) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);

  for (const Map &m1 : pieces_){ 
    for (const Map &m2 : othr.pieces_)
      res->emplaceBack(m1.composition(m2));
  }
  return res;
}

PWMapDelegPtr UnordPWMap::mapInf(unsigned int n) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(*this);
  PWMapDelegPtr old_res = std::make_unique<UnordPWMap>(fact_);

  if (!dom().isEmpty()) {
    for (unsigned int j = 0; old_res != res && j < n; ++j) {
      UnordPWMap *rs = static_cast<UnordPWMap *>(res.get());
      old_res = std::make_unique<UnordPWMap>(*rs);
      PWMapDelegPtr new_res = res->composition(*this);
      res = std::move(new_res);
    }

    if (*old_res == *res)
      return res;

    PWMapDelegPtr reduced = res->reduce();
    res = std::move(reduced);
    do {
      UnordPWMap *rs = static_cast<UnordPWMap *>(res.get());
      old_res = std::make_unique<UnordPWMap>(*rs);

      PWMapDelegPtr new_res = res->composition(*res);
      new_res = new_res->reduce();
      res = std::move(new_res);
    } while (*old_res != *res);
  }

  return res;
}

PWMapDelegPtr UnordPWMap::mapInf() const { return mapInf(0); }

// Extra operations ------------------------------------------------------------

PWMapDelegPtr UnordPWMap::concatenation(const PWMapDelegate &other) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(*this);

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map &m2 : othr.pieces_)
    res->emplaceBack(m2);

  return res;
}

PWMapDelegPtr UnordPWMap::combine(const PWMapDelegate &other) const
{ 
  
  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);

  if (isEmpty())
    return std::make_unique<UnordPWMap>(othr);

  if (other.isEmpty())
    return std::make_unique<UnordPWMap>(*this);

  PWMapDelegPtr res = std::make_unique<UnordPWMap>(*this);
  Set dom1 = dom();
  for (const Map &m2 : othr.pieces_) {
    Set dom2 = m2.dom(), new_dom = dom2.difference(dom1);
    res->emplaceBack(fact_.createMap(new_dom, m2.exp()));
  }

  return res;
}

PWMapDelegPtr UnordPWMap::reduce(const Interval &i, const LExp &le) const
{
  UnordMapCollection res;

  if (!i.isEmpty()) {
    RATIONAL zero(0, 1);
    if (le.slope() == 1 && le.offset() != 0) {
      INT h = le.offset().toInt();

      NAT st = i.step();
      if (h == (INT) st) {
        NAT hi = i.end();
        RATIONAL const_expr(hi + st, 1);
        if (st < Inf - hi)
          pushBack(res, fact_.createMap(i, LExp(zero, const_expr)));
      }

      else if (h == (INT) -st) {
        NAT lo = i.begin();
        RATIONAL const_expr(lo - st, 1);
        if (lo >= st)
          pushBack(res, fact_.createMap(i, LExp(zero, const_expr)));
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

            pushBack(res, fact_.createMap(kth_piece, LExp(0, kth_off)));
          }
        }
      }
    }

    else
      pushBack(res, fact_.createMap(i, le));
  }

  return std::make_unique<UnordPWMap>(fact_, res);
}

PWMapDelegPtr UnordPWMap::reduce(const Map &map) const
{
  UnordMapCollection res;

  Set not_reduced = fact_.createSet();
  Exp e = map.exp();
  for (const SetPiece &dom_piece : map.dom()) {
    SetPiece aux_piece = dom_piece;
    Exp aux_exp = e;
    bool was_reduced = false;
    for (unsigned int j = 0; j < dom_piece.arity(); ++j) {
      PWMapDelegPtr aux = reduce(dom_piece[j], e[j]); 
      UnordPWMap *jth_red = static_cast<UnordPWMap *>(aux.get());
      for (const Map &ith_reduced : jth_red->pieces_) {
        aux_piece[j] = ith_reduced.dom().begin().operator*().operator[](0);
        aux_exp[j] = ith_reduced.exp()[0];
        if (aux_piece != dom_piece || aux_exp != e) {
          pushBack(res, fact_.createMap(aux_piece, aux_exp));
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

  pushBack(res, fact_.createMap(not_reduced, e)); // Add unreduced subpieces

  return std::make_unique<UnordPWMap>(fact_, res);
}

PWMapDelegPtr UnordPWMap::reduce() const
{
  UnordMapCollection res;
  for (const Map &m : pieces_) {
    PWMapDelegPtr ith = reduce(m);
    UnordPWMap *ith_c = static_cast<UnordPWMap *>(ith.get());
    for(const Map &mi : ith_c->pieces_)
      pushBack(res, mi); 
  }
      
  return std::make_unique<UnordPWMap>(fact_, res);
}

PWMapDelegPtr UnordPWMap::minMap(const PWMapDelegate &other) const
{
  
  if (isEmpty() || other.isEmpty())
    return std::make_unique<UnordPWMap>(fact_);
  
  PWMapDelegPtr aux1 = restrict(other.dom()), aux2 = other.restrict(dom());
  Set zero = fact_.createSet(SetPiece(arity(), Interval(0, 1, 0)));
  
  Set to_zero = (*aux1 - *aux2)->preImage(zero);
  
  Set not_zero = aux2->dom().difference(to_zero);
   
  aux1 = aux1->restrict(to_zero);
  aux2 = aux2->restrict(not_zero);

  return aux1->combine(*aux2);
}

PWMapDelegPtr UnordPWMap::minAdjMap(const PWMapDelegate &other) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  Set visited = fact_.createSet();
  for (const Map &m1 : pieces_) {
    for (const Map &m2 : othr.pieces_) {
      Set dom_res = fact_.createSet();
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
          Map ith = fact_.createMap(dom_res, e_res);
          UnordPWMap ith_pw(fact_, ith);
          Set again = dom_res.intersection(visited);
          if (!again.isEmpty()) {
            PWMapDelegPtr aux_res = res->restrict(dom_res);
            PWMapDelegPtr min_map = aux_res->minMap(ith_pw);
            PWMapDelegPtr new_res = min_map->combine(ith_pw)->combine(*res);
            res = std::move(new_res);
            visited = visited.cup(ith_pw.dom());
          }
          else {
            res->emplaceBack(ith);
            visited = visited.disjointCup(dom_res);
          }
        }
      }
    }
  }
  return res;
}

PWMapDelegPtr UnordPWMap::firstInv(const Set &subdom) const
{
  UnordMapCollection res;

  Set visited = fact_.createSet();
  for (const Map &m : pieces_) {
    Set res_dom = m.image(subdom).difference(visited);
    if (!res_dom.isEmpty()) {
      Map new_map = fact_.createMap(m.preImage(res_dom), m.exp());
      pushBack(res, new_map.minInv());

      visited = visited.disjointCup(res_dom);
    }
  }

  return std::make_unique<UnordPWMap>(fact_, res);
}

PWMapDelegPtr UnordPWMap::firstInv() const { return firstInv(dom()); }

PWMapDelegPtr UnordPWMap::filterMap(bool (*f)(const Map &)) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  for (const Map &m : pieces_)
    if (f(m))
      res->emplaceBack(m);

  return res;
}

Set UnordPWMap::equalImage(const PWMapDelegate &other) const
{
  Set res = fact_.createSet();

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map &m1 : pieces_) {
    for (const Map &m2 : othr.pieces_) {
      Set cap_dom = m1.dom().intersection(m2.dom());
      if (!cap_dom.isEmpty()) {
        Map m1_cap = fact_.createMap(cap_dom, m1.exp());
        Map m2_cap = fact_.createMap(cap_dom, m2.exp());
        if (m1_cap == m2_cap)
          res = res.disjointCup(cap_dom);
      }
    }
  }

  return res;
}

Set UnordPWMap::sharedImage() const
{
  Set not_present = dom().difference(firstInv()->image());
  Set res = preImage(image(not_present));

  return res;
}

PWMapDelegPtr UnordPWMap::offsetDom(const MD_NAT &off) const
{
  UnordMapCollection res;

  for (const Map &m : pieces_)
    pushBack(res, fact_.createMap(m.dom().offset(off), m.exp()));

  return std::make_unique<UnordPWMap>(fact_, res);
}

PWMapDelegPtr UnordPWMap::offsetDom(const PWMapDelegate &off) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  for (const Map &m : pieces_) {
    Set ith_dom = off.image(m.dom());
    res->emplaceBack(fact_.createMap(ith_dom, m.exp()));
  }

  return res;
}

PWMapDelegPtr UnordPWMap::offsetImage(const MD_NAT &off) const
{
  UnordMapCollection res;

  for (const Map &m : pieces_) {
    Exp e = m.exp(), res_e;
    for (unsigned int j = 0; j < e.arity(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    pushBack(res, fact_.createMap(m.dom(), res_e));
  }

  return std::make_unique<UnordPWMap>(fact_, res);
}

PWMapDelegPtr UnordPWMap::offsetImage(const Exp &off) const
{
  UnordMapCollection res;

  for (const Map &m : pieces_) 
    pushBack(res, fact_.createMap(m.dom(), off + m.exp()));

  return std::make_unique<UnordPWMap>(fact_, res);
}

PWMapDelegPtr UnordPWMap::compact() const
{
  UnordMapCollection res;
  if (dom().isEmpty())
    return std::make_unique<UnordPWMap>(fact_, res);
  
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
      const Map &it = *(begin + id);
      Map new_ith = fact_.createMap(it.dom().compact(), it.exp());

      liCurr = indices.erase_after(liPrev);

    
      while (liCurr != indices.end()) {
        size_t idx = *liCurr;
        const Map &nextMap = *(begin + idx);

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

  return std::make_unique<UnordPWMap>(fact_, res);
}


//--------------------------------------------------------------------------------------

// Auxiliary functions - Ordered Piecewise maps ---------------------------------------------------------

SetPerimeter calculatePerimeter(const Set &s)
{
  MD_NAT max_per(s.arity(), 0);
  MD_NAT min_per(s.arity(), Inf); 
  
  for (const SetPiece &mdi : s) {
    MD_NAT candidate_max = mdi.maxElem();
    MD_NAT candidate_min = mdi.minElem();
    
    for (std::size_t i = 0; i < max_per.arity(); ++i) {
      max_per[i] = std::max(max_per[i], candidate_max[i]);
      min_per[i] = std::min(min_per[i], candidate_min[i]);
    }
  }
  
  return {min_per, max_per};
}

bool doInt(const SetPerimeter &p1, const SetPerimeter &p2)
{
  const auto max_per_p1 = p1.second;
  const auto min_per_p1 = p1.first;
  const auto max_per_p2 = p2.second;
  const auto min_per_p2 = p2.first;
  const unsigned int arity = max_per_p1.arity();

  for (unsigned int j = 0; j < arity; ++j) {
    if (max_per_p1[j] < min_per_p2[j] || max_per_p2[j] < min_per_p1[j]) {
      return false;  // No intersection
    }
  }
  
  return true;  // Intersection detected
}


MapEntry createMapEntry(const Map &m)
{
  SetPerimeter sp = calculatePerimeter(m.dom());
  return {m, sp}; 
}


bool mapEntryComp(const MapEntry &mpe1, const MapEntry &mpe2) 
{
  return mpe1.second.first < mpe2.second.first;
}

void pushBack(OrdMapCollection &ord_pw, const MapEntry &m)
{
  ord_pw.emplace_back(m);
}

void pushBack(OrdMapCollection &ord_pw, const Map &m)
{
  ord_pw.emplace_back(createMapEntry(m));
}

void emplaceHint(OrdMapCollection &ord_pw, const Map &m, NAT hint)
{
  auto end = ord_pw.end();
  auto it = ord_pw.begin();
  std::advance(it,hint);
  MapEntry mpe = createMapEntry(m);
  while (it != end){
    if (it->second.first < mpe.second.first){
      ++it;
    }
    else
      break;
  }

  ord_pw.insert(it, mpe);
}


void advanceHint(OrdMapCollection &ord_pw, const MD_NAT crit, NAT &hint)
{
  auto end = ord_pw.end();
  auto it = ord_pw.begin();
  std::advance(it,hint);
  while (it != end){
    if (it->second.first < crit){
      ++it;
      ++hint;
    }
    else
      break;
     
  }  
}

////////////////////////////////////////////////////////////////////////////////
// Ordered PWMap Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(OrdPWMap, OrdMapCollection, pieces);

OrdPWMap::OrdPWMap(const MapAF &fact) : PWMapDelegate(fact) {}
OrdPWMap::OrdPWMap(const MapAF &fact, const Set &s)
  : PWMapDelegate(fact), pieces_(){
  if (!s.isEmpty()) {
    SetPiece first = s.begin().operator*();
    pieces_.push_back(createMapEntry(fact_.createMap(s, Exp(first.arity(), LExp()))));
  }
}
OrdPWMap::OrdPWMap(const MapAF &fact, const Map &m)
  : PWMapDelegate(fact), pieces_(){
  if (!m.isEmpty()){
    pieces_.push_back(createMapEntry(m));
  }
}
OrdPWMap::OrdPWMap(const MapAF &fact, const OrdMapCollection &pieces)
  : PWMapDelegate(fact), pieces_(std::move(pieces)){}
OrdPWMap::OrdPWMap(const OrdPWMap &pw)
  : PWMapDelegate(pw.fact_), pieces_(pw.pieces_){}

PWMapDelegPtr OrdPWMap::clone() const
{
  return std::make_unique<OrdPWMap>(*this);
}

member_imp(OrdPWMap::Iterator, OrdMapCollection::const_iterator, it);

OrdPWMap::Iterator::Iterator(OrdMapCollection::const_iterator it)
  : it_(it) {}

void OrdPWMap::Iterator::operator++()
{
  ++it_;
  return;
}

bool OrdPWMap::Iterator::operator!=(const PWMapDelegate::Iterator &other)
  const
{
  return it_ != static_cast<const OrdPWMap::Iterator *>(&other)->it_;
}

const Map &OrdPWMap::Iterator::operator*() const { return it_->first; }

std::shared_ptr<PWMapDelegate::Iterator> OrdPWMap::begin() const
{ 
  return std::make_shared<OrdPWMap::Iterator>(pieces_.begin());
}

std::shared_ptr<PWMapDelegate::Iterator> OrdPWMap::end() const
{
  return std::make_shared<OrdPWMap::Iterator>(pieces_.end());
}

void OrdPWMap::emplaceBack(const Map &m)
{ 
  if (!m.isEmpty()){
    MapEntry mpe = createMapEntry(m);
    if (pieces_.empty() || pieces_.back().second.first < mpe.second.first) 
      pieces_.push_back(mpe);
    else
      emplaceHint(pieces_, m, 0);
  }
}

bool OrdPWMap::operator==(const PWMapDelegate &other) const 
{ 
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);

  if (dom() != othr.dom()) 
    return false;

  if (pieces_ == othr.pieces_)
    return true;
  
  const OrdPWMap *short_pwmap = this;
  const OrdPWMap *long_pwmap  = &othr;
  if (othr.pieces_.size() < pieces_.size()) {
    short_pwmap = &othr;
    long_pwmap  = this;
  }

  std::forward_list<size_t> indexes;
  auto si_it = indexes.before_begin();

  const size_t short_size = short_pwmap->pieces_.size();
  for (size_t i = 0; i < short_size; ++i)
    si_it = indexes.insert_after(si_it, i);

  auto short_begin = short_pwmap->pieces_.begin();

  for(const MapEntry &l_mpe : long_pwmap->pieces_ ) {
    const Map &l_m = l_mpe.first;
    const SetPerimeter &l_sp = l_mpe.second; 
    
    auto si_prev = indexes.before_begin();
    auto si_curr = indexes.begin();

    while (si_curr != indexes.end()) {
      size_t idx = *si_curr;
      const MapEntry &s_mpe = *(short_begin + idx);
      const Map &s_m = s_mpe.first;
      const SetPerimeter &s_sp = s_mpe.second; 
      
      if (s_sp.second[0] < l_sp.first[0]) {
        si_curr = indexes.erase_after(si_prev);
        continue;
      }

      if (l_sp.second[0] < s_sp.first[0])
        break;

      if (doInt(s_sp, l_sp)) {
        auto cap_dom = s_m.dom().intersection(l_m.dom());
        if(!cap_dom.isEmpty()){
          // Here we check by image because the same image can be obtained through
          // two different lexps.
          // Example: [1:1:1] -> 10 and [1:1:1] -> x+9
          if (cap_dom.cardinal() == 1) {
            Map map1 = fact_.createMap(cap_dom, l_m.exp());
            Map map2 = fact_.createMap(cap_dom, s_m.exp());
            if (map1.image() != map2.image())
              return false;
          }

          // When there is more than one element we can't check equality on the
          // image because there are at least two linear pieces with the same
          // domain and image.
          // Example: [1:1:10] -> x and [1:1:10] -> -x+10
          else {
            if (s_m.exp() != l_m.exp())
              return false;
          }
        }
 
      }

      ++si_prev;
      ++si_curr;
    }
    if (indexes.empty())
      break;
  }
  
  return true;
}

bool OrdPWMap::operator!=(const PWMapDelegate &other) const
{ 
  return !(*this == other);
}

OrdPWMap &OrdPWMap::operator=(OrdPWMap &&other)
{
  if (this != &other)
    pieces_ = std::move(other.pieces_);
  
  return *this;
}

PWMapDelegPtr OrdPWMap::operator+(const PWMapDelegate &other) const
{
  Set set_in = fact_.createSet();
  Set set_out = fact_.createSet();
  OrdMapCollection res;
  processMapsOrd(other,set_in, set_out, res, &OrdPWMap::processAdd,false);
  return std::make_unique<OrdPWMap>(fact_, res);
}

void OrdPWMap::processAdd(const Map &m1, const Map &m2, 
  Set &set_in, Set &set_out, 
  OrdMapCollection &ord_pwmap,
  NAT &global_pos) const
{   
  auto res_add = m1 + m2;
  if (!res_add.dom().isEmpty()){
     advanceHint(ord_pwmap, calculatePerimeter(m2.dom()).first, global_pos);
     emplaceHint(ord_pwmap, res_add, global_pos);
  }                                                                                  
}


PWMapDelegPtr OrdPWMap::operator-(const PWMapDelegate &other) const
{
  OrdMapCollection res;

  if (isEmpty() || other.isEmpty())
    return std::make_unique<OrdPWMap>(fact_, res);

  Interval all(0, 1, Inf);
  Set set_in = fact_.createSet(SetPiece(arity(), all));
  Set set_out = fact_.createSet(SetPiece(arity(), all));
  processMapsOrd(other,set_in, set_out, res, &OrdPWMap::processMinus,true);
  return std::make_unique<OrdPWMap>(fact_, res);
}

void OrdPWMap::processMinus(const Map &m1, const Map &m2, 
  Set &set_in, Set &set_out, 
  OrdMapCollection &ord_pwmap,
  NAT &global_pos) const
{ 
  Set dom = m1.dom().intersection(m2.dom());
  if (dom.isEmpty())
    return;
  
  Exp minus_exp = m1.exp() - m2.exp();
  OrdPWMap ith(fact_, set_in);
  
  for (unsigned int j = 0, n = arity(); j < n; ++j) {
    RATIONAL m = minus_exp[j].slope();
    RATIONAL h = minus_exp[j].offset();
    
    NAT begin_neg = 0, end_neg = Inf;
    NAT begin_pos = 0, end_pos = Inf;
    
    if (m == 0) { 
      if (h < 0) {
        begin_pos = 1;
        end_pos = 0;
      } else {
        begin_neg = 1;
        end_neg = 0;
      }
    }
    else if (m > 0) {  
      RATIONAL cross = -h / m;
      if (cross > 0 || cross == 0) {
        begin_pos = boost::rational_cast<NAT>(cross.value());
        if (begin_pos > 0)
          end_neg = begin_pos - 1;
        else {
          begin_neg = 1;
          end_neg   = 0;
        }
      }
    }
    else { 
      RATIONAL cross = -h / m;
      if (cross > 0 || cross == 0) {
        end_pos = boost::rational_cast<NAT>(cross.value());
        if (end_pos > 0)
          begin_neg = end_pos + 1;
      } else {
        begin_pos = 1;
        end_pos = 0;
      }
    }
    
    OrdPWMap jth(fact_);
    Interval neg(begin_neg, 1, end_neg);
    Interval pos(begin_pos, 1, end_pos);
    for (const MapEntry &mpe: ith.pieces_) {
      const Map &m = mpe.first;
      SetPiece mdi = *(m.dom().begin());
      Exp e = m.exp(); 

      if (!neg.isEmpty()) {
        mdi[j] = neg;
        e[j] = LExp(0, 0);
        jth.emplaceBack(fact_.createMap(mdi, e));
      }

      if (!pos.isEmpty()) {
        mdi[j] = pos;
        e[j] = minus_exp[j];
        jth.emplaceBack(fact_.createMap(mdi, e));
      }
    }

    ith = std::move(jth);
  }
   
  OrdPWMap res_pwmap(fact_, ord_pwmap); 
  PWMapDelegPtr concatenated_ptr = res_pwmap.concatenation(*ith.restrict(dom)); 
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(*concatenated_ptr);
  ord_pwmap = std::move(othr.pieces_);
}

std::ostream &OrdPWMap::print(std::ostream &out) const
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

// PWMap functions -------------------------------------------------------------

std::size_t OrdPWMap::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->first.dom().arity();
}

bool OrdPWMap::isEmpty() const 
{ 
  return pieces_.empty();
}

Set OrdPWMap::dom() const
{
  Set res = fact_.createSet();
  for (const MapEntry &mpe : pieces_) {
    Set d = mpe.first.dom();
    res = res.disjointCup(d);
  }

  return res;
}


PWMapDelegPtr OrdPWMap::restrict(const Set &subdom) const
{
  OrdMapCollection res;
  
  if (subdom.isEmpty())
    return std::make_unique<OrdPWMap>(fact_, res);
    
  NAT global_pos = 0;
  
  SetPerimeter s_sp = calculatePerimeter(subdom);
  auto s_max_per = s_sp.second;
  
  for (const MapEntry &mpe : pieces_) {
    const Map &m = mpe.first;
    const SetPerimeter &m_sp = mpe.second;
    auto m_min_per = m_sp.first;
    
    if (doInt(m_sp, s_sp)) {
      Map res_rest = m.restrict(subdom);
      if (!res_rest.isEmpty()) {
       advanceHint(res, m_min_per, global_pos);
       emplaceHint(res, res_rest, global_pos);
      }
      continue;
    }
    
    if (s_max_per[0] < m_min_per[0])
      break;


  }
  return std::make_unique<OrdPWMap>(fact_, res);
}

Set OrdPWMap::image() const
{
  Set res = fact_.createSet();

  for (const MapEntry &mpe : pieces_)
    res = res.cup(mpe.first.image());

  return res;
}

Set OrdPWMap::image(const Set &subdom) const
{
  return restrict(subdom)->image();
}

Set OrdPWMap::preImage(const Set &subcodom) const
{
  Set res = fact_.createSet();

  for (const MapEntry &mpe : pieces_)
    res = res.disjointCup(mpe.first.preImage(subcodom));

  return res;
}

PWMapDelegPtr OrdPWMap::inverse() const
{ 
  OrdMapCollection res;

  for (const MapEntry &mpe : pieces_){
    const Map &inv = mpe.first.minInv();
    if(!inv.isEmpty())
      pushBack(res, inv);  
  }
  
  std::sort(res.begin(),res.end(), mapEntryComp);

  return std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::composition(const PWMapDelegate &other) const 
{
  OrdMapCollection res; 
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);

  NAT global_pos = 0;

  for (const MapEntry &o_mpe : othr.pieces_) {
    const auto &o_m = o_mpe.first;
    auto img = o_m.image();
    
    SetPerimeter i_sp = calculatePerimeter(img);
    auto i_max_per = i_sp.second;

    advanceHint(res, o_mpe.second.first, global_pos);

    for (const MapEntry &t_mpe : pieces_) {
      const auto &t_m = t_mpe.first;
      const SetPerimeter &t_sp = t_mpe.second;
      auto t_min_per = t_sp.first;
      
      if (doInt(t_sp, i_sp)) {
        auto res_com = t_m.composition(o_m);
        if (!res_com.isEmpty())
          emplaceHint(res, res_com, global_pos);
        continue;
      }

      if (i_max_per[0] < t_min_per[0])
        break;
    }
  }

  return std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::mapInf(unsigned int n) const
{
  PWMapDelegPtr res = std::make_unique<OrdPWMap>(*this);
  PWMapDelegPtr old_res = std::make_unique<OrdPWMap>(fact_);

  if (!dom().isEmpty()) {
    for (unsigned int j = 0; old_res != res && j < n; ++j) {
      OrdPWMap *rs = static_cast<OrdPWMap *>(res.get());
      old_res = std::make_unique<OrdPWMap>(*rs);
      PWMapDelegPtr new_res = res->composition(*this);
      res = std::move(new_res);
    }

    if (*old_res == *res)
      return res;
    
    PWMapDelegPtr reduced = res->reduce();
    res = std::move(reduced);
    do {
      OrdPWMap *rs = static_cast<OrdPWMap *>(res.get());
      old_res = std::make_unique<OrdPWMap>(*rs);
      
      
      PWMapDelegPtr new_res = res->composition(*res);
      
      new_res = new_res->reduce();
      
      res = std::move(new_res);
    } while (*old_res != *res);
  }
  
  return res;
}

PWMapDelegPtr OrdPWMap::mapInf() const { return mapInf(0); }

// Extra operations ------------------------------------------------------------


PWMapDelegPtr OrdPWMap::concatenation(const PWMapDelegate &other) const
{
  OrdMapCollection res;
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);
  res.reserve(pieces_.size() + othr.pieces_.size());
  
  if (isEmpty())
    return std::make_unique<OrdPWMap>(othr);

  if (other.isEmpty())
    return std::make_unique<OrdPWMap>(*this);
  
  if (pieces_.back().second.first < othr.pieces_.front().second.first) {
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    res.insert(res.end(), othr.pieces_.begin(), othr.pieces_.end());
    return std::make_unique<OrdPWMap>(fact_, res);
  }
  
  if (othr.pieces_.back().second.first < pieces_.front().second.first) {
    res.insert(res.end(), othr.pieces_.begin(), othr.pieces_.end());
    res.insert(res.end(), pieces_.begin(), pieces_.end());
    return std::make_unique<OrdPWMap>(fact_, res);
  }
  
  auto it1 = pieces_.begin(), it2 = othr.pieces_.begin();
  auto end1 = pieces_.end(), end2 = othr.pieces_.end();
  

  for (; it1 != end1 && it2 != end2;) {
    auto min_per_m1 = it1->second.first;
    auto min_per_m2 = it2->second.first;

    if (min_per_m1 < min_per_m2) {
      pushBack(res, *it1);
      ++it1;
    } else {
      pushBack(res, *it2); 
      ++it2;
    }
  }

  res.insert(res.end(), it1, end1);
  res.insert(res.end(), it2, end2);

  return std::make_unique<OrdPWMap>(fact_, res);
}


PWMapDelegPtr OrdPWMap::combine(const PWMapDelegate &other) const
{
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);

  if (isEmpty())
    return std::make_unique<OrdPWMap>(othr);

  if (other.isEmpty())
    return std::make_unique<OrdPWMap>(*this);

  OrdMapCollection res = pieces_;

  NAT global_pos = 0;
  
  SetPerimeter t_sp = calculatePerimeter(dom());

  for (const MapEntry &o_mpe : othr.pieces_) {
    const Map &o_m = o_mpe.first;
    const SetPerimeter &o_sp = o_mpe.second;
    Map res_comb = fact_.createMap(o_m.dom(), o_m.exp());
    
    if (doInt(o_sp, t_sp)){
      Set dom_o_m = o_m.dom(), new_dom = dom_o_m.difference(dom());
      res_comb = fact_.createMap(new_dom, o_m.exp());
      
      if (res_comb.isEmpty())
        continue;
    }
    
    advanceHint(res, o_sp.first, global_pos);
    emplaceHint(res, res_comb, global_pos);
  }
  
  return  std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::reduce(const Interval &i, const LExp &le) const
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
          pushBack(res, fact_.createMap(i, LExp(zero, const_expr)));
      }

      else if (h == (INT) -st) {
        NAT lo = i.begin();
        RATIONAL const_expr(lo - st, 1);
        if (lo >= st)
          pushBack(res, fact_.createMap(i, LExp(zero, const_expr)));
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
              
            pushBack(res, fact_.createMap(kth_piece, LExp(0, kth_off)));
          }
        }
      }
    }

    else
      pushBack(res, fact_.createMap(i, le));
  }

  return std::make_unique<OrdPWMap>(fact_, res);
}


PWMapDelegPtr OrdPWMap::reduce(const Map &map) const
{
  OrdMapCollection res;
  Set not_reduced = fact_.createSet();
  Exp e = map.exp();
  for (const SetPiece &dom_piece : map.dom()) {
    SetPiece aux_piece = dom_piece;
    Exp aux_exp = e;
    bool was_reduced = false;
    for (unsigned int j = 0; j < dom_piece.arity(); ++j) {
      PWMapDelegPtr aux = reduce(dom_piece[j], e[j]); 
      OrdPWMap *jth_red = static_cast<OrdPWMap *>(aux.get());
      for (const MapEntry &mpe_ith_reduced : jth_red->pieces_) {
        const Map &ith_reduced = mpe_ith_reduced.first;
        aux_piece[j] = ith_reduced.dom().begin().operator*().operator[](0);
        aux_exp[j] = ith_reduced.exp()[0];
        if (aux_piece != dom_piece || aux_exp != e) {
          pushBack(res, fact_.createMap(aux_piece, aux_exp));
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
    pushBack(res, fact_.createMap(not_reduced, e)); // Add unreduced subpieces

  return std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::reduce() const
{ 
  OrdMapCollection res;
  for (const MapEntry &mpe : pieces_) {
    PWMapDelegPtr ith = reduce(mpe.first);
    OrdPWMap *ith_c = static_cast<OrdPWMap *>(ith.get());
    for (const MapEntry &mpe_ith_elem : ith_c->pieces_) 
      pushBack(res, mpe_ith_elem);
  } 

  std::sort(res.begin(), res.end(), mapEntryComp);
  
  return std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::minMap(const PWMapDelegate &other) const
{
  if (isEmpty() || other.isEmpty())
    return std::make_unique<OrdPWMap>(fact_);

  PWMapDelegPtr aux1 = restrict(other.dom()), aux2 = other.restrict(dom());
  Set zero = fact_.createSet(SetPiece(arity(), Interval(0, 1, 0)));
  Set to_zero = (*aux1 - *aux2)->preImage(zero);
  Set not_zero = aux2->dom().difference(to_zero);
  aux1 = aux1->restrict(to_zero);
  aux2 = aux2->restrict(not_zero);
  return aux1->combine(*aux2);
}



PWMapDelegPtr OrdPWMap::minAdjMap(const PWMapDelegate &other) const
{
  Set set_in = fact_.createSet();
  Set set_out = fact_.createSet();
  OrdMapCollection res;
  processMapsOrd(other, set_in, set_out, res, &OrdPWMap::processMinAdjMap,true);
  std::sort(res.begin(), res.end(), mapEntryComp); 
  return std::make_unique<OrdPWMap>(fact_, res);
}

void OrdPWMap::processMinAdjMap(const Map &m1, const Map &m2, 
  Set &set_in, Set &set_out, 
  OrdMapCollection &ord_pwmap,
  NAT &global_pos) const 
{ 
  Set dom_res = fact_.createSet();
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
      Map ith = fact_.createMap(dom_res, e_res);
      OrdPWMap ith_pw(fact_, ith);
      Set again = dom_res.intersection(set_in);
      if (!again.isEmpty()) {
        OrdPWMap ord_pwmap_aux(fact_, ord_pwmap);
        std::sort(ord_pwmap_aux.pieces_.begin(), ord_pwmap_aux.pieces_.end(), mapEntryComp);
        PWMapDelegPtr aux_res = ord_pwmap_aux.restrict(dom_res);
        PWMapDelegPtr min_map = aux_res->minMap(ith_pw);
        PWMapDelegPtr new_resPtr = min_map->combine(ith_pw)->combine(ord_pwmap_aux);
        OrdPWMapCRef new_res_c = static_cast<OrdPWMapCRef>(*new_resPtr);
        ord_pwmap = std::move(new_res_c.pieces_);
        set_in = set_in.cup(ith_pw.dom());
      }
      else {
        pushBack(ord_pwmap, ith);
        set_in = set_in.disjointCup(dom_res);
      }
    }
  }
}

PWMapDelegPtr OrdPWMap::firstInv(const Set &subdom) const
{
  OrdMapCollection res;
  if (isEmpty() || subdom.isEmpty() )
    return std::make_unique<OrdPWMap>(fact_, res);

  SetPerimeter s_sp = calculatePerimeter(subdom);
  auto s_max_per = s_sp.second;
  Set visited = fact_.createSet();
  
  for (const MapEntry &t_mpe : pieces_) {
    const Map &t_m = t_mpe.first;
    const SetPerimeter &t_sp = t_mpe.second;
    auto t_min_per = t_sp.first;

    
    if (doInt(t_sp, s_sp)) {
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
        Map new_map = fact_.createMap(t_m.preImage(img), t_m.exp());
        pushBack(res, new_map.minInv());
        visited = visited.disjointCup(img);
        continue;
      }
    }
    if (s_max_per[0] < t_min_per[0]) break;
  }

  std::sort(res.begin(), res.end(), mapEntryComp);

  return std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::firstInv() const { return firstInv(dom()); }

PWMapDelegPtr OrdPWMap::filterMap(bool (*f)(const Map &)) const
{ 
  OrdMapCollection res;
  for (const MapEntry &mpe : pieces_){
    if (f(mpe.first))
      pushBack(res, mpe);
  }
  
  return std::make_unique<OrdPWMap>(fact_, res);
}

Set OrdPWMap::equalImage(const PWMapDelegate &other) const
{ 
  Set set_in = fact_.createSet();
  Set set_out = fact_.createSet();
  OrdMapCollection no_used;
  processMapsOrd(other, set_in, set_out, no_used, &OrdPWMap::processEqualImage,false);
  return set_out;
}

void OrdPWMap::processEqualImage(const Map &m1, const Map &m2, 
  Set &set_in, Set &set_out, 
  OrdMapCollection &ord_pwmap,
  NAT &global_pos) const 
{ 
  Set cap_dom = m1.dom().intersection(m2.dom());
  if (!cap_dom.isEmpty()) {
    Map m1_cap = fact_.createMap(cap_dom, m1.exp());
    Map m2_cap = fact_.createMap(cap_dom, m2.exp());
    if (m1_cap == m2_cap)
      set_out = set_out.disjointCup(cap_dom);
  }
}

void OrdPWMap::processMapsOrd(
  const PWMapDelegate &other,
  Set &set_in,
  Set &set_out,
  OrdMapCollection  &ord_map,
  ProcessFunc process,
  bool order_mts
  ) const
{
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);

  const OrdPWMap *short_pwmap = this;
  const OrdPWMap *long_pwmap  = &othr;
  if (!order_mts && othr.pieces_.size() < pieces_.size()) {
    short_pwmap = &othr;
    long_pwmap  = this;
  }

  std::forward_list<size_t> indexes;
  auto si_it = indexes.before_begin();

  const size_t short_size = short_pwmap->pieces_.size();
  for (size_t i = 0; i < short_size; ++i)
    si_it = indexes.insert_after(si_it, i);

  auto short_begin = short_pwmap->pieces_.begin();
  NAT global_pos = 0;
  
  for(const MapEntry &l_mpe : long_pwmap->pieces_ ) {
    const Map &l_m = l_mpe.first;
    const SetPerimeter &l_sp = l_mpe.second; 
    
    auto si_prev = indexes.before_begin();
    auto si_curr = indexes.begin();

    while (si_curr != indexes.end()) {
      size_t idx = *si_curr;
      const MapEntry &s_mpe = *(short_begin + idx);
      const Map &s_m = s_mpe.first;
      const SetPerimeter &s_sp = s_mpe.second; 
      
      if (s_sp.second[0] < l_sp.first[0]) {
        si_curr = indexes.erase_after(si_prev);
        continue;
      }

      if (l_sp.second[0] < s_sp.first[0])
        break;

      // Process overlapping perimeters
      if (doInt(s_sp, l_sp)) {
        (this->*process)(s_m, l_m, set_in, set_out, ord_map, global_pos);
      }

      ++si_prev;
      ++si_curr;
    }
    if (indexes.empty())
      break;
  }
  
}


Set OrdPWMap::sharedImage() const
{
  Set not_present = dom().difference(firstInv()->image());
  Set res = preImage(image(not_present));

  return res;
}

PWMapDelegPtr OrdPWMap::offsetDom(const MD_NAT &off) const
{
  OrdMapCollection res;

  for (const MapEntry &mpe : pieces_){
    Map map = fact_.createMap(mpe.first.dom().offset(off), mpe.first.exp());
    if(!map.isEmpty())
      pushBack(res, map);
  }

  return std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::offsetDom(const PWMapDelegate &off) const
{
  OrdMapCollection res;
  const SetPerimeter o_sp = calculatePerimeter(off.dom());
  const auto o_max_per = o_sp.second;
  for (const MapEntry &t_mpe : pieces_) {
    const Map &t_m = t_mpe.first;
    const SetPerimeter &t_sp = t_mpe.second;
    const auto m_min_per = t_sp.first;
    
    if (doInt(t_sp, o_sp)) {
      Set ith_dom = off.image(t_m.dom());
      
      if (!ith_dom.isEmpty()){
          Map res_map = fact_.createMap(ith_dom, t_m.exp());
          pushBack(res, res_map);
      }
      continue;
    }
    
    if (o_max_per[0] <  m_min_per[0])
      break;
  }

  std::sort(res.begin(),res.end(), mapEntryComp);
  
  return std::make_unique<OrdPWMap>(fact_, res);

}

PWMapDelegPtr OrdPWMap::offsetImage(const MD_NAT &off) const
{
    OrdMapCollection res;

  for (const MapEntry &mpe : pieces_) {
    Exp e = mpe.first.exp(), res_e;
    for (unsigned int j = 0; j < e.arity(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    pushBack(res, fact_.createMap(mpe.first.dom(), res_e));
  }

  return std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::offsetImage(const Exp &off) const
{
  OrdMapCollection res;

  for (const MapEntry &mpe : pieces_) 
    pushBack(res, fact_.createMap(mpe.first.dom(), off + mpe.first.exp()));

  return std::make_unique<OrdPWMap>(fact_, res);
}

PWMapDelegPtr OrdPWMap::compact() const
{
  OrdMapCollection res;
  if (dom().isEmpty())
    return std::make_unique<OrdPWMap>(fact_, res);
  
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
      const MapEntry &mpe = *(begin + id);
      const Map &m = mpe.first;
      Map new_m = fact_.createMap(m.dom().compact(), m.exp());

      li_curr = indexes.erase_after(li_prev);
    
      while (li_curr != indexes.end()) {
        size_t idx = *li_curr;
        const MapEntry &next_mpe = *(begin + idx);
        const Map &next_map = next_mpe.first;

        auto comp = new_m.compact(next_map);
        if (comp) {
          new_m = comp.value();
          li_curr = indexes.erase_after(li_prev);
          continue;
        }
    
        ++li_prev;
        ++li_curr;
      }
      
      pushBack(res, new_m);
      li_prev = indexes.before_begin();
      li_curr = indexes.begin();
  }

  return std::make_unique<OrdPWMap>(fact_, res);
}

 
////////////////////////////////////////////////////////////////////////////////
// PWMap Implementation --------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap::PWMap(PWMapDelegPtr deleg) : delegate_(std::move(deleg)) {}
PWMap::PWMap(const PWMap &other)
  : delegate_(other.delegate_ ? other.delegate_->clone() : nullptr) {}

PWMap::Iterator::Iterator(std::shared_ptr<PWMapDelegate::Iterator> it)
  : it_(std::move(it)) {}

void PWMap::Iterator::operator++()
{
  ++(*it_);
  return;
}

Map PWMap::Iterator::operator*() const { return **it_; }

bool PWMap::Iterator::operator!=(const Iterator &other) const
{
  return *it_ != *other.it_;
}

PWMap::Iterator PWMap::begin() const { return delegate_->begin(); }
PWMap::Iterator PWMap::end() const { return delegate_->end(); }

void PWMap::emplaceBack(const Map &m)
{
  delegate_->emplaceBack(m);
  return;
}

bool PWMap::operator==(const PWMap &other) const
{
  return *delegate_ == *other.delegate_;
}

bool PWMap::operator!=(const PWMap &other) const { return !(*this == other); }

PWMap &PWMap::operator=(const PWMap &other)
{
  if (this != &other)
    delegate_ = other.delegate_->clone();

  return *this;
}

PWMap &PWMap::operator=(PWMap &&other)
{
  if (this != &other)
    delegate_ = std::move(other.delegate_);

  return *this;
}

std::ostream &PWMap::print(std::ostream &out) const
{
  delegate_->print(out);
  return out;
}

std::ostream &operator<<(std::ostream &out, const PWMap &pw)
{
  pw.print(out);
  return out;
}

PWMap PWMap::operator+(const PWMap &other) const
{
  return delegate_->operator+(*other.delegate_);
}

PWMap PWMap::operator-(const PWMap &other) const
{
  return delegate_->operator-(*other.delegate_);
}

std::size_t PWMap::arity() const { return delegate_->arity(); }

bool PWMap::isEmpty() const { return delegate_->isEmpty(); }

Set PWMap::dom() const { return delegate_->dom(); }

PWMap PWMap::restrict(const Set &subdom) const
{
  return delegate_->restrict(subdom);
}

Set PWMap::image() const { return delegate_->image(); }

Set PWMap::image(const Set &subdom) const { return delegate_->image(subdom); }

Set PWMap::preImage(const Set &subcodom) const
{
  return delegate_->preImage(subcodom);
}

PWMap PWMap::inverse() const { return delegate_->inverse(); }

PWMap PWMap::composition(const PWMap &other) const
{
  return delegate_->composition(*other.delegate_);
}

PWMap PWMap::mapInf(unsigned int n) const { return delegate_->mapInf(n); }

PWMap PWMap::mapInf() const { return delegate_->mapInf(); }

PWMap PWMap::concatenation(const PWMap &other) const
{
  return delegate_->concatenation(*other.delegate_);
}

PWMap PWMap::combine(const PWMap &other) const
{
  return delegate_->combine(*other.delegate_);
}

PWMap PWMap::reduce() const { return delegate_->reduce(); }

PWMap PWMap::minMap(const PWMap &other) const
{
  return delegate_->minMap(*other.delegate_);
}

PWMap PWMap::minAdjMap(const PWMap &other) const
{
  return delegate_->minAdjMap(*other.delegate_);
}

PWMap PWMap::firstInv(const Set &subdom) const
{
  return delegate_->firstInv(subdom);
}

PWMap PWMap::firstInv() const { return delegate_->firstInv(); }

PWMap PWMap::filterMap(bool (*f)(const Map &)) const
{
  return delegate_->filterMap(f);
}

Set PWMap::equalImage(const PWMap &other) const
{
  return delegate_->equalImage(*other.delegate_);
}

Set PWMap::sharedImage() const { return delegate_->sharedImage(); }

PWMap PWMap::offsetDom(const MD_NAT &off) const
{
  return delegate_->offsetDom(off);
}

PWMap PWMap::offsetDom(const PWMap &off) const
{
  return delegate_->offsetDom(*off.delegate_);
}

PWMap PWMap::offsetImage(const MD_NAT &off) const
{
  return delegate_->offsetImage(off);
}

PWMap PWMap::offsetImage(const Exp &off) const
{
  return delegate_->offsetImage(off);
}

PWMap PWMap::compact() const { return delegate_->compact(); }

} // namespace LIB

} // namespace SBG;

