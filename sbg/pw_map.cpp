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
#include "sbg/pw_map.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// PWMap Delegate Constructors -------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMapDelegate::PWMapDelegate() {}

////////////////////////////////////////////////////////////////////////////////
// Unordered PWMap Implementation ----------------------------------------------
////////////////////////////////////////////////////////////////////////////////

member_imp(UnordPWMap, UnordMapCollection, pieces);

UnordPWMap::UnordPWMap() {}
UnordPWMap::UnordPWMap(const Set &s) : pieces_() {
  if (!s.isEmpty()) {
    SetPiece first = s.begin().operator*();
    pieces_.push_back(Map(s, Exp(first.arity(), LExp())));
  }
}
UnordPWMap::UnordPWMap(const Map &m) : pieces_() {
  if (!m.dom().isEmpty())
    pieces_.push_back(m);
}
UnordPWMap::UnordPWMap(const UnordMapCollection &pieces)
  : pieces_(std::move(pieces)) {}
UnordPWMap::UnordPWMap(const UnordPWMap &pw) : pieces_(pw.pieces_) {}

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
          Map map1(cap_dom, m1.exp());
          Map map2(cap_dom, m2.exp());
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

  return std::make_unique<UnordPWMap>(res);
}

PWMapDelegPtr UnordPWMap::operator-(const PWMapDelegate &other) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  if (isEmpty() || other.isEmpty())
    return res;

  Set univ = SET_FACT.createSet(SetPiece(arity(), Interval(0, 1, Inf)));

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map &m1 : pieces_) {
    for (const Map &m2 : othr.pieces_) {
      Set dom1 = m1.dom(), dom2 = m2.dom();  
      Set dom = dom1.intersection(dom2);
      if (!dom.isEmpty()) {
        Exp minus_exp = m1.exp() - m2.exp();
        UnordPWMap ith(univ);
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
          // Decresing expression
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

          UnordPWMap jth;
          Interval neg(begin_neg, 1, end_neg);
          Interval pos(begin_pos, 1, end_pos);
          for (const Map &m : ith.pieces_) {
            SetPiece mdi = *(m.dom().begin());
            Exp e = m.exp(); 

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
  Set res = SET_FACT.createSet();
  for (const Map &m : pieces_) {
    Set d = m.dom();
    res = res.disjointCup(d);
  }

  return res;
}

PWMapDelegPtr UnordPWMap::restrict(const Set &subdom) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  for (const Map &m : pieces_)
    res->emplaceBack(m.restrict(subdom));

  return res;
}

Set UnordPWMap::image() const
{
  Set res = SET_FACT.createSet();

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
  Set res = SET_FACT.createSet();

  for (const Map &m : pieces_)
    res = res.disjointCup(m.preImage(subcodom));

  return res;
}

PWMapDelegPtr UnordPWMap::inverse() const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  for (const Map &m : pieces_)
    res->emplaceBack(m.minInv());

  return res;
}

PWMapDelegPtr UnordPWMap::composition(const PWMapDelegate &other) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  Set im = othr.image(), new_dom = othr.preImage(im.intersection(dom()));

  PWMapDelegPtr aux = othr.restrict(new_dom);
  UnordPWMap *other_restricted = static_cast<UnordPWMap *>(aux.get());
  for (const Map &m1 : pieces_) 
    for (const Map &m2 : other_restricted->pieces_)
      res->emplaceBack(m1.composition(m2));

  return res;
}

PWMapDelegPtr UnordPWMap::mapInf(unsigned int n) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>(*this);
  PWMapDelegPtr old_res = std::make_unique<UnordPWMap>();

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

Set UnordPWMap::fixedPoints() const
{
  Set res = SET_FACT.createSet();

  for (const Map &m : pieces_)
    res = res.disjointCup(m.fixedPoints());

  return res;
}

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
    res->emplaceBack(Map(new_dom, m2.exp()));
  }

  return res;
}

PWMapDelegPtr UnordPWMap::reduce(const Interval &i, const LExp &le) const
{
  UnordPWMap res;

  if (!i.isEmpty()) {
    RATIONAL zero(0, 1);
    if (le.slope() == 1 && le.offset() != 0) {
      INT h = le.offset().toInt();

      NAT st = i.step();
      if (h == (INT) st) {
        NAT hi = i.end();
        RATIONAL const_expr(hi + st, 1);
        if (st < Inf - hi)
          res = UnordPWMap(Map(i, LExp(zero, const_expr)));
      }

      else if (h == (INT) -st) {
        NAT lo = i.begin();
        RATIONAL const_expr(lo - st, 1);
        if (lo >= st)
          res = UnordPWMap(Map(i, LExp(zero, const_expr)));
      }

      else if (h % (INT) st == 0) {
        // Is convenient the partition of the piece?
        if ((INT) i.cardinal() > h*h) {
          res = UnordPWMap();
          INT absh = std::abs(h);

          for (int k = 1; k <= absh; ++k) {
            NAT new_begin = i.begin() + k - 1;
            Interval kth_piece(new_begin, (NAT) absh, i.end());

            RATIONAL kth_off;
            if (h > 0)
              kth_off = kth_piece.end() + h;
            else
              kth_off = kth_piece.begin() + h;

            res.emplaceBack(Map(kth_piece, LExp(0, kth_off)));
          }
        }
      }
    }

    else
      res.emplaceBack(Map(i, le));
  }

  return std::make_unique<UnordPWMap>(res);
}

