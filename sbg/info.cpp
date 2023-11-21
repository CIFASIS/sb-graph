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

#include "sbg/info.hpp"

namespace SBG {

namespace LIB {

DeclRanged::DeclRanged() : prefix_(), name_(), reps_() {}
DeclRanged::DeclRanged(std::string prefix, std::string name, SetPiece reps) 
  : prefix_(prefix), name_(name), reps_(reps) {}

member_imp(DeclRanged, std::string, prefix);
member_imp(DeclRanged, std::string, name);
member_imp(DeclRanged, SetPiece, reps);

std::ostream &operator<<(std::ostream &out, const DeclRanged &dr)
{
  out << dr.prefix() << " " << dr.name();
  
  SetPiece mdi = dr.reps();
  unsigned int sz = mdi.size();
  if (sz > 0) {
    out << "[";
    unsigned int j = 0;
    for (; j < sz - 1; ++j) {
      out << cardinal(mdi[j]) << ", ";
    }
    out << cardinal(mdi[j]);
    out << "]";
  }
  out << ";";

  return out;
}

std::ostream &operator<<(std::ostream &out, const DeclsRanged &ddr)
{
  BOOST_FOREACH (DeclRanged dr, ddr) out << dr << "\n";

  return out;
}

VertexInfo::VertexInfo() : decl_() {}
VertexInfo::VertexInfo(DeclRanged decl) : decl_(decl) {}

member_imp(VertexInfo, DeclRanged, decl);

std::ostream &operator<<(std::ostream &out, const VertexInfo &vi)
{
  out << vi.decl();

  return out;
}

std::ostream &operator<<(std::ostream &out, const VerticesInfo &vvi)
{
  BOOST_FOREACH (VertexInfo vi, vvi) out << vi << "\n";

  return out;
}

UseRanged::UseRanged() : prefix_(), name_(), subs_(), suffix_() {}
UseRanged::UseRanged(std::string prefix, std::string name, MDLExp subs, std::string suffix) 
  : prefix_(prefix), name_(name), subs_(subs), suffix_(suffix) {}

member_imp(UseRanged, std::string, prefix);
member_imp(UseRanged, std::string, name);
member_imp(UseRanged, MDLExp, subs);
member_imp(UseRanged, std::string, suffix);

std::ostream &operator<<(std::ostream &out, const UseRanged &ur)
{
  out << ur.prefix() << ur.name();
  
  MDLExp mdle = ur.subs();
  unsigned int sz = mdle.size();
  if (sz > 0) {
    out << "[";
    unsigned int j = 0;
    for (; j < sz - 1; ++j) out << mdle[j] << ", ";
    out << mdle[j];
    out << "]"; 
  }

  out << ur.suffix(); 

  return out;
}

std::ostream &operator<<(std::ostream &out, const UsesRanged &uur)
{
  BOOST_FOREACH (UseRanged ur, uur) out << ur;

  return out;
}

RangedExpr::RangedExpr() : range_(), expr_() {}
RangedExpr::RangedExpr(SetPiece range, UsesRanged expr) : range_(range), expr_(expr) {}

member_imp(RangedExpr, SetPiece, range);
member_imp(RangedExpr, UsesRanged, expr);

std::string iterators[7] = {"i", "j", "k", "l", "m", "n", "o"};

std::ostream &operator<<(std::ostream &out, const RangedExpr &re)
{
  SetPiece mdi = re.range();
  unsigned int sz = mdi.size();
  if (sz > 0) {
    out << "for ";

    unsigned int j = 0;
    for (; j < sz - 1; ++j) out << iterators[j] << " in " << mdi[j] << ", ";
    out << iterators[j] << " in " << mdi[j];

    out << " loop\n" << re.expr() << "\nend for;";
  }

  else out << re.expr(); 

  return out;
}

EdgeInfo::EdgeInfo() : expr_() {}
EdgeInfo::EdgeInfo(RangedExpr expr) : expr_(expr) {}

member_imp(EdgeInfo, RangedExpr, expr);

std::ostream &operator<<(std::ostream &out, const EdgeInfo &ei)
{
  out << ei.expr();

  return out;
}

std::ostream &operator<<(std::ostream &out, const EdgesInfo &eei)
{
  BOOST_FOREACH (EdgeInfo ei, eei) out << ei << "\n";

  return out;
}

} // namespace LIB

} // namespace SBG
