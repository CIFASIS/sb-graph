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

#include "sbg/set.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Generic implementation ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

Set::Set(UnordSet c) : pieces_(c) {}
Set::Set(OrdSet c) : pieces_(c) {}

member_imp(Set, SetType, pieces);

std::size_t Set::size()
{
  return std::visit([mdi](auto &v) {
    v.size();
  }, pieces_ref());
}

void Set::emplace(SetPiece mdi)
{
  std::visit([mdi](auto &v) {
    v.emplace(mdi);
  }, pieces_ref());
  return;
}

SetPiece Set::operator[](std::size_t n)
{
  return std::visit([n](auto &v) {
    return v[n];
  }, pieces_ref());
}

bool Set::operator<(const Set &other) const
{
  Set s1 = *this, s2 = other;
  bool res = std::visit(Util::Overload {
    [](UnordSet aux1, UnordSet aux2) { return aux1 < aux2; },
    [](OrdSet aux1, OrdSet aux2) { return aux1 < aux2; },
    [](auto aux1, auto aux2) {
      if (isEmpty(aux1)) return true;
      if (isEmpty(aux2)) return false;

      Util::ERROR("LIB::Set::operator<: mismatched containers");
      return false;
    }
  }, s1.pieces(), s2.pieces());

   return res;
}

bool Set::operator==(const Set &other) const
{
  Set s1 = *this, s2 = other;
  bool res = std::visit(Util::Overload {
      [](UnordSet aux1, UnordSet aux2) { return aux1 == aux2; },
      [](OrdSet aux1, OrdSet aux2) { return aux1 == aux2; },
      [](auto aux1, auto aux2) {
        if (isEmpty(aux1)) return isEmpty(aux2);
        if (isEmpty(aux2)) return isEmpty(aux1);

        Util::ERROR("LIB::Set::operator==: mismatched containers");
        return false;
      }
  }, s1.pieces(), s2.pieces());

  return res;
}

std::ostream &operator<<(std::ostream &out, const Set &s)
{
  std::visit([&out](auto v) {
    out << v;
  }, s.pieces());

  return out;
}

void foreach(std::function<void (SetPiece)> f, Set s)
{
  std::visit([f](auto v) {
    for (SetPiece mdi : v) f(mdi);
    return;
  }, s.pieces());
  return;
}

// Set operations --------------------------------------------------------------

unsigned int cardinal(Set s)
{
  return std::visit([](auto v) {
    return cardinal(v);
  }, s.pieces());
}

bool isEmpty(Set s)
{
  return std::visit([](auto v) {
    return isEmpty(v);
  }, s.pieces());
}

bool isMember(MD_NAT x, Set s)
{
  return std::visit([x](auto v){
    return isMember(x, v);
  }, s.pieces());
}

Util::MD_NAT minElem(Set s)
{
  return std::visit([](auto v){
    return minElem(v);
  }, s.pieces());
}

Util::MD_NAT maxElem(Set s)
{
  return std::visit([](auto v){
    return maxElem(v);
  }, s.pieces());
}

Set intersection(Set s1, Set s2)
{
  std::cout << s1 << " | " << s2 << "\n";
  return std::visit(Util::Overload {
    [](UnordSet aux1, UnordSet aux2) { return Set(intersection(aux1, aux2)); },
    [](OrdSet aux1, OrdSet aux2) { return Set(intersection(aux1, aux2)); },
    [](UnordSet aux1, OrdSet aux2) {
      if (isEmpty(aux1)) return Set(OrdSet());

      Util::ERROR("LIB::Set::intersection: mismatched containers 1");
      return Set(UnordSet());
    },
    [](OrdSet aux1, UnordSet aux2) {
      if (isEmpty(aux2)) return Set(OrdSet());

      Util::ERROR("LIB::Set::intersection: mismatched containers 2");
      return Set(UnordSet());
    }
  }, s1.pieces(), s2.pieces());
}

Set cup(Set s1, Set s2)
{
  return std::visit(Util::Overload {
      [](UnordSet aux1, UnordSet aux2) { return Set(cup(aux1, aux2)); },
      [](OrdSet aux1, OrdSet aux2) { return Set(cup(aux1, aux2)); },
      [](UnordSet aux1, OrdSet aux2) {
        if (isEmpty(aux1)) return Set(aux2);

        Util::ERROR("LIB::Set::cup: mismatched containers 1");
        return Set(UnordSet());
      },
      [](OrdSet aux1, UnordSet aux2) {
        if (isEmpty(aux2)) return Set(aux1);

        Util::ERROR("LIB::Set::cup: mismatched containers 2");
        return Set(UnordSet());
      }
  }, s1.pieces(), s2.pieces());
}

Set difference(Set s1, Set s2)
{
  return std::visit(Util::Overload {
      [](UnordSet aux1, UnordSet aux2) { return Set(difference(aux1, aux2)); },
      [](OrdSet aux1, OrdSet aux2) { return Set(difference(aux1, aux2)); },
      [](UnordSet aux1, OrdSet aux2) {
        if (isEmpty(aux1)) return Set(OrdSet());

        Util::ERROR("LIB::Set::difference: mismatched containers 1");
        return Set(UnordSet());
      },
      [](OrdSet aux1, UnordSet aux2) {
        if (isEmpty(aux2)) return Set(aux1);

        Util::ERROR("LIB::Set::difference: mismatched containers 2");
        return Set(UnordSet());
      }
  }, s1.pieces(), s2.pieces());
}

// Extra operations ------------------------------------------------------------

Set concatenation(Set s1, Set s2)
{
  return std::visit(Util::Overload {
      [](UnordSet aux1, UnordSet aux2) { return Set(concatenation(aux1, aux2)); },
      [](OrdSet aux1, OrdSet aux2) { return Set(concatenation(aux1, aux2)); },
      [](UnordSet aux1, OrdSet aux2) {
        if (isEmpty(aux1)) return Set(aux2);

        Util::ERROR("LIB::Set::concatenation: mismatched containers 1");
        return Set(UnordSet());
      },
      [](OrdSet aux1, UnordSet aux2) {
        if (isEmpty(aux2)) return Set(aux1);

        Util::ERROR("LIB::Set::concatenation: mismatched containers 2");
        return Set(UnordSet());
      }
  }, s1.pieces(), s2.pieces());
}

Set filterSet(bool (*f)(SetPiece), Set s)
{
  return std::visit([&f](auto v){
   return Set(filterSet(f, v));
  }, s.pieces());
}

Set offset(Util::MD_NAT off, Set s)
{
  return std::visit([off](auto v){
    return Set(offset(off, v));
  }, s.pieces());
}

std::size_t hash_value(const Set &s)
{
  return std::visit([](auto v){
    return hash_value(v);
  }, s.pieces());
}

} // namespace LIB

} // namespace SBG
