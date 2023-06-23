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
Interval::Interval(INT begin, INT step, INT end) : begin_(begin), step_(step), end_(end) {
  if (end >= begin) {
    int rem = fmod(end - begin, step);
    set_end(end - rem);
  }
}

member_imp(Interval, INT, begin);
member_imp(Interval, INT, step);
member_imp(Interval, INT, end);

unsigned int cardinality(Interval i) { return (i.end() - i.begin()) / i.step(); }

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

Interval difference(Interval i1, Interval i2)
{
  return Interval(); // TODO
}

} // namespace SBG
