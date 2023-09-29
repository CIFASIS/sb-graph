/** @file info.hpp

 @brief <b>SBG information structures</b>

 These structures were designed to keep information about variable names and
 equations definitions in the ModelicaCC context. In sense, they are an
 interface between Modelica code and the SBG representation. They store
 information to print Modelica-like code after each flatter/causalization
 stage in which an SBG is used.

 <hr>

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

#ifndef SBG_INFO_HPP
#define SBG_INFO_HPP

#include <sbg/multidim_inter.hpp>
#include <sbg/multidim_lexp.hpp>

namespace SBG {

namespace LIB {

/** @struct DeclRanged
 *
 * @brief DeclRanged is used to represent the declaration of an array of
 * objects.
 *
 * In the Modelica context, the declaration of Real a[100] will result in the
 * creation of interval [700:799], so there will be a VertexInfo vi with a
 * DeclRanged dr such that dr.prefix = "Real", dr.name = "a", 
 * dr.reps = [700:799].
 */

struct DeclRanged {
  member_class(std::string, prefix);
  member_class(std::string, name);
  member_class(SetPiece, reps);

  DeclRanged();
  DeclRanged(std::string prefix, std::string name, SetPiece reps);
};
std::ostream &operator<<(std::ostream &out, const DeclRanged &rd);

typedef std::vector<DeclRanged> DeclsRanged;
std::ostream &operator<<(std::ostream &out, const DeclsRanged &rd);

struct VertexInfo {
  member_class(DeclRanged, decl);

  VertexInfo();
  VertexInfo(DeclRanged decl);
};
std::ostream &operator<<(std::ostream &out, const VertexInfo &vi);

typedef std::vector<VertexInfo> VerticesInfo;
std::ostream &operator<<(std::ostream &out, const VerticesInfo &vvi);

/** @struct UseRanged
 *
 * @brief UseRanged should keep info about the use of an array of variables.
 * The trail member is used to specify any possible code between two
 * consecutive arrays.
 *
 * For example, when the array "a" is used by a loop:
 *   for i [5:5:50] loop
 *     a[i] = b[i];
 *   end for;
 * the corresponding UseRanged ur will be such that ur.prefix = "", 
 * ur.name = "a", ur.subs = 1*x+0, ur.suffix = " = "
 */

struct UseRanged {
  member_class(std::string, prefix);
  member_class(std::string, name);
  member_class(MDLExp, subs);
  member_class(std::string, suffix);

  UseRanged();
  UseRanged(std::string prefix, std::string name, MDLExp subs, std::string suffix);
};
std::ostream &operator<<(std::ostream &out, const UseRanged &rd);

typedef std::vector<UseRanged> UsesRanged;
std::ostream &operator<<(std::ostream &out, const UsesRanged &uur);

struct RangedExpr {
  member_class(SetPiece, range);
  member_class(UsesRanged, expr);

  RangedExpr();
  RangedExpr(SetPiece range, UsesRanged expr);
};
std::ostream &operator<<(std::ostream &out, const RangedExpr &re);

struct EdgeInfo {
  member_class(RangedExpr, expr);

  EdgeInfo();
  EdgeInfo(RangedExpr expr);
};
std::ostream &operator<<(std::ostream &out, const EdgeInfo &ri);

typedef std::vector<EdgeInfo> EdgesInfo;
std::ostream &operator<<(std::ostream &out, const EdgesInfo &eei);

} // namespace LIB

}  // namespace SBG
 
#endif
