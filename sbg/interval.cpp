/*****************************************************************************

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
#include <boost/foreach.hpp>

#include <sbg/interval.hpp>

namespace SBG {

// Intervals --------------------------------------------------------------------------------------

INTER_TEMPLATE
INTER_TEMP_TYPE::IntervalImp1(){};

INTER_TEMPLATE
INTER_TEMP_TYPE::IntervalImp1(bool empty) : lo_(-1), step_(-1), hi_(-1), empty_(empty){};

INTER_TEMPLATE
INTER_TEMP_TYPE::IntervalImp1(INT lo, INT step, INT hi)
{
  set_lo(-1);
  set_step(-1);
  set_hi(-1);
  set_empty(true);

  if (lo >= 0 && step > 0 && hi >= lo) {
    set_empty(false);
    set_lo(lo);
    set_step(step);

    if (hi < Inf) {
      int rem = std::fmod(hi - lo, step);
      set_hi(hi - rem);
    }

    else
      set_hi(Inf);
  }

  else if (lo >= 0 && step == 0 && hi == lo) {
    set_empty(false);
    set_lo(lo);
    set_hi(hi);
    set_step(1);
  }
};

member_imp_temp(INTER_TEMPLATE, INTER_TEMP_TYPE, INT, lo);
member_imp_temp(INTER_TEMPLATE, INTER_TEMP_TYPE, INT, step);
member_imp_temp(INTER_TEMPLATE, INTER_TEMP_TYPE, INT, hi);
member_imp_temp(INTER_TEMPLATE, INTER_TEMP_TYPE, bool, empty);

INTER_TEMPLATE
INT INTER_TEMP_TYPE::gcd(INT a, INT b)
{
  INT c;

  do {
    c = a % b;
    if (c > 0) {
      a = b;
      b = c;
    }
  } while (c != 0);

  return b;
}

INTER_TEMPLATE
INT INTER_TEMP_TYPE::lcm(INT a, INT b)
{
  if (a < 0 || b < 0) return -1;

  return (a * b) / gcd(a, b);
}

INTER_TEMPLATE
bool INTER_TEMP_TYPE::isIn(INT x)
{
  if (x < lo() || x > hi() || empty()) return false;

  float aux = fmod(x - lo(), step());
  if (aux == 0) return true;

  return false;
}

INTER_TEMPLATE
int INTER_TEMP_TYPE::card()
{
  int res = 0;

  if (step() != 0) res = (hi() - lo()) / step() + 1;

  return res;
}

INTER_TEMPLATE
INTER_TEMP_TYPE INTER_TEMP_TYPE::offset(int off)
{
  if (!empty())
    return IntervalImp1(lo() + off, step(), hi() + off);

  else
    return IntervalImp1(true);
}

INTER_TEMPLATE
INTER_TEMP_TYPE INTER_TEMP_TYPE::cap(INTER_TEMP_TYPE i2)
{
  INT maxLo = std::max(lo(), i2.lo()), newLo = -1;
  INT newStep = lcm(step(), i2.step());
  INT newEnd = std::min(hi(), i2.hi());

  if (!empty() && !i2.empty())
    for (INT i = 0; i < newStep; i++) {
      INT res1 = maxLo + i;

      if (isIn(res1) && i2.isIn(res1)) {
        newLo = res1;
        break;
      }
    }

  else
    return IntervalImp1(true);

  if (newLo < 0) return IntervalImp1(true);
 

  return IntervalImp1(newLo, newStep, newEnd);
}

INTER_TEMPLATE
UNIQUE_ORD_CT<INTER_TEMP_TYPE> INTER_TEMP_TYPE::complement()
{
  UNIQUE_ORD_CT<IntervalImp1> res;

  if (empty()) return res;

  // "Before" interval
  if (lo() > 0) res.insert(res.end(), IntervalImp1(0, 1, lo() - 1));

  // "During" interval
  if (step() > 1) {
    for (int i = 1; i < step(); i++)
      res.insert(res.end(), IntervalImp1(lo() + i, step(), hi()));
  }

  // "After" interval
  res.insert(res.end(), IntervalImp1(hi() + 1, 1, Inf));
  
  return res;
}

INTER_TEMPLATE
UNIQUE_ORD_CT<INTER_TEMP_TYPE> INTER_TEMP_TYPE::diff(INTER_TEMP_TYPE i2)
{
  UNIQUE_ORD_CT<IntervalImp1> res;
  IntervalImp1 capres = cap(i2);

  if (capres.empty()) {
    IntervalImp1 aux = *this;
    res.insert(aux);
    return res;
  }

  if (capres == *this) return res;

  // "Before" intersection
  if (lo() < capres.lo()) {
    IntervalImp1 aux = IntervalImp1(lo(), step(), capres.lo() - 1);
    IntervalImp1 left = cap(aux);
    res.insert(left);
  }

  // "During" intersection
  if (capres.step() <= (capres.hi() - capres.lo())) {
    int nInters = capres.step() / step();
    for (int i = 1; i < nInters; i++) {
      IntervalImp1 aux = IntervalImp1(capres.lo() + i * step(), capres.step(), capres.hi());
      res.insert(aux);
    }
  }

  // "After" intersection
  if (hi() > capres.hi()) {
    IntervalImp1 aux = IntervalImp1(capres.hi() + step(), step(), hi());
    IntervalImp1 right = cap(aux);
    res.insert(right);
  }

  return res;
}

INTER_TEMPLATE
INT INTER_TEMP_TYPE::minElem() { return lo(); }

INTER_TEMPLATE
INT INTER_TEMP_TYPE::nextElem(INT cur) { return cur + step(); }

INTER_TEMPLATE
INT INTER_TEMP_TYPE::maxElem() { return hi(); }

INTER_TEMPLATE
INTER_TEMP_TYPE INTER_TEMP_TYPE::normalize(INTER_TEMP_TYPE i2)
{
  INT l = lo();
  INT st = step();
  INT h = hi();
  INT l2 = i2.lo();
  INT h2 = i2.hi();

  if (st == i2.step()) {
    if (h + st == l2 || isIn(l2))
      return IntervalImp1(l, st, h2);

    else if (h2 + st == l || i2.isIn(l))
      return IntervalImp1(l2, st, h);
  }

  return IntervalImp1(true);
}

INTER_TEMPLATE
bool INTER_TEMP_TYPE::operator==(const INTER_TEMP_TYPE &other) const
{
  INT l = lo(), ol = other.lo();
  INT s = step(), os = other.step();
  INT h = hi(), oh = other.hi();

  bool cond1 = l == h && l == ol && h == oh;
  bool cond2 = (l == ol) && (s == os) && (h == oh) && (empty() == other.empty());
  return cond1 || cond2;
}

INTER_TEMPLATE
bool INTER_TEMP_TYPE::operator!=(const INTER_TEMP_TYPE &other) const { return !(*this == other); }

INTER_TEMPLATE
bool INTER_TEMP_TYPE::operator<(const INTER_TEMP_TYPE &other) const { return lo() < other.lo(); }

template struct IntervalImp1<UniqueOrdCT>;

INTER_TEMPLATE
std::ostream &operator<<(std::ostream &out, const INTER_TEMP_TYPE &i)
{
  out << "[" << i.lo() << ":" << i.step() << ":" << i.hi() << "]";
  return out;
}

template std::ostream &operator<<(std::ostream &out, const Interval &i);

size_t hash_value(const Interval &inter)
{
  size_t seed = 0;

  boost::hash_combine(seed, inter.lo());
  boost::hash_combine(seed, inter.step());
  boost::hash_combine(seed, inter.hi());

  return seed;
}

std::ostream &operator<<(std::ostream &out, const ORD_INTERS &inters)
{
  out << "[";
  if (inters.size() == 1)
    out << *(inters.begin());

  else if (inters.size() > 1) {
    for (auto it = inters.begin(); std::next(it) != inters.end(); it++) out << *it << ", ";
    out << *(std::prev(inters.end()));
  }
  out << "]";

  return out;
}

std::ostream &operator<<(std::ostream &out, const UNIQUE_ORD_INTERS &inters)
{
  out << "{¿";
  if (inters.size() == 1)
    out << *(inters.begin());

  else if (inters.size() > 1) {
    for (auto it = inters.begin(); std::next(it) != inters.end(); it++) out << *it << ", ";
    out << *(std::prev(inters.end()));
  }
  out << "?}";

  return out;
}

std::ostream &operator<<(std::ostream &out, const UNORD_INTERS &inters)
{
  UNORD_INTERS auxinters = inters;
  Interval i1 = *(inters.begin());

  out << "{";
  if (auxinters.size() == 1)
    out << i1;

  else if (auxinters.size() > 1) {
    auxinters.erase(i1);

    BOOST_FOREACH (Interval i, auxinters)
      out << i << ", ";
    out << i1;
  }
  out << "}";

  return out;
}

// >>>>> To add new implementation, add new methods:
// X IntervalImp2::func1() { ... }
// X IntervalImp2::func2() { ... }
// ...
// ...
//
// template struct IntervalImp2<UnordCT>; --- If it is a template class
//
// Then modify hash_value implementation for Interval

}  // namespace SBG
