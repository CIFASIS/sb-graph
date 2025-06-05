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

/*
PWMapDelegPtr UnordPWMap::composition(const PWMapDelegate &other) const
{
  //std::cout << "composition\n";
  UnordPWMap res(fact_); 
  
  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  
    UnordPWMap t(fact_), o(fact_);
  for(const Map& m : pieces_)
    t.emplaceBack(m);
  for(const Map& m : othr.pieces_)
    o.emplaceBack(m);
  std::sort(t.pieces_.begin(),t.pieces_.end());
    std::sort(o.pieces_.begin(),o.pieces_.end());

  //unsigned int posGlobal = 0;
  for (const Map &mO : o.pieces_) {
    
    const auto mOMin = mO.image().minElem();
    const auto mOMax = mO.image().maxElem();
    
    //posGlobal = res.advanceHint(mO.dom().minElem(),posGlobal);
  
    for (const Map &mT : t.pieces_) {
      const auto mTMin = mT.dom().minElem();
      const auto mTMax = mT.dom().maxElem();
  
      if (mTMax[0] < mOMin[0]) {
        continue;
      }
  
      if (mOMax[0] < mTMin[0])
        break;
              
      if (!(mTMax.menorThan(mOMin)) && !(mOMax.menorThan(mTMin)) ) {
          auto resCom = mT.composition(mO);
  
          if (!resCom.dom().isEmpty()){
               
                res.emplaceBack(resCom);
            
          }

      }

    }
    
  }
    //if(!res.isOrdered()){
    //std::cout << "composition\n";
    //res.print(std::cout);}
  return std::make_unique<UnordPWMap>(res);
}
*/


PWMapDelegPtr UnordPWMap::composition(const PWMapDelegate &other) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  //Set im = othr.image(), new_dom = othr.preImage(im.intersection(dom()));

  //PWMapDelegPtr aux = othr.restrict(new_dom);
 
  //UnordPWMap *other_restricted = static_cast<UnordPWMap *>(aux.get());
  for (const Map &m1 : pieces_){ 
    //for (const Map &m2 : other_restricted->pieces_)
    for (const Map &m2 : othr.pieces_)
      res->emplaceBack(m1.composition(m2));}

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
  UnordPWMap res(fact_);

  if (!i.isEmpty()) {
    RATIONAL zero(0, 1);
    if (le.slope() == 1 && le.offset() != 0) {
      INT h = le.offset().toInt();

      NAT st = i.step();
      if (h == (INT) st) {
        NAT hi = i.end();
        RATIONAL const_expr(hi + st, 1);
        if (st < Inf - hi)
          res = UnordPWMap(fact_, fact_.createMap(i, LExp(zero, const_expr)));
      }

      else if (h == (INT) -st) {
        NAT lo = i.begin();
        RATIONAL const_expr(lo - st, 1);
        if (lo >= st)
          res = UnordPWMap(fact_, fact_.createMap(i, LExp(zero, const_expr)));
      }

      else if (h % (INT) st == 0) {
        // Is convenient the partition of the piece?
        if ((INT) i.cardinal() > h*h) {
          res = UnordPWMap(fact_);
          INT absh = std::abs(h);

          for (int k = 1; k <= absh; ++k) {
            NAT new_begin = i.begin() + k - 1;
            Interval kth_piece(new_begin, (NAT) absh, i.end());

            RATIONAL kth_off;
            if (h > 0)
              kth_off = kth_piece.end() + h;
            else
              kth_off = kth_piece.begin() + h;

            res.emplaceBack(fact_.createMap(kth_piece, LExp(0, kth_off)));
          }
        }
      }
    }

    else
      res.emplaceBack(fact_.createMap(i, le));
  }

  return std::make_unique<UnordPWMap>(res);
}

PWMapDelegPtr UnordPWMap::reduce(const Map &map) const
{
  UnordPWMap res(fact_);

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
          res.emplaceBack(fact_.createMap(aux_piece, aux_exp));
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

  res.emplaceBack(fact_.createMap(not_reduced, e)); // Add unreduced subpieces

  return std::make_unique<UnordPWMap>(res);
}

PWMapDelegPtr UnordPWMap::reduce() const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);
  for (const Map &m : pieces_) {
    PWMapDelegPtr ith = reduce(m);
    
    res = res->concatenation(*ith); 
    

    
  }
      
  return res;
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
  UnordPWMap res(fact_);

  Set visited = fact_.createSet();
  for (const Map &m : pieces_) {
    Set res_dom = m.image(subdom).difference(visited);
    if (!res_dom.isEmpty()) {
      Map new_map = fact_.createMap(m.preImage(res_dom), m.exp());
      res.emplaceBack(new_map.minInv());

      visited = visited.cup(m.image(subdom));
    }
  }

  return std::make_unique<UnordPWMap>(res);
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
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  for (const Map &m : pieces_)
    res->emplaceBack(fact_.createMap(m.dom().offset(off), m.exp()));

  return res;
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
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  for (const Map &m : pieces_) {
    Exp e = m.exp(), res_e;
    for (unsigned int j = 0; j < e.arity(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    res->emplaceBack(fact_.createMap(m.dom(), res_e));
  }

  return res;
}

PWMapDelegPtr UnordPWMap::offsetImage(const Exp &off) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(fact_);

  for (const Map &m : pieces_) 
    res->emplaceBack(fact_.createMap(m.dom(), off + m.exp()));

  return res;
}

PWMapDelegPtr UnordPWMap::compact() const
{
  UnordPWMap res(fact_);
  //std::cout << "compact\n";  
  if (dom().isEmpty())
    return std::make_unique<UnordPWMap>(res);
  
  std::forward_list<size_t> indices;
  auto liIt = indices.before_begin();

  const size_t lSize = pieces_.size();
  for (size_t i = 0; i < lSize; ++i)
    liIt = indices.insert_after(liIt, i);

  
  auto begin = pieces_.begin();
  //unsigned int posInit=0;
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
      
      res.pieces_.emplace_back(new_ith);
      liPrev = indices.before_begin();
      liCurr = indices.begin();
  }
  //if(!res.isOrdered()){
    //std::cout << "compact\n";
    //res.print(std::cout);
    //}
  return std::make_unique<UnordPWMap>(res);
}
/*
PWMapDelegPtr UnordPWMap::compact() const
{
  UnordPWMap res(fact_);

  if (dom().isEmpty())
    return std::make_unique<UnordPWMap>(res);

  Set compacted = fact_.createSet();
  for (auto it = pieces_.begin(); it != pieces_.end(); ++it) {
    auto next_it = it;
    ++next_it;
    Set ith_compacted = compacted.intersection(it->dom());
    if (ith_compacted.isEmpty()) {
      
      Map new_ith = fact_.createMap(it->dom().compact(), it->exp());
      for (; next_it != pieces_.end(); ++next_it) {
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

  return std::make_unique<UnordPWMap>(res);
}
*/




////////////////////////////////////////////////////////////////////////////////
// Ordered PWMap Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
member_imp(OrdPWMap, OrdMapCollection, pieces);

OrdPWMap::OrdPWMap(const MapAF &fact) : PWMapDelegate(fact) {}
OrdPWMap::OrdPWMap(const MapAF &fact, const Set &s)
  : PWMapDelegate(fact), pieces_() {
  if (!s.isEmpty()) {
    SetPiece first = s.begin().operator*();
    pieces_.push_back(fact_.createMap(s, Exp(first.arity(), LExp())));
  }
}
OrdPWMap::OrdPWMap(const MapAF &fact, const Map &m)
  : PWMapDelegate(fact), pieces_() {
  if (!m.dom().isEmpty())
    pieces_.push_back(m);
}
OrdPWMap::OrdPWMap(const MapAF &fact, const OrdMapCollection &pieces)
  : PWMapDelegate(fact), pieces_(std::move(pieces)) {}
OrdPWMap::OrdPWMap(const OrdPWMap &pw)
  : PWMapDelegate(pw.fact_), pieces_(pw.pieces_) {}

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

const Map &OrdPWMap::Iterator::operator*() const { return *it_; }

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
    
    if (!m.dom().isEmpty()) {
        if (pieces_.empty() || pieces_.back() < m)  // usa .back() que es más claro y seguro
         pieces_.push_back(m);
        else
            emplaceHint(m, 0);
    }
  
    //if(!isOrdered()){
     // print(std::cout);
     // std::cout << "\n EL MAPA"<<"\n";
     // std::cout << m <<"\n";
     // std::terminate();
    //}
}

unsigned int OrdPWMap::emplaceHint(const Map &m,unsigned int hint)
{
 
    Map resMap = fact_.createMap();
    auto end = pieces_.end();
    auto it = pieces_.begin();
    std::advance(it,hint);
    while (it != end ) {
        resMap = *it;
        if (resMap.minPer() < m.minPer()){
            ++it;
            ++hint;
        }
        else
          break;
  
    }

    pieces_.insert(it, m);
    
    
     //     if(!isOrdered()){
    ///std::cout << "emplaceHint\n";
    //print(std::cout);
    //}
    
    return hint;
   
}


unsigned int OrdPWMap::advanceHint(const MD_NAT crit ,unsigned int hint)
{
 
    Map resMap = fact_.createMap();
    auto end = pieces_.end();
    auto it = pieces_.begin();
    std::advance(it,hint);
    while (it != end ) {
        resMap = *it;
        if (resMap.minPer() < crit){
            ++it;
            ++hint;
        }
        else
          break;
  
    }
    
     // if(!isOrdered()){
    //std::cout << "advanceHint\n";
    //print(std::cout);
    //}
  
    return hint;
   
}

