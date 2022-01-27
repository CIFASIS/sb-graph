/*****************************************************************************

    This file is part of Modelica C Compiler.

    Modelica C Compiler is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Modelica C Compiler is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Modelica C Compiler.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#include <boost/foreach.hpp>

#include <sbg/graph_builders/matching_graph_builder.h>
#include <sbg/sbg_printer.h>

namespace SBG
{
  MatchingGraphBuilder::MatchingGraphBuilder(Equations equations, Variables variables) : _U(), _F(), _eq_usage(), _equations(equations), _variables(variables), _model_name("matching_graph.dot") {}

  MatchingGraphBuilder::MatchingGraphBuilder(Equations equations, Variables variables, std::string model_name) : _U(), _F(), _eq_usage(), _equations(equations), _variables(variables), _model_name(model_name) {}

  void MatchingGraphBuilder::addDims(size_t max_dim, size_t exp_dim, SBG::MultiInterval &intervals, int offset)
  {
    if (max_dim > exp_dim)
    {
      for (size_t i = exp_dim; i < max_dim; i++)
      {
        Interval interval(offset, 1, offset);
        intervals.addInter(interval);
      }
    }
  }

  void MatchingGraphBuilder::addDims(size_t max_dim, size_t exp_dim, ORD_REALS &constant, ORD_REALS &slope)
  {
    ORD_REALS::iterator constant_it = constant.end();
    ORD_REALS::iterator slope_it = slope.end();
    if (max_dim > exp_dim)
    {
      for (size_t i = exp_dim; i < max_dim; i++)
      {
        constant_it = constant.insert(constant_it, 0);
        slope_it = slope.insert(slope_it, 0);
      }
    }
  }

  SBG::Set MatchingGraphBuilder::buildSet(VariableInfo variable, int offset, size_t max_dim)
  {
    std::vector<int> dims = variable.size;
    MultiInterval variable_intervals;
    if (dims.empty())
    {
      addDims(max_dim, 0, variable_intervals, offset);
    }
    else
    {
      for (int d : dims)
      {
        REAL res = d - 1;
        Interval interval(offset, 1, offset + res);
        variable_intervals.addInter(interval);
      }
      addDims(max_dim, dims.size(), variable_intervals, offset);
    }
    return createSet(variable_intervals);
  }

  SBG::Set MatchingGraphBuilder::buildSet(EquationInfo eq, std::string eq_id, int offset, size_t max_dim)
  {
    Usage usage;
    MultiInterval equation_intervals;
    if (eq.size.empty())
    {
      addDims(max_dim, 0, equation_intervals, offset);
    }
    else
    {
      std::vector<RangeDef> ranges = eq.size;
      for (RangeDef range : ranges)
      {
        REAL lower = range.begin;
        REAL step = range.step;
        REAL upper = range.end;
        REAL end = offset + upper - lower;
        Interval interval(offset, step, end);
        equation_intervals.addInter(interval);
        usage[range.iterator] = lower;
      }
      addDims(max_dim, ranges.size(), equation_intervals, offset);
      _eq_usage[eq_id] = usage;
    }
    return createSet(equation_intervals);
  }

  /**
   * @brief Add the offset to a given equation domain.
   *
   * @param dom
   * @param offset
   * @return SBG::Set The new set with all the offsets applied.
   */
  SBG::Set MatchingGraphBuilder::generateMapDom(SBG::Set dom, SBG::Set unk_dom, int offset, size_t max_dim)
  {
    MultiInterval edge_set_intervals;
    SBG::UNORD_MI atom_sets = dom.asets();
    BOOST_FOREACH (MultiInterval dom_intervals, atom_sets)
    {
      BOOST_FOREACH (Interval inter, dom_intervals.inters())
      {
        REAL end = inter.card() + offset - 1;
        edge_set_intervals.addInter(Interval(offset, inter.step(), end));
      }
      addDims(max_dim, dom_intervals.inters().size(), edge_set_intervals, offset);
    }
    return createSet(edge_set_intervals);
  }

  SetVertexDesc MatchingGraphBuilder::addVertex(std::string vertex_name, SBG::Set set, SBGraph &graph)
  {
    SetVertex V(vertex_name, num_vertices(graph) + 1, set, 0);
    SetVertexDesc v = boost::add_vertex(graph);
    graph[v] = V;
    return v;
  }

  void MatchingGraphBuilder::addEquation(EquationInfo eq, std::string id, SBG::Set set, SBGraph &graph)
  {
    _F.push_back(std::make_pair(addVertex(id, set, graph), eq));
  }

  PWLMap MatchingGraphBuilder::buildPWLMap(ORD_REALS constants, ORD_REALS slopes, SBG::Set dom)
  {
    LMap lmap(slopes, constants);

    OrdCT<Set> doms;
    doms.insert(doms.end(), dom);

    OrdCT<LMap> maps;
    maps.insert(maps.end(), lmap);
    PWLMap map(doms, maps);
    return map;
  }

  MatchingGraphBuilder::MatchingMaps MatchingGraphBuilder::generatePWLMaps(VariableUsage var_usage, SBG::Set dom, SBG::Set unk_dom, int offset, std::string eq_id, size_t max_dim)
  {
    ORD_REALS constant_pwl_map_u;
    ORD_REALS::iterator constant_pwl_map_u_it = constant_pwl_map_u.begin();
    ORD_REALS slope_pwl_map_u;
    ORD_REALS::iterator slope_pwl_map_u_it = slope_pwl_map_u.begin();
    ORD_REALS constant_pwl_map_f;
    ORD_REALS::iterator constant_pwl_map_f_it = constant_pwl_map_f.begin();
    ORD_REALS slope_pwl_map_f;
    ORD_REALS::iterator slope_pwl_map_f_it = slope_pwl_map_f.begin();

    SBG::ORD_INTS init_elems = unk_dom.minElem();
    assert(init_elems.size() == var_usage.usage.size() || var_usage.usage.size() == 0);
    SBG::ORD_INTS::iterator min_elem = init_elems.begin();
    SBG::Set map_dom = generateMapDom(dom, unk_dom, offset, max_dim);
    int map_offset = offset - 1;

    for (LinearMap map : var_usage.usage)
    {
      SBG::INT set_vertex_init = *min_elem - 1;
      Usage usage = _eq_usage[eq_id];
      int range_init_value = usage[map.iterator];
      int set_vertex_offset = map.slope * offset;
      int map_first_value = map.constant + map.slope * range_init_value + set_vertex_init;
      std::cout << set_vertex_init << " " << range_init_value << " " << map_first_value << " " << set_vertex_offset << std::endl;
      constant_pwl_map_u_it = constant_pwl_map_u.insert(constant_pwl_map_u_it, map_first_value - set_vertex_offset);
      slope_pwl_map_u_it = slope_pwl_map_u.insert(slope_pwl_map_u_it, map.slope);
      min_elem++;
    }
    if (var_usage.usage.empty())
    { // Scalar variable.
      SBG::INT set_vertex_init = *min_elem - 1;
      constant_pwl_map_u_it = constant_pwl_map_u.insert(constant_pwl_map_u_it, -map_offset + set_vertex_init);
      slope_pwl_map_u_it = slope_pwl_map_u.insert(slope_pwl_map_u_it, 1);
      addDims(max_dim, 1, constant_pwl_map_u, slope_pwl_map_u);
    }
    else
    {
      addDims(max_dim, var_usage.usage.size(), constant_pwl_map_u, slope_pwl_map_u);
    }
    for (SBG::INT init : dom.minElem())
    {
      SBG::INT set_vertex_init = init - 1;
      constant_pwl_map_f_it = constant_pwl_map_f.insert(constant_pwl_map_f_it, -map_offset + set_vertex_init);
      slope_pwl_map_f_it = slope_pwl_map_f.insert(slope_pwl_map_f_it, 1);
    }
    addDims(max_dim, dom.minElem().size(), constant_pwl_map_f, slope_pwl_map_f);
    return std::make_pair(buildPWLMap(constant_pwl_map_f, slope_pwl_map_f, map_dom), buildPWLMap(constant_pwl_map_u, slope_pwl_map_u, map_dom));
  }

  SBGraph MatchingGraphBuilder::build()
  {
    SBGraph graph;
    size_t max_dim = 1;
    // Get max dim defined in the model.
    for (VariableInfo var_info : _variables)
    {
      if (var_info.size.size() > max_dim)
      {
        max_dim = var_info.size.size();
      }
    }

    int set_vertex_offset = 1;
    // Build unknown nodes.
    for (VariableInfo var_info : _variables)
    {
      SBG::Set vertex_dom = buildSet(var_info, set_vertex_offset, max_dim);
      _U[var_info.name] = addVertex(var_info.name, vertex_dom, graph);
      set_vertex_offset += vertex_dom.card();
    }

    // Build equation nodes.
    int id = 1;
    for (EquationInfo eq_info : _equations)
    {
      SBG::Set vertex_dom;
      std::string eq_name = "eq_" + std::to_string(id++);
      vertex_dom = buildSet(eq_info, eq_name, set_vertex_offset, max_dim);
      set_vertex_offset += vertex_dom.card();
      addEquation(eq_info, eq_name, vertex_dom, graph);
    }

    // Build edges.
    int set_edge_offset = 1;
    unsigned int edge_id = 1;
    for (EquationDesc eq_desc : _F)
    {
      SetVertexDesc eq_vertex_desc = eq_desc.first;
      SetVertex eq_vertex = graph[eq_vertex_desc];
      SBG::Set dom = eq_vertex.range();
      // Add printers for debug
      //    std::cout << "Matched exps for: " << unknown_vertex.name() << " in " << eq_desc.second << std::endl;
      //    std::cout << "Equation dom: " << dom << std::endl;
      EquationInfo eq_info = eq_desc.second;
      for (VariableUsage var_usage : eq_info.var_usage)
      {
        SetVertexDesc unknown_vertex_desc = _U[var_usage.name];
        SBG::Set unk_dom = graph[unknown_vertex_desc].range();
        // std::cout << "Expression: " << exp << std::endl;
        MatchingMaps maps = generatePWLMaps(var_usage, dom, unk_dom, set_edge_offset, eq_vertex.name(), max_dim);
        set_edge_offset += dom.card();
        std::string edge_name = "E_" + std::to_string(edge_id++);
        std::cout << "MapF: " << maps.first << std::endl;
        std::cout << "MapU: " << maps.second << std::endl;
        SetEdge edge(edge_name, num_edges(graph) + 1, maps.first, maps.second, 0);
        SetEdgeDesc e;
        bool b;
        boost::tie(e, b) = boost::add_edge(eq_vertex_desc, unknown_vertex_desc, graph);
        graph[e] = edge;
      }
    }
    SBG::GraphPrinter printer(graph, 0);

    printer.printGraph("matching_graph.dot");

    return graph;
  }

} // namespace SBG