PWMapDelegPtr UnordPWMap::reduce(const Map &map) const
{
  UnordPWMap res;

  Set not_reduced = SET_FACT.createSet();
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
          res.emplaceBack(Map(aux_piece, aux_exp));
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

  res.emplaceBack(Map(not_reduced, e)); // Add unreduced subpieces

  return std::make_unique<UnordPWMap>(res);
}

PWMapDelegPtr UnordPWMap::reduce() const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  for (const Map &m : pieces_) {
    PWMapDelegPtr ith = reduce(m);
    res = res->concatenation(*ith);
  }

  return res;
}

PWMapDelegPtr UnordPWMap::minMap(const PWMapDelegate &other) const
{
  if (isEmpty() || other.isEmpty())
    return std::make_unique<UnordPWMap>();

  PWMapDelegPtr aux1 = restrict(other.dom()), aux2 = other.restrict(dom());

  Set min_in_pw1 = SET_FACT.createSet();
  PWMapDelegPtr off1 = aux1->offsetImage(MD_NAT(arity(), 1)); 
  PWMapDelegPtr subt = (*off1 - *aux2);
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

PWMapDelegPtr UnordPWMap::minAdjMap(const PWMapDelegate &other) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  Set visited = SET_FACT.createSet();
  for (const Map &m1 : pieces_) {
    for (const Map &m2 : othr.pieces_) {
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
          UnordPWMap ith_pw(ith);
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
  UnordPWMap res;

  Set visited = SET_FACT.createSet();
  for (const Map &m : pieces_) {
    Set res_dom = m.image(subdom).difference(visited);
    if (!res_dom.isEmpty()) {
      Map new_map(m.preImage(res_dom), m.exp());
      res.emplaceBack(new_map.minInv());

      visited = visited.cup(m.image(subdom));
    }
  }

  return std::make_unique<UnordPWMap>(res);
}

PWMapDelegPtr UnordPWMap::firstInv() const { return firstInv(dom()); }

PWMapDelegPtr UnordPWMap::filterMap(bool (*f)(const Map &)) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  for (const Map &m : pieces_)
    if (f(m))
      res->emplaceBack(m);

  return res;
}

Set UnordPWMap::equalImage(const PWMapDelegate &other) const
{
  Set res = SET_FACT.createSet();

  UnordPWMapCRef othr = static_cast<UnordPWMapCRef>(other);
  for (const Map &m1 : pieces_) {
    for (const Map &m2 : othr.pieces_) {
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

Set UnordPWMap::sharedImage() const
{
  Set not_present = dom().difference(firstInv()->image());
  Set res = preImage(image(not_present));

  return res;
}

PWMapDelegPtr UnordPWMap::offsetDom(const MD_NAT &off) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  for (const Map &m : pieces_)
    res->emplaceBack(Map(m.dom().offset(off), m.exp()));

  return res;
}

PWMapDelegPtr UnordPWMap::offsetDom(const PWMapDelegate &off) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  for (const Map &m : pieces_) {
    Set ith_dom = off.image(m.dom());
    res->emplaceBack(Map(ith_dom, m.exp()));
  }

  return res;
}

PWMapDelegPtr UnordPWMap::offsetImage(const MD_NAT &off) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  for (const Map &m : pieces_) {
    Exp e = m.exp(), res_e;
    for (unsigned int j = 0; j < e.arity(); ++j) {
      LExp res_lexp(e[j].slope(), e[j].offset() + (RATIONAL) off[j]);
      res_e.emplaceBack(res_lexp);
    }

    res->emplaceBack(Map(m.dom(), res_e));
  }

  return res;
}

PWMapDelegPtr UnordPWMap::offsetImage(const Exp &off) const
{
  PWMapDelegPtr res = std::make_unique<UnordPWMap>();

  for (const Map &m : pieces_) 
    res->emplaceBack(Map(m.dom(), off + m.exp()));

  return res;
}

PWMapDelegPtr UnordPWMap::compact() const
{
  UnordPWMap res;

  if (dom().isEmpty())
    return std::make_unique<UnordPWMap>(res);

  Set compacted = SET_FACT.createSet();
  for (auto it = pieces_.begin(); it != pieces_.end(); ++it) {
    auto next_it = it;
    ++next_it;
    Set ith_compacted = compacted.intersection(it->dom());
    if (ith_compacted.isEmpty()) {
      Map new_ith(it->dom().compact(), it->exp());
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

Set PWMap::fixedPoints() const { return delegate_->fixedPoints(); }

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

} // namespace SBG