bool OrdPWMap::isOrdered() const{

  for (size_t i = 1; i < pieces_.size(); ++i) {
        if (!(pieces_[i-1] < pieces_[i])) {
        
            std::cout << "\n\n\nMapa no ordenado detectado!\n\n\n";
            return false;
        }
  }

  return true;
}


bool OrdPWMap::operator==(const PWMapDelegate &other) const 
{ 
  //std::cout << "igual\n";
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);

  if (dom() != othr.dom()) 
    return false;

  if (pieces_ == othr.pieces_)
    return true;

   // Seleccionamos el conjunto más corto para iterar externamente.
   const OrdPWMap *shortMap = this;
   const OrdPWMap *longMap = &othr;
   if (othr.pieces_.size() < pieces_.size()) {
       shortMap = &othr;
       longMap  = this;
   }
 
   // Creamos una lista de índices para iterar sobre el conjunto largo.
   std::forward_list<size_t> longIndices;
   auto liIt = longIndices.before_begin();
 
   const size_t longSize = longMap->pieces_.size();
   for (size_t i = 0; i < longSize; ++i)
       liIt = longIndices.insert_after(liIt, i);
 
 
   // Se asume que el contenedor 'pieces_' del conjunto largo tiene acceso aleatorio.
   auto longBegin = longMap->pieces_.begin();
   
   // Iteramos sobre cada elemento del conjunto corto.
   for (const Map &m1 : shortMap->pieces_) {
     const auto m1Min = m1.minPer();
     const auto m1Max = m1.maxPer();
     const auto m1Min0 = m1Min[0];
     const auto m1Max0 = m1Max[0];
 
     // Iteradores para recorrer los índices del conjunto largo.
     auto liPrev = longIndices.before_begin();
     auto liCurr = longIndices.begin();
     while (liCurr != longIndices.end()) {
       size_t idx = *liCurr;
       const Map &m2 = *(longBegin + idx);
       const auto m2Min = m2.minPer();
       const auto m2Max = m2.maxPer();
       const auto m2Min0 = m2Min[0];
       const auto m2Max0 = m2Max[0];
       
       // Si el elemento del conjunto largo es "demasiado chico", eliminamos su índice.
     if (m2Max0 < m1Min0) {
         liCurr = longIndices.erase_after(liPrev);
         continue;
     }
     // Si no se solapan, no hay más intersecciones para este elemento.
     if (m1Max0 < m2Min0)
         break;
 
     // Si hay intersección entre los elementos:
     if (!(m2Max.menorThan(m1Min)) && !(m1Max.menorThan(m2Min))) {
         auto cap_dom = m1.dom().intersection(m2.dom());
         if(!cap_dom.isEmpty()){
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
     ++liPrev;
     ++liCurr;
 }
 if (longIndices.empty())
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
  //std::cout << "suma\n";
  Set noUsed = fact_.createSet();

  PWMapDelegPtr res = std::make_unique<OrdPWMap>(fact_);
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);
  processMapsOrd(othr,noUsed, static_cast<OrdPWMapRef>(*res), &OrdPWMap::processAdd,false);
    //if(!(static_cast<OrdPWMapRef>(*res)).isOrdered())
    //std::cout << "suma\n";
  return res;
}

void OrdPWMap::processAdd(const Map &m1, const Map &m2, Set &set, PWMapDelegate &ordpwmap,
  unsigned int* posGlobal) const
{   
    OrdPWMapRef res = static_cast<OrdPWMapRef>(ordpwmap);
    auto resAdd = m1 + m2;
    if (!resAdd.dom().isEmpty()){
       *posGlobal=res.advanceHint(m1.minPer(),*posGlobal);
       res.emplaceHint(resAdd,*posGlobal);
    
    }                            
                                                          
}


PWMapDelegPtr OrdPWMap::operator-(const PWMapDelegate &other) const
{

  //std::cout << "menos\n";
  PWMapDelegPtr res = std::make_unique<OrdPWMap>(fact_);

  if (isEmpty() || other.isEmpty())
    return res;



  Interval all(0, 1, Inf);
  Set univ = fact_.createSet(SetPiece(arity(), all));
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);
  processMapsOrd(othr,univ, static_cast<OrdPWMapRef>(*res) , &OrdPWMap::processMinus,true);
  
  
  //if(!static_cast<OrdPWMapRef>(*res).isOrdered())
   // std::cout << "Resta\n";
  return res;
}

