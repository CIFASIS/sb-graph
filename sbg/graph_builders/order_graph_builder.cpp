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
OrderGraphBuilder::OrderGraphBuilder(SCCStruct scc) : scc_(scc), result_() {
  set_rmapB(scc.rmap().compPW(scc.mapB()));
  set_rmapD(scc.rmap().compPW(scc.mapD()));
  
  Set all_edges = scc.Emap().wholeDom();
  Set all_reps = scc.rmap().image();

  Set part_edgesB = rmapB().preImage(all_reps), part_edgesD = rmapD().preImage(all_reps);
  set_part_edges(all_edges.cap(part_edgesB).cap(part_edgesD));
}

member_imp(OrderGraphBuilder, SCCStruct, scc);
member_imp(OrderGraphBuilder, Set, part_edges);
member_imp(OrderGraphBuilder, PWLMap, rmapB);
member_imp(OrderGraphBuilder, PWLMap, rmapD);
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

void OrderGraphBuilder::create_edges()
{
  DSBGraph dg = result_ref();

  std::cout << part_edges() << "\n\n";

  BOOST_FOREACH (MultiInterval mi, part_edges().asets()) {
    Set mi_set(mi);
    std::cout << mi_set << "\n";
    PWLMap left = rmapB_ref().restrictMap(mi_set), right = rmapD_ref().restrictMap(mi_set);  
    Set left_reps = left.image(), right_reps = right.image();
    auto lv = get_vertex(left_reps, dg), rv = get_vertex(right_reps, dg);
    std::cout << *lv << " | " << *rv << "\n";

    bool b;
    DSetEdgeDesc ed;
    boost::tie(ed, b) = boost::edge(*lv, *rv, dg);
    std::cout << b << "\n";

    if (left_reps != right_reps) {
      if (b) {
        DSetEdge e = dg[ed];
        PWLMap new_left = e.map_b_ref().concat(left), new_right = e.map_d_ref().concat(right);
        result_ref()[ed] = DSetEdge(e.name(), new_left, new_right);
      }
   
      else {
        boost::tie(ed, b) = boost::add_edge(*lv, *rv, result_ref());
        std::string l_name = get_name(left_reps, dg), r_name = get_name(right_reps, dg);
        result_ref()[ed] = DSetEdge(l_name + r_name, left, right);
      }
    }
    std::cout << "\n";
  }

  return;
}

void OrderGraphBuilder::build()
{
  create_vertices(); 
  create_edges();

  return;
}

} // namespace SBG
