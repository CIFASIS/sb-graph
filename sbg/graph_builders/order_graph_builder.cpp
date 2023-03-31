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
#include <sbg/graph_builders/order_graph_builder.hpp>

namespace SBG {

OrderGraphBuilder::OrderGraphBuilder() : scc_(), result_() {}
OrderGraphBuilder::OrderGraphBuilder(SCCStruct scc) : scc_(scc), result_() {}

member_imp(OrderGraphBuilder, SCCStruct, scc);
member_imp(OrderGraphBuilder, DSBGraph, result);

Set OrderGraphBuilder::get_representants()
{
  Set result;

  parallel_foreach2 (scc_ref().rmap_ref().dom_ref(), scc_ref().rmap_ref().lmap_ref()) {
    Set d = boost::get<0>(items);
    LMap l = boost::get<1>(items);

    BOOST_FOREACH (MultiInterval mi, d.asets()) {
      AtomPWLMap apw(mi, l);

      if (apw.isId())
        result.addAtomSet(mi);
    }
  }

  return result;
}

void OrderGraphBuilder::create_vertices()
{
  BOOST_FOREACH (MultiInterval mi, get_representants().asets()) {
    DSetVertexDesc vd = boost::add_vertex(result_ref());
    result_ref()[vd] = SetVertex(get_name(Set(mi), scc().g()), Set(mi));
  }

  return;
}

void OrderGraphBuilder::sort()
{
  create_vertices(); 

  Set all_edges = scc().Emap().wholeDom();
  PWLMap rmapB = scc().rmap().compPW(scc().mapB()), rmapD = scc().rmap().compPW(scc().mapD());
  Set all_reps = scc().rmap().image();

  Set part_edgesB = rmapB.preImage(all_reps), part_edgesD = rmapD.preImage(all_reps);
  Set part_edges = all_edges.cap(part_edgesB).cap(part_edgesD);


  return;
}

} // namespace SBG