void OrdPWMap::processMinus(const Map &m1, const Map &m2, Set &set, PWMapDelegate &ordpwmap,
  unsigned int* posGlobal) const
{ 

  // Conversión a referencia interna
  OrdPWMapRef ordpwmap_c = static_cast<OrdPWMapRef>(ordpwmap);
  
  // Calcular dominio común
  Set dom = m1.dom().intersection(m2.dom());
  if (dom.isEmpty())
    return;
  
  // Calcular la diferencia de expresiones
  Exp minus_exp = m1.exp() - m2.exp();
  OrdPWMap ith(fact_, set);
  
  for (unsigned int j = 0, n = arity(); j < n; ++j) {
      RATIONAL m = minus_exp[j].slope();
      RATIONAL h = minus_exp[j].offset();
      
      // Inicialización de intervalos: negativos y positivos
      NAT begin_neg = 0, end_neg = Inf;
      NAT begin_pos = 0, end_pos = Inf;
      
      if (m == 0) {  // Expresión constante
        if (h < 0) {
          begin_pos = 1;
          end_pos = 0;
        } else {
          begin_neg = 1;
          end_neg = 0;
        }
      }
      else if (m > 0) {  // Expresión creciente
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
      else {  // Expresión decreciente
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
    //std::sort(ith.pieces_.begin(),ith.pieces_.end());
    PWMapDelegPtr concatenatedPtr = ordpwmap_c.concatenation(*ith.restrict(dom)); 
    OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(*concatenatedPtr);
    ordpwmap_c.pieces_= std::move(othr.pieces_);


}

std::ostream &OrdPWMap::print(std::ostream &out) const
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

std::size_t OrdPWMap::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->dom().arity();
}

bool OrdPWMap::isEmpty() const { 
//std::cout << "isEmpty\n";

return pieces_.empty(); }

Set OrdPWMap::dom() const
{
  //std::cout << "DOM\n";
  Set res = fact_.createSet();
  for (const Map &m : pieces_) {
    Set d = m.dom();
    res = res.disjointCup(d);
  }

  return res;
}


PWMapDelegPtr OrdPWMap::restrict(const Set &subdom) const
{
  //std::cout << "RESTRICT\n";
  //isOrdered();
  OrdPWMap res(fact_);
  if (subdom.isEmpty())
    return std::make_unique<OrdPWMap>(res);
  unsigned int posGlobal = 0;
  
  Map subdomMap = fact_.createMap(subdom,Exp(subdom.arity(),LExp(0,0)));
  const auto minSubdom= subdomMap.minPer();
  const auto maxSubdom= subdomMap.maxPer();
 
  auto it = pieces_.begin();
  while (it != pieces_.end()) {
    const Map &m = *it;
    const auto domMin = m.minPer();
    const auto domMax = m.maxPer();

    if (domMax[0] < minSubdom[0]) {
      ++it;
      continue;
    }

    if (maxSubdom[0] < domMin[0]) {
      break;
    }

    if (!(domMax.menorThan(minSubdom)) && !(maxSubdom.menorThan(domMin))) {
      Map resRest = m.restrict(subdom);
      if (!resRest.dom().isEmpty()) {
       posGlobal=res.advanceHint(domMin,posGlobal);
       res.emplaceHint(resRest,posGlobal);
      }
    }

    ++it;
  }
  
  //if(!res.isOrdered()){
   // std::cout << "restrict\n";
   // res.print(std::cout);
   // }
  //std::cout << "End RESTRICT\n";
  return std::make_unique<OrdPWMap>(res);
}




Set OrdPWMap::image() const
{
  Set res = fact_.createSet();

  for (const Map &m : pieces_) {
    res = res.cup(m.image());
  }

  return res;
}

Set OrdPWMap::image(const Set &subdom) const
{
  return restrict(subdom)->image();
}

Set OrdPWMap::preImage(const Set &subcodom) const
{
  Set res = fact_.createSet();

  for (const Map &m : pieces_)
    res = res.disjointCup(m.preImage(subcodom));

  return res;
}

PWMapDelegPtr OrdPWMap::inverse() const
{ 
  //std::cout << "inverse\n";

  OrdPWMap res(fact_);

  for (const Map &m : pieces_){
    const Map &inv = m.minInv();
    if(!inv.dom().isEmpty())
      res.pieces_.emplace_back(m.minInv());
  }
    
  std::sort(res.pieces_.begin(),res.pieces_.end());
  
  
  //if(!res.isOrdered())
   // std::cout << "inverse\n";
    
  return std::make_unique<OrdPWMap>(res);
}

PWMapDelegPtr OrdPWMap::composition(const PWMapDelegate &other) const {
  OrdPWMap res(fact_); 
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);

  unsigned int posGlobal = 0;

  for (const Map &mO : othr.pieces_) {
    const auto &img = mO.image();
    
    OrdAF fact;
    Map imgMap(fact,img,Exp(img.arity(),LExp(0,0)));
    const auto mOMin = imgMap.minPer();
    const auto mOMax = imgMap.maxPer();
    
    posGlobal = res.advanceHint(mO.minPer(), posGlobal);

    for (const Map &mT : pieces_) {
      const auto mTMin = mT.minPer();
      const auto mTMax = mT.maxPer();

      if (mTMax[0] < mOMin[0]) continue;
      if (mOMax[0] < mTMin[0]) break;

      if (!(mTMax.menorThan(mOMin)) && !(mOMax.menorThan(mTMin))) {
        auto resCom = mT.composition(mO);
        if (!resCom.dom().isEmpty()) {
          res.emplaceHint(resCom, posGlobal);
        }
      }
    }
  }

  return std::make_unique<OrdPWMap>(std::move(res));
}

PWMapDelegPtr OrdPWMap::mapInf(unsigned int n) const
{
  //std::cout << "mapinf\n";
  
  PWMapDelegPtr res = std::make_unique<OrdPWMap>(*this);
  PWMapDelegPtr old_res = std::make_unique<OrdPWMap>(fact_);
   std::cout << "valor de n\n" << n;
  if (!dom().isEmpty()) {
    for (unsigned int j = 0; old_res != res && j < n; ++j) {
      OrdPWMap *rs = static_cast<OrdPWMap *>(res.get());
      old_res = std::make_unique<OrdPWMap>(*rs);
      PWMapDelegPtr new_res = res->composition(*this);
      res = std::move(new_res);
    }

    if (*old_res == *res)
      return res;
    
    //std::cout << "pasamos de largo\n";
    //static_cast<OrdPWMapRef>(*res).print(std::cout);
    //static_cast<OrdPWMapRef>(*old_res).print(std::cout);
    
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
  
  //if(!static_cast<OrdPWMapRef>(*res).isOrdered()){
    //std::cout << "mapInf\n";
    //static_cast<OrdPWMapRef>(*res).print(std::cout);}
  return res;
}

PWMapDelegPtr OrdPWMap::mapInf() const { return mapInf(0); }

// Extra operations ------------------------------------------------------------


PWMapDelegPtr OrdPWMap::concatenation(const PWMapDelegate &other) const
{

  //std::cout << "concatenation\n";
  OrdPWMap res(fact_);
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);
  res.pieces_.reserve(pieces_.size() + othr.pieces_.size());
  
  if (isEmpty())
    return std::make_unique<OrdPWMap>(othr);

  if (other.isEmpty())
    return std::make_unique<OrdPWMap>(*this);
  
  
  if (pieces_.back().dom().minElem() < othr.pieces_.front().dom().minElem()) {
  res.pieces_.insert(res.pieces_.end(), pieces_.begin(), pieces_.end());
  res.pieces_.insert(res.pieces_.end(), othr.pieces_.begin(), othr.pieces_.end());
  return std::make_unique<OrdPWMap>(std::move(res));
  }
  
  if (othr.pieces_.back().dom().minElem() < pieces_.front().dom().minElem()) {
  res.pieces_.insert(res.pieces_.end(), othr.pieces_.begin(), othr.pieces_.end());
  res.pieces_.insert(res.pieces_.end(), pieces_.begin(), pieces_.end());
  return std::make_unique<OrdPWMap>(std::move(res));
  }
  
  auto it1 = pieces_.begin(), it2 = othr.pieces_.begin();
  auto end1 = pieces_.end(), end2 = othr.pieces_.end();
  
  for (; it1 != end1 && it2 != end2;) {
    const Map &map1 = *it1;
    const Map &map2 = *it2;
    auto min1 = map1.dom().minElem();
    auto min2 = map2.dom().minElem();

    if (min1 < min2) {
      res.emplaceBack(map1);
      ++it1;
    } else {
      res.emplaceBack(map2); 
      ++it2;
    }
  }

  for (; it1 != end1; ++it1) {
    const Map &map1 = *it1;
    res.emplaceBack(map1);
  }

  for (; it2 != end2; ++it2) {
    const Map &map2 = *it2;
    res.emplaceBack(map2);
  }
  
  //if(!res.isOrdered())
    //std::cout << "concatenation\n";

  return std::make_unique<OrdPWMap>(std::move(res));
}


PWMapDelegPtr OrdPWMap::combine(const PWMapDelegate &other) const
{
   //std::cout << "vombine\n";
  
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);

  if (isEmpty())
    return std::make_unique<OrdPWMap>(othr);

  if (other.isEmpty())
    return std::make_unique<OrdPWMap>(*this);

  PWMapDelegPtr res_ptr = std::make_unique<OrdPWMap>(*this);
  OrdPWMap &res = static_cast<OrdPWMap&>(*res_ptr);
  
  Set dom1 = dom();

  unsigned int posGlobal=0;
  
  OrdAF fact;
  Map dom1Map(fact,dom1,Exp(dom1.arity(),LExp(0,0)));
  const auto dom1Min = dom1Map.minPer();
  const auto dom1Max = dom1Map.maxPer();
  
  for (const Map &m2 : othr.pieces_) {
    const auto dom2Max = m2.maxPer();
    const auto dom2Min = m2.minPer();
    Map resComb = fact_.createMap(m2.dom(), m2.exp());
    
     if (!(dom2Max.menorThan(dom1Min)) && !(dom1Max.menorThan(dom2Min)) ) {

          Set dom2 = m2.dom(), new_dom = dom2.difference(dom1);
          resComb = fact_.createMap(new_dom, m2.exp());
          
          
          if (resComb.dom().isEmpty())
            continue;
      }
      posGlobal = res.advanceHint(dom2Min,posGlobal);
      res.emplaceHint(std::move(resComb),posGlobal);
      
    }
  

  //if(!res.isOrdered()){
   // std::cout << "convine\n";
   // res.print(std::cout);
   // }
  
  return  std::make_unique<OrdPWMap>(res);
}

PWMapDelegPtr OrdPWMap::reduce(const Interval &i, const LExp &le) const
{
  //std::cout << "entramos reduce de interval\n";
  OrdPWMap res(fact_);

  if (!i.isEmpty()) {
    RATIONAL zero(0, 1);
    if (le.slope() == 1 && le.offset() != 0) {
      INT h = le.offset().toInt();

      NAT st = i.step();
      if (h == (INT) st) {
        NAT hi = i.end();
        RATIONAL const_expr(hi + st, 1);
        if (st < Inf - hi)
          res = OrdPWMap(fact_, fact_.createMap(i, LExp(zero, const_expr)));
      }

      else if (h == (INT) -st) {
        NAT lo = i.begin();
        RATIONAL const_expr(lo - st, 1);
        if (lo >= st)
          res = OrdPWMap(fact_, fact_.createMap(i, LExp(zero, const_expr)));
      }

      else if (h % (INT) st == 0) {
        // Is convenient the partition of the piece?
        if ((INT) i.cardinal() > h*h) {
          res = OrdPWMap(fact_);
          INT absh = std::abs(h);

          for (int k = 1; k <= absh; ++k) {
            NAT new_begin = i.begin() + k - 1;
            Interval kth_piece(new_begin, (NAT) absh, i.end());

            RATIONAL kth_off;
            if (h > 0)
              kth_off = kth_piece.end() + h;
            else
              kth_off = kth_piece.begin() + h;

            res.pieces_.emplace_back(fact_.createMap(kth_piece, LExp(0, kth_off)));
          }
        }
      }
    }

    else
      res.pieces_.emplace_back(fact_.createMap(i, le));
  }
  //if(!res.isOrdered()){
   //std::cout << "reduce de interval\n";
   //res.print(std::cout);
  //}


  return std::make_unique<OrdPWMap>(res);
}


