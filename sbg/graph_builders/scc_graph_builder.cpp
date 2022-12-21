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

namespace SBG {

SCCGraphBuilder::SCCGraphBuilder(){};

SCCGraphBuilder::SCCGraphBuilder(MatchingStruct m) : mtchng_(m){}; 

member_imp(SCCGraphBuilder, MatchingStruct, mtchng);
member_imp(SCCGraphBuilder, DBSGraph, dg);

DSBGraph SCCGraphBuilder::build() 
{
  DSBGraph result;

  SBGraph g = mtchng().g();
  Set matched = mtchng.matchedE();

  BOOST_FOREACH(DSetEdgeDesc e, g.edges()) {
    Set e_dom = g[e].map_f().wholeDom();

    SetVertex v("", g[e].id(), e_dom, 0);

    SetVertexDesc dv = boost::add_vertex(result);
    result[dv] = v; 
  }

  return result;
}

} // namespace SBG
