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

#include <sbg/interval.hpp>

namespace SBG {

Interval::Interval() : begin_(1), step_(0), end_(0) {}
Interval::Interval(INT begin, INT step, INT end) : begin_(begin), step_(step), end_(end) 
{
  if (end >= begin) {
    int rem = fmod(end - begin, step);
    set_end(end - rem);
  }
}

member_imp(Interval, INT, begin);
member_imp(Interval, INT, step);
member_imp(Interval, INT, end);

bool Interval::operator==(const Interval &other) const
{
  return (begin() == other.begin()) && (step() == other.step()) && (end() == other.end());
}

std::ostream &operator<<(std::ostream &out, const Interval &i) 
{
  out << "[" << Util::to_str(i.begin());
  if (i.step() != 1)
    out << ":" << Util::to_str(i.step());
  out << ":" << Util::to_str(i.end()) << "]";
 
  return out;
}

// Type definitions ------------------------------------------------------------

std::size_t hash_value(Interval const& i) 
{
  std::size_t seed = 0;

  boost::hash_combine(seed, i.begin());
  boost::hash_combine(seed, i.step());
  boost::hash_combine(seed, i.end());

  return seed;
}

std::ostream &operator<<(std::ostream &out, const InterSet &ii) 
{
  out << "{";
  if (ii.size() > 0) {
    BOOST_FOREACH (Interval i, ii)
      out << i << ", "; 
    out << "\b\b";
  }
  out << "}";

  return out;
}

// Functions -------------------------------------------------------------------

unsigned int cardinal(Interval i) { return (i.end() - i.begin()) / i.step() + 1; }

bool isEmpty(Interval i) { return i.end() < i.begin(); }

bool isMember(INT x, Interval i)
{
  if (x < i.begin() || x > i.end()) return false;

  int rem = fmod(x - i.begin(), i.step());

  return rem == 0;
}

Interval intersection(Interval i1, Interval i2)
{
  if (isEmpty(i1) || isEmpty(i2)) return Interval();

  if ((i1.end() < i2.begin()) || (i2.end() < i1.begin())) return Interval();

  // Two non overlapping intervals with the same step
  if (i1.step() == i2.step() && !isMember(i1.begin(), i2) && !isMember(i2.begin(), i1)) return Interval();

  INT max_begin = std::max(i1.begin(), i2.begin());    
  INT new_step = std::lcm(i1.step(), i2.step()), new_begin = max_begin, new_end = std::min(i1.end(), i2.end());
  for (INT x = max_begin; x < max_begin + new_step - 1; x++) 
    if (isMember(x, i1) && isMember(x, i2)) 
      new_begin = x;

  return Interval(new_begin, new_step, new_end);
}

InterSet complement(Interval i)
{
  InterSet c;

  // Before interval
  if (i.begin() != 0)
    c.insert(Interval(0, 1, i.begin() - 1));  

  // After interval
  if (i.end() < Util::Inf)
    c.insert(Interval(i.end() + 1, 1, Util::Inf));

  else 
    c.insert(Interval(Util::Inf, 1, Util::Inf));

  // "During" interval
  if (i.begin() < Util::Inf)
    for (Util::INT j = 1; j < i.step(); j++)
      c.insert(Interval(i.begin()+j, i.step(), i.end()));

  return c;
}

InterSet difference(Interval i1, Interval i2) { 
  InterSet diff;

  BOOST_FOREACH (Interval ci2, complement(i2))
    diff.insert(intersection(i1, ci2)); 

  return diff;
}

} // namespace SBG