PWMapDelegPtr OrdPWMap::reduce(const Map &map) const
{
  OrdPWMap res(fact_);
  //std::cout << "entramos reduce de map\n";
  Set not_reduced = fact_.createSet();
  Exp e = map.exp();
  for (const SetPiece &dom_piece : map.dom()) {
    SetPiece aux_piece = dom_piece;
    Exp aux_exp = e;
    bool was_reduced = false;
    for (unsigned int j = 0; j < dom_piece.arity(); ++j) {
      PWMapDelegPtr aux = reduce(dom_piece[j], e[j]); 
      OrdPWMap *jth_red = static_cast<OrdPWMap *>(aux.get());
      for (const Map &ith_reduced : jth_red->pieces_) {
        aux_piece[j] = ith_reduced.dom().begin().operator*().operator[](0);
        aux_exp[j] = ith_reduced.exp()[0];
        if (aux_piece != dom_piece || aux_exp != e) {
          res.pieces_.emplace_back(fact_.createMap(aux_piece, aux_exp));
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
  
  //TODO: esto seguro no esta ordenado
  if(!not_reduced.isEmpty())
    res.pieces_.emplace_back(fact_.createMap(not_reduced, e)); // Add unreduced subpieces
  
  //if(!res.isOrdered()){
   //std::cout << "reduce de map\n";
   //res.print(std::cout);
   //}
  std::sort(res.pieces_.begin(),res.pieces_.end());
  return std::make_unique<OrdPWMap>(res);
}

//TODO-med: trata de dilucidar para que sirve reduce
PWMapDelegPtr OrdPWMap::reduce() const
{ 
     //std::cout << "reduce\n";
  
    //PWMapDelegPtr res = std::make_unique<OrdPWMap>(fact_);
  
    OrdPWMap res(fact_);
    for (const Map &m : pieces_) {
        PWMapDelegPtr ith = reduce(m);
        OrdPWMap *ith_c = static_cast<OrdPWMap *>(ith.get());
        for (const Map &ith_elem : ith_c->pieces_) {
             res.pieces_.emplace_back(ith_elem);
        }
    } 

    std::sort(res.pieces_.begin(), res.pieces_.end());
    //if(!res.isOrdered()){
    //std::cout << "reduce\n";
    ////res.print(std::cout);
    //}
    return std::make_unique<OrdPWMap>(res);
}

PWMapDelegPtr OrdPWMap::minMap(const PWMapDelegate &other) const
{
   //std::cout << "minmap\n";

  if (isEmpty() || other.isEmpty())
    return std::make_unique<OrdPWMap>(fact_);

  PWMapDelegPtr aux1 = restrict(other.dom()), aux2 = other.restrict(dom());
  Set zero = fact_.createSet(SetPiece(arity(), Interval(0, 1, 0)));
  Set to_zero = (*aux1 - *aux2)->preImage(zero);
  Set not_zero = aux2->dom().difference(to_zero);
  aux1 = aux1->restrict(to_zero);
  aux2 = aux2->restrict(not_zero);
  
  PWMapDelegPtr res = aux1->combine(*aux2);
 //if(!static_cast<OrdPWMapRef>(*res).isOrdered()){
   // std::cout << "minMap\n";
   // static_cast<OrdPWMapRef>(*res).print(std::cout);
   //}
  return res;
}



PWMapDelegPtr OrdPWMap::minAdjMap(const PWMapDelegate &other) const
{
  //std::cout << "MinADj\n";
  PWMapDelegPtr res = std::make_unique<OrdPWMap>(fact_);
  Set visited = fact_.createSet();
  processMapsOrd(other,visited, static_cast<OrdPWMapRef>(*res) , &OrdPWMap::processMinAdjMap,true);
 //if(!static_cast<OrdPWMapRef>(*res).isOrdered()){
  //  std::cout << "minAdj\n";
   // static_cast<OrdPWMapRef>(*res).print(std::cout);
   //}
   
  OrdPWMapRef res_c = static_cast<OrdPWMapRef>(*res);


  std::sort(res_c.pieces_.begin(), res_c.pieces_.end()); 
   
  return res;
}


//TODO-Low:terminar de enteder que pasa
void OrdPWMap::processMinAdjMap(const Map &m1, const Map &m2, Set &set, PWMapDelegate &ordpwmap,
     unsigned int*posGlobal) const 
{ 

  OrdPWMapRef ordpwmap_c = static_cast<OrdPWMapRef>(ordpwmap);
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
      Set again = dom_res.intersection(set);
      if (!again.isEmpty()) {
        PWMapDelegPtr aux_res = ordpwmap_c.restrict(dom_res);
        PWMapDelegPtr min_map = aux_res->minMap(ith_pw);
        PWMapDelegPtr new_resPtr = min_map->combine(ith_pw)->combine(ordpwmap_c);
        OrdPWMapCRef new_res_c = static_cast<OrdPWMapCRef>(*new_resPtr);
        ordpwmap_c.pieces_= std::move(new_res_c.pieces_);
        set = set.cup(ith_pw.dom());
      }
      else {
       
        ordpwmap_c.pieces_.emplace_back(ith);
        set = set.disjointCup(dom_res);
      }
    }
  }
}

PWMapDelegPtr OrdPWMap::firstInv(const Set &subdom) const
{
  //std::cout << "firstInv\n";
  OrdPWMap res(fact_);
  if (isEmpty() || subdom.isEmpty() )
    return std::make_unique<OrdPWMap>(res);

   
  OrdAF fact;
  Map subdomMap(fact,subdom,Exp(subdom.arity(),LExp(0,0)));
  const auto mSMin = subdomMap.minPer();
  const auto mSMax = subdomMap.maxPer();
  Set visited = fact_.createSet();
  
  for (const Map &m : pieces_) {
    const auto mTMin = m.minPer();
    const auto mTMax = m.maxPer();
    
    if (mTMax[0] < mSMin[0]) continue;
    if (mSMax[0] < mTMin[0]) break;

    if (!(mTMax.menorThan(mSMin)) && !(mSMax.menorThan(mTMin))) {
      Set mImg = m.image(subdom);
      
      if (!mImg.isEmpty()) {
        Map mImgMap(fact,mImg,Exp(mImg.arity(),LExp(0,0)));
        const auto mIMin = mImgMap.minPer();
        const auto mIMax = mImgMap.maxPer();

        if (!visited.isEmpty()) {
          Map visitedMap(fact,visited,Exp(visited.arity(),LExp(0,0)));
          const auto vMin = visitedMap.minPer();
          const auto vMax = visitedMap.maxPer();

          if (!(mIMax.menorThan(vMin)) && !(vMax.menorThan(mIMin))) {
            Set res_dom = mImg.difference(visited);
            if (!res_dom.isEmpty())
              mImg = res_dom;
            else
              continue;
          }
        }

        Map new_map = fact_.createMap(m.preImage(mImg), m.exp());
        res.pieces_.emplace_back(new_map.minInv());
        visited = visited.cup(mImg);
      }
    }
  }

  std::sort(res.pieces_.begin(), res.pieces_.end());

  // if (!res.isOrdered()) {
  //   std::cout << "firstInv\n";
  //   res.print(std::cout);
  // }

  return std::make_unique<OrdPWMap>(res);
}


PWMapDelegPtr OrdPWMap::firstInv() const { return firstInv(dom()); }


PWMapDelegPtr OrdPWMap::filterMap(bool (*f)(const Map &)) const
{ 
  //std::cout << "FILTER\n";
  
  PWMapDelegPtr res = std::make_unique<OrdPWMap>(fact_);

  for (const Map &m : pieces_)
    if (f(m))
      res->emplaceBack(m);
  
  //if(!static_cast<OrdPWMapRef>(*res).isOrdered()){
    //std::cout << "filterMap\n";
    ///static_cast<OrdPWMapRef>(*res).print(std::cout);
    //}
  return res;
}

Set OrdPWMap::equalImage(const PWMapDelegate &other) const

{ //std::cout << "equalImage\n";   

  Set res = fact_.createSet();
  OrdPWMap noUsed(fact_);
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);
  processMapsOrd(othr, res, noUsed, &OrdPWMap::processEqualImage,false);
  return res;

}

void OrdPWMap::processEqualImage(const Map &m1, const Map &m2, Set &set, PWMapDelegate &ordpwmap,
     unsigned int*posGlobal) const 
{ 
  Set cap_dom = m1.dom().intersection(m2.dom());
  if (!cap_dom.isEmpty()) {
    Map m1_cap = fact_.createMap(cap_dom, m1.exp());
    Map m2_cap = fact_.createMap(cap_dom, m2.exp());
    if (m1_cap == m2_cap)
      set = set.disjointCup(cap_dom);
  }
}

void OrdPWMap::processMapsOrd(
  const PWMapDelegate &other,
  Set &set,
  PWMapDelegate  &ordmap,
  ProcessFunc process,
  bool orderMts
  ) const
{
  OrdPWMapCRef othr = static_cast<OrdPWMapCRef>(other);

  const OrdPWMap *shortMap = this;
  const OrdPWMap *longMap  = &othr;
  if (!orderMts && othr.pieces_.size() < pieces_.size()) {
    shortMap = &othr;
    longMap  = this;
  }

  
  std::forward_list<size_t> shortIndices;
  auto siIt = shortIndices.before_begin();

  const size_t shortSize = shortMap->pieces_.size();
  for (size_t i = 0; i < shortSize; ++i)
    siIt = shortIndices.insert_after(siIt, i);

  auto shortBegin = shortMap->pieces_.begin();
  unsigned int posGlobal = 0;
  
  // Iterate first over the larger map (longMap)
  for (const Map &m2 : longMap->pieces_){ 
    const auto m2Min = m2.minPer();
    const auto m2Max = m2.maxPer();
    const auto m2Min0 = m2Min[0];
    const auto m2Max0 = m2Max[0];
    
    auto siPrev = shortIndices.before_begin();
    auto siCurr = shortIndices.begin();

    while (siCurr != shortIndices.end()) {
      size_t idx = *siCurr;
      const Map &m1 = *(shortBegin + idx);
      const auto m1Min = m1.minPer();
      const auto m1Max = m1.maxPer();
      const auto m1Min0 = m1Min[0];
      const auto m1Max0 = m1Max[0];
      
      // Skip ranges completely before
      if (m1Max0 < m2Min0) {
        siCurr = shortIndices.erase_after(siPrev);
        continue;
      }
      // Break once ranges no longer overlap
      if (m2Max0 < m1Min0)
        break;

      // Process overlapping intervals
      if (!(m1Max.menorThan(m2Min)) && !(m2Max.menorThan(m1Min))) {
        (this->*process)(m1, m2, set, ordmap, &posGlobal);
      }

      ++siPrev;
      ++siCurr;
    }
    if (shortIndices.empty())
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

    //std::cout << "OFSET\n";
  PWMapDelegPtr res = std::make_unique<OrdPWMap>(fact_);

  for (const Map &m : pieces_)
    res->emplaceBack(fact_.createMap(m.dom().offset(off), m.exp()));
  
  //if(!static_cast<OrdPWMapRef>(*res).isOrdered()){
   // std::cout << "ofsetdom off\n";
    //static_cast<OrdPWMapRef>(*res).print(std::cout);
    //}
  return res;
}

PWMapDelegPtr OrdPWMap::offsetDom(const PWMapDelegate &off) const
{
  //std::cout << "offsetDom\n";  
     if(!isOrdered()){
   std::cout << "entrada ofsetdom pw\n";
   print(std::cout);
  }
  OrdPWMap res(fact_);
  OrdAF fact;
  Map offMap(fact,off.dom(),Exp(off.dom().arity(),LExp(0,0)));
  const auto sdomMax = offMap.maxPer();
  const auto sdomMin = offMap.minPer();
  for (const Map &m : pieces_) {
    const auto domMax = m.maxPer();
    const auto domMin = m.minPer();
    
    if (domMax[0] < sdomMin[0])
      continue;
    
    if (sdomMax[0] < domMin[0])
        break;
    
    if (!(domMax.menorThan(sdomMin)) && !(sdomMax.menorThan(domMin)) ) {
      Set ith_dom = off.image(m.dom());
      Map resMap = fact_.createMap(ith_dom, m.exp());
      
      if (!resMap.dom().isEmpty())
          res.pieces_.emplace_back(resMap);
      
    }
  }

  
  std::sort(res.pieces_.begin(),res.pieces_.end());
   if(!res.isOrdered()){
   std::cout << "ofsetdom pw\n";
   res.print(std::cout);
  }
  return std::make_unique<OrdPWMap>(res);

}

PWMapDelegPtr OrdPWMap::offsetImage(const MD_NAT &off) const
{
  //std::cout << "offsetImage\n";

  PWMapDelegPtr res = std::make_unique<OrdPWMap>(fact_);

  for (const Map &m : pieces_) {
    Exp e = m.exp(), res_e;
    for (unsigned int j = 0; j < e.arity(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    res->emplaceBack(fact_.createMap(m.dom(), res_e));
  }
    //if(!static_cast<OrdPWMapRef>(*res).isOrdered()){
   // std::cout << "ofsetImage off\n";
   // static_cast<OrdPWMapRef>(*res).print(std::cout);
   // }
  return res;
}

PWMapDelegPtr OrdPWMap::offsetImage(const Exp &off) const
{
    //std::cout << "offsetImage2\n";
  PWMapDelegPtr res = std::make_unique<OrdPWMap>(fact_);

  for (const Map &m : pieces_) 
    res->emplaceBack(fact_.createMap(m.dom(), off + m.exp()));
  
    //  if(!static_cast<OrdPWMapRef>(*res).isOrdered()){
    //std::cout << "ofsetImage exp\n";
    //static_cast<OrdPWMapRef>(*res).print(std::cout);
    //}

  return res;
}

PWMapDelegPtr OrdPWMap::compact() const
{
  OrdPWMap res(fact_);
  //std::cout << "compact\n";  
  if (dom().isEmpty())
    return std::make_unique<OrdPWMap>(res);
  
  std::forward_list<size_t> indices;
  auto liIt = indices.before_begin();

  const size_t lSize = pieces_.size();
  for (size_t i = 0; i < lSize; ++i)
    liIt = indices.insert_after(liIt, i);

  
  auto begin = pieces_.begin();
  //unsigned int posInit=0;
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
      
      res.pieces_.emplace_back(new_ith);
      liPrev = indices.before_begin();
      liCurr = indices.begin();
  }
  //if(!res.isOrdered()){
    //std::cout << "compact\n";
    //res.print(std::cout);
    //}
  return std::make_unique<OrdPWMap>(res);
}

/*PWMapDelegPtr OrdPWMap::compact() const
{
  OrdPWMap res(fact_);

  if (dom().isEmpty())
    return std::make_unique<OrdPWMap>(res);
  
  std::forward_list<size_t> indices;
  auto liIt = indices.before_begin();

  const size_t lSize = pieces_.size();
  for (size_t i = 0; i < lSize; ++i)
    liIt = indices.insert_after(liIt, i);

  
  auto begin = pieces_.begin();
  //unsigned int posInit=0;
  Set compacted = fact_.createSet();
  for (auto it = pieces_.begin(); it != pieces_.end(); ++it) {

    //std::cout << "Conjunto=\n";
    //std::cout << compacted;
    //std::cout << "\n";
    Set ith_compacted = compacted.intersection(it->dom());
    if (ith_compacted.isEmpty()) {
      Map new_ith = fact_.createMap(it->dom().compact(), it->exp());

      auto liPrev = indices.before_begin();
      auto liCurr = indices.begin();

    
      while (liCurr != indices.end()) {
        size_t idx = *liCurr;
        const Map &nextMap = *(begin + idx);
        if (nextMap == *it){
            liCurr = indices.erase_after(liPrev);
            continue;
        }
        
        //Set next_compacted = compacted.intersection(nextMap.dom());
       // if (next_compacted.isEmpty()) {
          auto ith = new_ith.compact(nextMap);
          if (ith) {
            new_ith = ith.value();
            compacted = compacted.cup(nextMap.dom());
            liCurr = indices.erase_after(liPrev);
            continue;
          }
        //}

        ++liPrev;
        ++liCurr;
      }
      
      res.emplaceBack(new_ith);
    }
  }

  return std::make_unique<OrdPWMap>(res);
}
 */
 
 
 
////////////////////////////////////////////////////////////////////////////////
// Hybrid PWMap Implementation ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
member_move_imp(HybridPWMap, MapCollection, pieces);

HybridPWMap::HybridPWMap(const MapAF &fact) : PWMapDelegate(fact) {}
HybridPWMap::HybridPWMap(const MapAF &fact, const Set &s)
  : PWMapDelegate(fact), pieces_() {
  if (!s.isEmpty()) {
    SetPiece first = s.begin().operator*();
    pieces_.emplace_hint(pieces_.cend(), fact_.createMap(s, Exp(first.arity(), LExp())));
    //pieces_.push_back(fact_.createMap(s, Exp(first.arity(), LExp())));
  }
}
HybridPWMap::HybridPWMap(const MapAF &fact, const Map &m)
  : PWMapDelegate(fact), pieces_() {
  if (!m.dom().isEmpty()){
    pieces_.emplace_hint(pieces_.cend(), m);
    //pieces_.push_back(m);
    }
}
HybridPWMap::HybridPWMap(const MapAF &fact, const MapCollection &pieces)
  : PWMapDelegate(fact), pieces_(std::move(pieces)) {}
HybridPWMap::HybridPWMap(const HybridPWMap &pw)
  : PWMapDelegate(pw.fact_), pieces_(pw.pieces_) {}

PWMapDelegPtr HybridPWMap::clone() const
{
  return std::make_unique<HybridPWMap>(*this);
}

member_imp(HybridPWMap::Iterator, MapCollection::const_iterator, it);

HybridPWMap::Iterator::Iterator(MapCollection::const_iterator it)
  : it_(it) {}

void HybridPWMap::Iterator::operator++()
{
  ++it_;
  return;
}

bool HybridPWMap::Iterator::operator!=(const PWMapDelegate::Iterator &other)
  const
{
  return it_ != static_cast<const HybridPWMap::Iterator *>(&other)->it_;
}

const Map &HybridPWMap::Iterator::operator*() const { return *it_; }

std::shared_ptr<PWMapDelegate::Iterator> HybridPWMap::begin() const
{
  return std::make_shared<HybridPWMap::Iterator>(pieces_.begin());
}

std::shared_ptr<PWMapDelegate::Iterator> HybridPWMap::end() const
{
  return std::make_shared<HybridPWMap::Iterator>(pieces_.end());
}

void HybridPWMap::emplaceBack(const Map &m)
{ 
    
    if (!m.dom().isEmpty()) {
         //pieces_.push_back(m);
         pieces_.emplace_hint(pieces_.cend(), m);
    }
  
}

//Innecesaria
unsigned int HybridPWMap::emplaceHint(const Map &m,unsigned int hint)
{
 
    Map resMap = fact_.createMap();
    auto end = pieces_.end();
    auto it = pieces_.begin();
    std::advance(it,hint);
    while (it != end ) {
        resMap = *it;
        if (resMap.dom().minElem() < m.dom().minElem()){
            ++it;
            ++hint;
        }
        else
          break;
  
    }

    pieces_.insert(it, m);
    
    
     //     if(!isOrdered()){
    ///std::cout << "emplaceHint\n";
    //print(std::cout);
    //}
    
    return hint;
   
}

//Innecesaria
unsigned int HybridPWMap::advanceHint(const MD_NAT crit ,unsigned int hint)
{
 
    Map resMap = fact_.createMap();
    auto end = pieces_.end();
    auto it = pieces_.begin();
    std::advance(it,hint);
    while (it != end ) {
        resMap = *it;
        if (resMap.dom().minElem() < crit){
            ++it;
            ++hint;
        }
        else
          break;
  
    }
    
     // if(!isOrdered()){
    //std::cout << "advanceHint\n";
    //print(std::cout);
    //}
  
    return hint;
   
}

/*
bool HybridPWMap::isOrdered() const{

  for (size_t i = 1; i < pieces_.size(); ++i) {
        if (!(pieces_[i-1] < pieces_[i])) {
        
            std::cout << "\n\n\nMapa no ordenado detectado!\n\n\n";
            return false;
        }
  }

  return true;
}*/


bool HybridPWMap::operator==(const PWMapDelegate &other) const 
{ 
  //std::cout << "==\n"; 
  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);

  if (dom() != othr.dom()) 
    return false;

  if (pieces_ == othr.pieces_)
    return true;
  /*
  HybridPWMap t(fact_), o(fact_);

  t.pieces_.reserve(pieces_.size());
  for (const Map& m : pieces_)
      t.emplaceBack(m);  

  o.pieces_.reserve(othr.pieces_.size());
  for (const Map& m : othr.pieces_)
      o.emplaceBack(m);  

  std::sort(t.pieces_.begin(), t.pieces_.end());
  std::sort(o.pieces_.begin(), o.pieces_.end());
  */
   // Seleccionamos el conjunto más corto para iterar externamente.
   const HybridPWMap *shortMap = this;
   const HybridPWMap *longMap = &othr;
   if (othr.pieces_.size() < pieces_.size()) {
       shortMap = &othr;
       longMap  = this;
   }
 
   // Creamos una lista de índices para iterar sobre el conjunto largo.
   std::forward_list<size_t> longIndices;
   auto liIt = longIndices.before_begin();
 
   const size_t longSize = longMap->pieces_.size();
   for (size_t i = 0; i < longSize; ++i)
       liIt = longIndices.insert_after(liIt, i);
 
 
   // Se asume que el contenedor 'pieces_' del conjunto largo tiene acceso aleatorio.
   auto longBegin = longMap->pieces_.begin();
   
   // Iteramos sobre cada elemento del conjunto corto.
   for (const Map &m1 : shortMap->pieces_) {
     const auto m1Min = m1.dom().minElem();
     const auto m1Max = m1.dom().maxElem();
     const auto m1Min0 = m1Min[0];
     const auto m1Max0 = m1Max[0];
 
     // Iteradores para recorrer los índices del conjunto largo.
     auto liPrev = longIndices.before_begin();
     auto liCurr = longIndices.begin();
     while (liCurr != longIndices.end()) {
       size_t idx = *liCurr;
       const Map &m2 = *(longBegin + idx);
       const auto m2Min = m2.dom().minElem();
       const auto m2Max = m2.dom().maxElem();
       const auto m2Min0 = m2Min[0];
       const auto m2Max0 = m2Max[0];
       
       // Si el elemento del conjunto largo es "demasiado chico", eliminamos su índice.
     if (m2Max0 < m1Min0) {
         liCurr = longIndices.erase_after(liPrev);
         continue;
     }
     // Si no se solapan, no hay más intersecciones para este elemento.
     if (m1Max0 < m2Min0)
         break;
 
     // Si hay intersección entre los elementos:
     if (!(m2Max.menorThan(m1Min)) && !(m1Max.menorThan(m2Min))) {
         auto cap_dom = m1.dom().intersection(m2.dom());
         if(!cap_dom.isEmpty()){
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
     ++liPrev;
     ++liCurr;
 }
 if (longIndices.empty())
     break; 
 }

  return true;
}


bool HybridPWMap::operator!=(const PWMapDelegate &other) const
{ 
  return !(*this == other);
}

HybridPWMap &HybridPWMap::operator=(HybridPWMap &&other)
{
  if (this != &other)
    pieces_ = std::move(other.pieces_);

  return *this;
}



PWMapDelegPtr HybridPWMap::operator+(const PWMapDelegate &other) const
{
  //std::cout << "suma\n";
  Set noUsed = fact_.createSet();

  PWMapDelegPtr res = std::make_unique<HybridPWMap>(fact_);
  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);
  
  processMapsOrd(othr,noUsed, static_cast<HybridPWMapRef>(*res), &HybridPWMap::processAdd);
  return res;
}

void HybridPWMap::processAdd(const Map &m1, const Map &m2, Set &set, PWMapDelegate &ordpwmap,
  unsigned int* posGlobal) const
{   
    HybridPWMapRef res = static_cast<HybridPWMapRef>(ordpwmap);
    auto resAdd = m1 + m2;
    if (!resAdd.dom().isEmpty()){
     
      res.emplaceBack(resAdd);
    
    }                            
                                                          
}


PWMapDelegPtr HybridPWMap::operator-(const PWMapDelegate &other) const
{

  //std::cout << "menos\n";
  PWMapDelegPtr res = std::make_unique<HybridPWMap>(fact_);

  if (isEmpty() || other.isEmpty())
    return res;



  Interval all(0, 1, Inf);
  Set univ = fact_.createSet(SetPiece(arity(), all));
  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);

  processMapsOrd(othr,univ, static_cast<HybridPWMapRef>(*res) , &HybridPWMap::processMinus);
  
  return res;
}

void HybridPWMap::processMinus(const Map &m1, const Map &m2, Set &set, PWMapDelegate &ordpwmap,
  unsigned int* posGlobal) const
{ 

  // Conversión a referencia interna
  HybridPWMapRef ordpwmap_c = static_cast<HybridPWMapRef>(ordpwmap);
  
  // Calcular dominio común
  Set dom = m1.dom().intersection(m2.dom());
  if (dom.isEmpty())
    return;
  
  // Calcular la diferencia de expresiones
  Exp minus_exp = m1.exp() - m2.exp();
  HybridPWMap ith(fact_, set);
  
  for (unsigned int j = 0, n = arity(); j < n; ++j) {
      RATIONAL m = minus_exp[j].slope();
      RATIONAL h = minus_exp[j].offset();
      
      // Inicialización de intervalos: negativos y positivos
      NAT begin_neg = 0, end_neg = Inf;
      NAT begin_pos = 0, end_pos = Inf;
      
      if (m == 0) {  // Expresión constante
        if (h < 0) {
          begin_pos = 1;
          end_pos = 0;
        } else {
          begin_neg = 1;
          end_neg = 0;
        }
      }
      else if (m > 0) {  // Expresión creciente
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
      else {  // Expresión decreciente
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
      
      HybridPWMap jth(fact_);
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
    
    PWMapDelegPtr concatenatedPtr = ordpwmap_c.concatenation(*ith.restrict(dom)); 
    HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(*concatenatedPtr);
    ordpwmap_c.pieces_= std::move(othr.pieces_);


}

std::ostream &HybridPWMap::print(std::ostream &out) const
{
  //int sz = pieces_.size();

  out << "<<";
  for (const Map &m : pieces_){
        out << m << ", ";
    }
  /*
  if (sz > 0) {
    int i = 0;
    for (; i < sz - 1; ++i) {
      out << pieces_[i] << ", ";
    }
    out << pieces_[i];
  }
  */
  out << ">>";

  return out;
}

// PWMap functions -------------------------------------------------------------

std::size_t HybridPWMap::arity() const
{
  if (isEmpty())
    return 0;

  return pieces_.begin()->dom().arity();
}

bool HybridPWMap::isEmpty() const { 
//std::cout << "isEmpty\n";

return pieces_.empty(); }

Set HybridPWMap::dom() const
{
  //std::cout << "DOM\n";
  Set res = fact_.createSet();
  for (const Map &m : pieces_) {
    Set d = m.dom();
    res = res.disjointCup(d);
  }

  return res;
}

PWMapDelegPtr HybridPWMap::restrict(const Set &subdom) const
{
  //std::cout << "RESTRICT\n";

  HybridPWMap res(fact_);
  if (subdom.isEmpty())
    return std::make_unique<HybridPWMap>(res);
  /*
  HybridPWMap t(fact_);
    
  t.pieces_.reserve(pieces_.size());
  for (const Map& m : pieces_)
      t.emplaceBack(m);  

  std::sort(t.pieces_.begin(), t.pieces_.end());
  */
  const auto sdomMax = subdom.maxElem();
  const auto sdomMin = subdom.minElem();
  auto it = pieces_.begin();
  while (it != pieces_.end()) {
    const Map &m = *it;
    const auto domMax = m.dom().maxElem();
    const auto domMin = m.dom().minElem();

    if (domMax[0] < sdomMin[0]) {
      ++it;
      continue;
    }
    
    if (sdomMax[0] < domMin[0])
        break;

    if (!(domMax.menorThan(sdomMin)) && !(sdomMax.menorThan(domMin))) {
      Map resRest = m.restrict(subdom);
      if (!resRest.dom().isEmpty()) 
        res.emplaceBack(resRest);
      
    }

    ++it;
  }
  
  return std::make_unique<HybridPWMap>(res);
}




Set HybridPWMap::image() const
{
  Set res = fact_.createSet();

  for (const Map &m : pieces_) {
    res = res.cup(m.image());
  }

  return res;
}

Set HybridPWMap::image(const Set &subdom) const
{
  return restrict(subdom)->image();
}

Set HybridPWMap::preImage(const Set &subcodom) const
{
  Set res = fact_.createSet();

  for (const Map &m : pieces_)
    res = res.disjointCup(m.preImage(subcodom));

  return res;
}

PWMapDelegPtr HybridPWMap::inverse() const
{ 
  //std::cout << "inverse\n";

  HybridPWMap res(fact_);

  for (const Map &m : pieces_){
    const Map &inv = m.minInv();
      res.emplaceBack(m.minInv());
  }
    
    
  return std::make_unique<HybridPWMap>(res);
}


PWMapDelegPtr HybridPWMap::composition(const PWMapDelegate &other) const
{ 
  //std::cout << "composition\n";
  HybridPWMap res(fact_); 
  
  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);
  
  /*
  HybridPWMap t(fact_), o(fact_);

  t.pieces_.reserve(pieces_.size());
  for (const Map& m : pieces_)
      t.emplaceBack(m);  

  o.pieces_.reserve(othr.pieces_.size());
  for (const Map& m : othr.pieces_)
      o.emplaceBack(m);  

  std::sort(t.pieces_.begin(), t.pieces_.end());
  std::sort(o.pieces_.begin(), o.pieces_.end());
  */


  for (const Map &mO : othr.pieces_) {
    
    const auto mOMin = mO.image().minElem();
    const auto mOMax = mO.image().maxElem();
    
    for (const Map &mT : pieces_) {
      const auto mTMin = mT.dom().minElem();
      const auto mTMax = mT.dom().maxElem();
  
      if (mTMax[0] < mOMin[0]) {
        continue;
      }
  
      if (mOMax[0] < mTMin[0])
        break;
              
      if (!(mTMax.menorThan(mOMin)) && !(mOMax.menorThan(mTMin)) ) {
          auto resCom = mT.composition(mO);
  
          if (!resCom.dom().isEmpty())
              res.emplaceBack(resCom); 

      }

    }
    
  }
  return std::make_unique<HybridPWMap>(res);

}

PWMapDelegPtr HybridPWMap::mapInf(unsigned int n) const
{
  //std::cout << "mapinf\n";
  
  PWMapDelegPtr res = std::make_unique<HybridPWMap>(*this);
  PWMapDelegPtr old_res = std::make_unique<HybridPWMap>(fact_);

  if (!dom().isEmpty()) {
    for (unsigned int j = 0; old_res != res && j < n; ++j) {
      HybridPWMap *rs = static_cast<HybridPWMap *>(res.get());
      old_res = std::make_unique<HybridPWMap>(*rs);
      PWMapDelegPtr new_res = res->composition(*this);
      res = std::move(new_res);
    }

    if (*old_res == *res)
      return res;

    PWMapDelegPtr reduced = res->reduce();
    res = std::move(reduced);
    do {
      HybridPWMap *rs = static_cast<HybridPWMap *>(res.get());
      old_res = std::make_unique<HybridPWMap>(*rs);

      PWMapDelegPtr new_res = res->composition(*res);
      new_res = new_res->reduce();
      res = std::move(new_res);
    } while (*old_res != *res);
  }
  
  return res;
}

PWMapDelegPtr HybridPWMap::mapInf() const { return mapInf(0); }

// Extra operations ------------------------------------------------------------


PWMapDelegPtr HybridPWMap::concatenation(const PWMapDelegate &other) const
{

  //std::cout << "concatenation\n";
  PWMapDelegPtr res = std::make_unique<HybridPWMap>(*this);

  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);
  for (const Map &m2 : othr.pieces_)
    res->emplaceBack(m2);

  return res;
}


PWMapDelegPtr HybridPWMap::combine(const PWMapDelegate &other) const
{
   //std::cout << "vombine\n";
  
  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);

  if (isEmpty())
    return std::make_unique<HybridPWMap>(othr);

  if (other.isEmpty())
    return std::make_unique<HybridPWMap>(*this);

  PWMapDelegPtr res_ptr = std::make_unique<HybridPWMap>(*this);
  HybridPWMap &res = static_cast<HybridPWMap&>(*res_ptr);
  
  Set dom1 = dom();

  const auto dom1Max = dom1.maxElem();
  const auto dom1Min = dom1.minElem();
  for (const Map &m2 : othr.pieces_) {
    const auto dom2Max = m2.dom().maxElem();
    const auto dom2Min = m2.dom().minElem();
    Map resComb = fact_.createMap(m2.dom(), m2.exp());
    
     if (!(dom2Max.menorThan(dom1Min)) && !(dom1Max.menorThan(dom2Min)) ) {

          Set dom2 = m2.dom(), new_dom = dom2.difference(dom1);
          resComb = fact_.createMap(new_dom, m2.exp());
          
          
          if (resComb.dom().isEmpty())
            continue;
      }
      res.emplaceBack(std::move(resComb));
      
    }
  
  return  std::make_unique<HybridPWMap>(res);
}

PWMapDelegPtr HybridPWMap::reduce(const Interval &i, const LExp &le) const
{
  HybridPWMap res(fact_);

  if (!i.isEmpty()) {
    RATIONAL zero(0, 1);
    if (le.slope() == 1 && le.offset() != 0) {
      INT h = le.offset().toInt();

      NAT st = i.step();
      if (h == (INT) st) {
        NAT hi = i.end();
        RATIONAL const_expr(hi + st, 1);
        if (st < Inf - hi)
          res = HybridPWMap(fact_, fact_.createMap(i, LExp(zero, const_expr)));
      }

      else if (h == (INT) -st) {
        NAT lo = i.begin();
        RATIONAL const_expr(lo - st, 1);
        if (lo >= st)
          res = HybridPWMap(fact_, fact_.createMap(i, LExp(zero, const_expr)));
      }

      else if (h % (INT) st == 0) {
        // Is convenient the partition of the piece?
        if ((INT) i.cardinal() > h*h) {
          res = HybridPWMap(fact_);
          INT absh = std::abs(h);

          for (int k = 1; k <= absh; ++k) {
            NAT new_begin = i.begin() + k - 1;
            Interval kth_piece(new_begin, (NAT) absh, i.end());

            RATIONAL kth_off;
            if (h > 0)
              kth_off = kth_piece.end() + h;
            else
              kth_off = kth_piece.begin() + h;

            res.emplaceBack(fact_.createMap(kth_piece, LExp(0, kth_off)));
          }
        }
      }
    }

    else
      res.emplaceBack(fact_.createMap(i, le));
  }

  return std::make_unique<HybridPWMap>(res);
}


PWMapDelegPtr HybridPWMap::reduce(const Map &map) const
{
  HybridPWMap res(fact_);

  Set not_reduced = fact_.createSet();
  Exp e = map.exp();
  for (const SetPiece &dom_piece : map.dom()) {
    SetPiece aux_piece = dom_piece;
    Exp aux_exp = e;
    bool was_reduced = false;
    for (unsigned int j = 0; j < dom_piece.arity(); ++j) {
      PWMapDelegPtr aux = reduce(dom_piece[j], e[j]); 
      HybridPWMap *jth_red = static_cast<HybridPWMap *>(aux.get());
      for (const Map &ith_reduced : jth_red->pieces_) {
        aux_piece[j] = ith_reduced.dom().begin().operator*().operator[](0);
        aux_exp[j] = ith_reduced.exp()[0];
        if (aux_piece != dom_piece || aux_exp != e) {
          res.emplaceBack(fact_.createMap(aux_piece, aux_exp));
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
    res.emplaceBack(fact_.createMap(not_reduced, e)); // Add unreduced subpieces
  
  return std::make_unique<HybridPWMap>(res);
}

//TODO-med: trata de dilucidar para que sirve reduce
PWMapDelegPtr HybridPWMap::reduce() const
{ 
     //std::cout << "reduce\n";
  
    HybridPWMap res(fact_);
    for (const Map &m : pieces_) {
        PWMapDelegPtr ith = reduce(m);
        HybridPWMap *ith_c = static_cast<HybridPWMap *>(ith.get());
        for (const Map &ith_elem : ith_c->pieces_) {
             res.emplaceBack(ith_elem);
        }
    } 

    return std::make_unique<HybridPWMap>(res);
}

PWMapDelegPtr HybridPWMap::minMap(const PWMapDelegate &other) const
{
   //std::cout << "minmap\n";

  if (isEmpty() || other.isEmpty())
    return std::make_unique<HybridPWMap>(fact_);

  PWMapDelegPtr aux1 = restrict(other.dom()), aux2 = other.restrict(dom());
  Set zero = fact_.createSet(SetPiece(arity(), Interval(0, 1, 0)));
  Set to_zero = (*aux1 - *aux2)->preImage(zero);
  Set not_zero = aux2->dom().difference(to_zero);
  aux1 = aux1->restrict(to_zero);
  aux2 = aux2->restrict(not_zero);
  
  PWMapDelegPtr res = aux1->combine(*aux2);
  return res;
}



PWMapDelegPtr HybridPWMap::minAdjMap(const PWMapDelegate &other) const
{
  //std::cout << "MinADj\n";
  PWMapDelegPtr res = std::make_unique<HybridPWMap>(fact_);
  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);
  Set visited = fact_.createSet();

  processMapsOrd(othr,visited, static_cast<HybridPWMapRef>(*res) , &HybridPWMap::processMinAdjMap);
  return res;
}


//TODO-Low:terminar de enteder que pasa
void HybridPWMap::processMinAdjMap(const Map &m1, const Map &m2, Set &set, PWMapDelegate &ordpwmap,
     unsigned int*posGlobal) const 
{ 

  HybridPWMapRef ordpwmap_c = static_cast<HybridPWMapRef>(ordpwmap);
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
      HybridPWMap ith_pw(fact_, ith);
      Set again = dom_res.intersection(set);
      if (!again.isEmpty()) {
        PWMapDelegPtr aux_res = ordpwmap_c.restrict(dom_res);
        PWMapDelegPtr min_map = aux_res->minMap(ith_pw);
        PWMapDelegPtr new_resPtr = min_map->combine(ith_pw)->combine(ordpwmap_c);
        HybridPWMapCRef new_res_c = static_cast<HybridPWMapCRef>(*new_resPtr);
        ordpwmap_c.pieces_= std::move(new_res_c.pieces_);
        set = set.cup(ith_pw.dom());
      }
      else {
       
        ordpwmap_c.emplaceBack(ith);
        set = set.disjointCup(dom_res);
      }
    }
  }
}

PWMapDelegPtr HybridPWMap::firstInv(const Set &subdom) const
{
  //std::cout << "firstInv\n";
  HybridPWMap res(fact_);

  Set visited = fact_.createSet();
  for (const Map &m : pieces_) {
    Set res_dom = m.image(subdom).difference(visited);
    if (!res_dom.isEmpty()) {
      Map new_map = fact_.createMap(m.preImage(res_dom), m.exp());
      res.emplaceBack(new_map.minInv());
      visited = visited.cup(m.image(subdom));
    }
  }
  
  return std::make_unique<HybridPWMap>(res);
}

PWMapDelegPtr HybridPWMap::firstInv() const { return firstInv(dom()); }


PWMapDelegPtr HybridPWMap::filterMap(bool (*f)(const Map &)) const
{ 
  //std::cout << "FILTER\n";
  
  PWMapDelegPtr res = std::make_unique<HybridPWMap>(fact_);

  for (const Map &m : pieces_)
    if (f(m))
      res->emplaceBack(m);
  
  return res;
}

Set HybridPWMap::equalImage(const PWMapDelegate &other) const

{ //std::cout << "equalImage\n";   

  Set res = fact_.createSet();
  HybridPWMap noUsed(fact_);
  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);
  processMapsOrd(othr, res, noUsed, &HybridPWMap::processEqualImage);
  return res;

}

void HybridPWMap::processEqualImage(const Map &m1, const Map &m2, Set &set, PWMapDelegate &ordpwmap,
     unsigned int*posGlobal) const 
{ 
  Set cap_dom = m1.dom().intersection(m2.dom());
  if (!cap_dom.isEmpty()) {
    Map m1_cap = fact_.createMap(cap_dom, m1.exp());
    Map m2_cap = fact_.createMap(cap_dom, m2.exp());
    if (m1_cap == m2_cap)
      set = set.disjointCup(cap_dom);
  }
}

void HybridPWMap::processMapsOrd(
  const PWMapDelegate &other,
  Set &set,
  PWMapDelegate  &ordmap,
  ProcessFunc process
  ) const
{
  HybridPWMapCRef othr = static_cast<HybridPWMapCRef>(other);
  
  /*
  HybridPWMap t(fact_), o(fact_);
  
  t.pieces_.reserve(pieces_.size());
  for (const Map& m : pieces_)
      t.emplaceBack(m);  

  o.pieces_.reserve(othr.pieces_.size());
  for (const Map& m : othr.pieces_)
      o.emplaceBack(m);  

  std::sort(t.pieces_.begin(), t.pieces_.end());
  std::sort(o.pieces_.begin(), o.pieces_.end());
  */
  const HybridPWMap *shortMap = this;
  const HybridPWMap *longMap = &othr;

  std::forward_list<size_t> longIndices;
  auto liIt = longIndices.before_begin();

  const size_t longSize = longMap->pieces_.size();
  for (size_t i = 0; i < longSize; ++i)
    liIt = longIndices.insert_after(liIt, i);

  auto longBegin = longMap->pieces_.begin();
  unsigned int posGlobal = 0;
  for (const Map &m1 : shortMap->pieces_) {
    const auto m1Min = m1.dom().minElem();
    const auto m1Max = m1.dom().maxElem();
    const auto m1Min0 = m1Min[0];
    const auto m1Max0 = m1Max[0];
  
    auto liPrev = longIndices.before_begin();
    auto liCurr = longIndices.begin();
    
  
    while (liCurr != longIndices.end()) {
      size_t idx = *liCurr;
      const Map &m2 = *(longBegin + idx);
      const auto m2Min = m2.dom().minElem();
      const auto m2Max = m2.dom().maxElem();
      const auto m2Min0 = m2Min[0];
      const auto m2Max0 = m2Max[0];

      if (m2Max0 < m1Min0) {
        liCurr = longIndices.erase_after(liPrev);
        continue;
      }

      if (m1Max0 < m2Min0)
        break;

      if (!(m2Max.menorThan(m1Min)) && !(m1Max.menorThan(m2Min)) ) {
        (this->*process)(m1, m2, set, ordmap, &posGlobal);
      }

      ++liPrev;
      ++liCurr;
    }
    if (longIndices.empty())
      break;
  }
}

Set HybridPWMap::sharedImage() const
{
  Set not_present = dom().difference(firstInv()->image());
  Set res = preImage(image(not_present));

  return res;
}

PWMapDelegPtr HybridPWMap::offsetDom(const MD_NAT &off) const
{

    //std::cout << "OFSET\n";
  PWMapDelegPtr res = std::make_unique<HybridPWMap>(fact_);

  for (const Map &m : pieces_)
    res->emplaceBack(fact_.createMap(m.dom().offset(off), m.exp()));
  
  return res;
}

PWMapDelegPtr HybridPWMap::offsetDom(const PWMapDelegate &off) const
{
  //std::cout << "offsetDom\n";  
  HybridPWMap res(fact_);
  
  const auto sdomMax = off.dom().maxElem();
  const auto sdomMin = off.dom().minElem();
  for (const Map &m : pieces_) {
    const auto domMax = m.dom().maxElem();
    const auto domMin = m.dom().minElem();
    
    if (domMax[0] < sdomMin[0])
      continue;
    
    if (sdomMax[0] < domMin[0])
        break;
    
    if (!(domMax.menorThan(sdomMin)) && !(sdomMax.menorThan(domMin)) ) {
      Set ith_dom = off.image(m.dom());
      Map resMap = fact_.createMap(ith_dom, m.exp());
      
      if (!resMap.dom().isEmpty())
          res.emplaceBack(resMap);
      
    }
  }
  
  return std::make_unique<HybridPWMap>(res);

}

PWMapDelegPtr HybridPWMap::offsetImage(const MD_NAT &off) const
{
  //std::cout << "offsetImage\n";

  PWMapDelegPtr res = std::make_unique<HybridPWMap>(fact_);

  for (const Map &m : pieces_) {
    Exp e = m.exp(), res_e;
    for (unsigned int j = 0; j < e.arity(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    res->emplaceBack(fact_.createMap(m.dom(), res_e));
  }

  return res;
}

PWMapDelegPtr HybridPWMap::offsetImage(const Exp &off) const
{
    //std::cout << "offsetImage2\n";
  PWMapDelegPtr res = std::make_unique<HybridPWMap>(fact_);

  for (const Map &m : pieces_) 
    res->emplaceBack(fact_.createMap(m.dom(), off + m.exp()));

  return res;
}

PWMapDelegPtr HybridPWMap::compact() const
{
  HybridPWMap res(fact_);
  //std::cout << "compact\n";  
  if (dom().isEmpty())
    return std::make_unique<HybridPWMap>(res);
  
  std::forward_list<size_t> indices;
  auto liIt = indices.before_begin();

  const size_t lSize = pieces_.size();
  for (size_t i = 0; i < lSize; ++i)
    liIt = indices.insert_after(liIt, i);

  
  auto begin = pieces_.begin();
  //unsigned int posInit=0;
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
      
      res.emplaceBack(new_ith);
      liPrev = indices.before_begin();
      liCurr = indices.begin();
  }

  return std::make_unique<HybridPWMap>(res);
}

/*PWMapDelegPtr OrdPWMap::compact() const
{
  OrdPWMap res(fact_);

  if (dom().isEmpty())
    return std::make_unique<OrdPWMap>(res);
  
  std::forward_list<size_t> indices;
  auto liIt = indices.before_begin();

  const size_t lSize = pieces_.size();
  for (size_t i = 0; i < lSize; ++i)
    liIt = indices.insert_after(liIt, i);

  
  auto begin = pieces_.begin();
  //unsigned int posInit=0;
  Set compacted = fact_.createSet();
  for (auto it = pieces_.begin(); it != pieces_.end(); ++it) {

    //std::cout << "Conjunto=\n";
    //std::cout << compacted;
    //std::cout << "\n";
    Set ith_compacted = compacted.intersection(it->dom());
    if (ith_compacted.isEmpty()) {
      Map new_ith = fact_.createMap(it->dom().compact(), it->exp());

      auto liPrev = indices.before_begin();
      auto liCurr = indices.begin();

    
      while (liCurr != indices.end()) {
        size_t idx = *liCurr;
        const Map &nextMap = *(begin + idx);
        if (nextMap == *it){
            liCurr = indices.erase_after(liPrev);
            continue;
        }
        
        //Set next_compacted = compacted.intersection(nextMap.dom());
       // if (next_compacted.isEmpty()) {
          auto ith = new_ith.compact(nextMap);
          if (ith) {
            new_ith = ith.value();
            compacted = compacted.cup(nextMap.dom());
            liCurr = indices.erase_after(liPrev);
            continue;
          }
        //}

        ++liPrev;
        ++liCurr;
      }
      
      res.emplaceBack(new_ith);
    }
  }

  return std::make_unique<OrdPWMap>(res);
}
 */
 
 
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

} // namespace SBGpieces_.push_back(m);

