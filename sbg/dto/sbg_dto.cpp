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
#include <sbg/dto/converters/pw_map_dto_converter.hpp>
#include <sbg/dto/converters/pw_mdinter_dto_converter.hpp>
#include <sbg/dto/sbg_dto.hpp>

#include <sbg/dto/pw_mdinter_dto.hpp>

namespace SBG {

namespace API {

template<typename NodeUserData, typename EdgeUserData>
CanonDSBG<NodeUserData, EdgeUserData>::CanonDSBG() //TODO: Add ctor with json.
    : _node_user_data_map(),
      _edge_user_data_map(),
      _graph()
{
}

template<typename NodeUserData, typename EdgeUserData>
CanonDSBG<NodeUserData, EdgeUserData>::CanonDSBG() //TODO: Add ctor with json.
    : _node_user_data_map(),
      _edge_user_data_map(),
      _graph()
{

}




















//TODO: separate file into two.


// JSON ------------------------------------------------------



//V = {[100:198], [0:0], [1:99]};
//Vmap = <<{[100:198]} ↦ 1, {[0:0]} ↦ 2, {[1:99]} ↦ 3>>;
//
//E = {[200:297], [199:199], [298:396]}
//map1 = <<{[199:199]} ↦ x-199, {[200:297]} ↦ x-199, {[298:396]} ↦ x-297>>
//map2 = <<{[199:199]} ↦ x-198, {[200:297]} ↦ x-198, {[298:396]} ↦ x-198>>
//Emap = <<{[200:297], [199:199], [298:396]} ↦ 1>>
//
//0, {[100:198], [0:0]}
//1, {[1:99]}
//{[1:99]}, {[100:198], [0:0]}




/// This funcion takes a json object and returns a list of parsed node objects (Node)
/// and its id as key/value.
void createCanonDSBG(const std::string& document) //TODO: Document is a rapidjson class. Need to add the dependency.
{
  //TODO: merge this and create_sb_graph. Handle offsets here. Add necessary class members.
  auto nodes_array = document["nodes"].GetArray(); //TODO: check if "nodes" is even necessary. We can directly read it as an array.

  int current_max = 0;
  SBG::LIB::OrdSet node_set;
  std::map<int, int> node_offsets;

  // Parse each node in the json
  for (const auto& node : nodes_array) {
    SetPiece array_of_nodes;
    // ID ------------
    assert(node.HasMember("id") && node["id"].IsInt());
    unsigned id = node["id"].GetInt();

    // Interval ------
    assert(node.HasMember("interval") and node["interval"].IsArray() and error_msg.c_str());
    auto node_intervals = node["interval"].GetArray();





    //TODO: move this to a function probably.
    // Parse nodes ---
    for (size_t i = 0; i < node_intervals.Size(); i++) {
      auto node_interval = node.intervals[i]; //TODO: check if this has [] operator.
      assert(node_interval.IsArray() && node_interval.Size() == 2);

      int interval_start = int(node_interval[0].GetInt());
      int interval_end = int(node_interval[1].GetInt());

      int shifted_interval_begin, shifted_interval_end; //TODO: check names
      if (i == 0) {
        shifted_interval_begin = current_max;
        shifted_interval_end = (interval_end - interval_start) + current_max;
      } else {
        shifted_interval_begin = interval_start;
        shifted_interval_end = interval_end;
      }
      IntervalDTO interval = IntervalDTO(shifted_interval_begin, 1, shifted_interval_end);

      array_of_nodes.emplaceBack(interval);

      // set this node offset, the difference between the interval and the original one
      node_offsets[id] =  shifted_interval_begin - interval_start;

      // udpate max value
      if (i == 0) {
        current_max = shifted_interval_end + 1;
      }
    }
    node_set.emplaceBack(array_of_nodes);
  }

  //TODO: add node_set as graph V.
  _graph = _graph.addSV(node_set);
  //TODO: do the same with edges.
  //TODO: save credentials


  //TODO: move this to a function probably.
  // Parse edges ---
  SBG::LIB::OrdSet edge_set;  // Our set of edges
  CanonPWMap rhs_maps;  // Map object of one of the sides
  CanonPWMap lhs_maps; // Map object of one of the other side
  for (const auto& node : nodes_array) {
    // ID ------------
    unsigned id = node["id"].GetInt();

    // Interval ------
    auto node_intervals = node["interval"].GetArray();
    // Define the equation intervals (without offsets)
    SBG::LIB::OrdSet current_node_domain; //TODO: check why this needs the fully qualified namespace.
    SetPiece interval_set_piece;
    for (const auto& node_interval : node_intervals) {
      SBG::LIB::Interval interval(node_interval[0].GetInt(), 1, node_interval[1].GetInt());
      interval_set_piece.emplaceBack(interval);
    }
    current_node_domain.emplace(interval_set_piece);


  }






    assert(node.HasMember("lhs"));
    if (node.HasMember("rhs")) {
        //TODO: x = (x + 1) - 3
    } else {
        //TODO: Real u[100]; Check what to do with this
    }
    vector<Var> rhs = read_var_object(node["rhs"]);
    vector<Var> lhs = read_var_object(node["lhs"]);

    Node node_element = Node{int(id), node_weight, std::move(intervals), std::move(rhs), std::move(lhs)};

    nodes.insert({node_element.id, node_element});
  }

  return nodes;
}

// END OF JSON -----------------------------------------------





/// This funcion takes a json object and returns a list of parsed node objects (Node)
/// and its id as key/value.
map<int, Node> create_node_objects_from_json(const Document& document)
{
  auto nodes_array = document["nodes"].GetArray();
  //map<int, Node> nodes;
  int current_max = 0;
  SetDTO node_set;

  for (const auto& node : nodes_array) {

    assert(node.HasMember("id") and node["id"].IsInt());
    unsigned id = node["id"].GetInt();

    std::string error_msg = "Interval " + to_string(id) + " format is wrong";
    assert(node.HasMember("interval") && node["interval"].IsArray() && error_msg.c_str());

    auto node_intervals = node["interval"].GetArray();

    vector<pair<int, int>> intervals;
    intervals.reserve(node_intervals.Size());
    for (const auto& interval : node_intervals) {
      assert(interval.IsArray() and interval.Size() == 2);

      int interval_start = int(interval[0].GetInt());
      int interval_end = int(interval[1].GetInt());

      intervals.push_back(make_pair(interval_start, interval_end));
    }

    vector<Var> rhs = read_var_object(node["rhs"]);
    vector<Var> lhs = read_var_object(node["lhs"]);

    Node node_element = Node{int(id), node_weight, std::move(intervals), std::move(rhs), std::move(lhs)};

    nodes.insert({node_element.id, node_element});
  }

  for (const auto& [i, n]: nodes) {
    logging::sbg_log << n << endl;
  }
  logging::sbg_log << endl;

  return nodes;
}






WeightedSBGraph create_sb_graph(const std::map<int, Node>& nodes)
{
  int max_value = 0;  // We track the max value, so we avoid domain collision between edges and nodes
  map<int, int> node_offsets;

  // Now, we create our set of nodes.
  auto [node_set, weights] = create_set_of_nodes(nodes, node_offsets, max_value);
  logging::sbg_log << "node_set " << node_set << endl;

  // Now, let's build a graph!
  WeightedSBGraph graph; // This will be our graph

  // Firstly, add nodes to the graph.
  graph = addSVW(node_set, weights, graph);

  // Then, create edges and maps.
  auto [edge_set, left_maps, right_maps, costs] = create_graph_edges(nodes, node_offsets, max_value);

  // Now add those edges and maps to the graph
  graph = addSEW(left_maps, right_maps, costs, graph);

  return graph;
}




WeightedSBGraph build_sb_graph(const string& filename)
{
  logging::sbg_log << "Reading " << filename << "..." << endl;

  // Parse json document
  Document document;
  ifstream ifs(filename);
  IStreamWrapper isw(ifs);
  document.ParseStream(isw);

  // Now read the document and convert it into a known type
  auto nodes = create_node_objects_from_json(document);

  // Now, let's get our graph
  auto graph = create_sb_graph(nodes);

  SBG_LOG << graph;

  return graph;
}

































template<typename NodeUserData, typename EdgeUserData>  //TODO: if we just add the Set (OrdSet / UnordSet) type and the function to convert that set (ToOrdSet / ToUnordSet) here, we should be able to support all Set types for the _graph member. See template instantiation at the end sbg.cpp 
void CanonDSBG<NodeUserData, EdgeUserData>::addNode(const SetNode<NodeUserData>& n) {
    SBG::LIB::OrdSet set = SetDTOConverter::convertToOrdSet(n.nodes);
    _graph = _graph.addSV(set);
    _node_user_data_map.insert({set, n.user_data});
}

template<typename NodeUserData, typename EdgeUserData>
void CanonDSBG<NodeUserData, EdgeUserData>::addEdge(const SetEdge<EdgeUserData>& e) { //TODO: Handle shift in each ctor. Check if necessary in addNode.
    SBG::LIB::CanonPWMap pw1 = PWMapDTOConverter::convertToCanonPWMap(e.map_1);
    pw1 = pw1.offsetDom(shift);
    SBG::LIB::CanonPWMap pw2 = PWMapDTOConverter::convertToCanonPWMap(e.map_2);
    _graph = _graph.addSE(pw1, pw2);

    _edge_user_data_map.insert({dom(pw1), e.user_data});
}

template<typename NodeUserData, typename EdgeUserData>
SBG::LIB::OrdSet CanonDSBG<NodeUserData, EdgeUserData>::outputEdges(const SetNode<NodeUserData>& n) {
    SBG::LIB::OrdSet set = SetDTOConverter::convertToOrdSet(n.nodes);
    SBG::LIB::OrdSet output_edges = _graph.mapB().preImage(set);
    return output_edges; //TODO: return SetNode (with a SetDTO inside) instead of Set. Add conversion from actual implementation to DTO in each converter.
}

template<typename NodeUserData, typename EdgeUserData>
SBG::LIB::OrdSet CanonDSBG<NodeUserData, EdgeUserData>::outputSetEdges(const SetNode<NodeUserData>& n) {
    SBG::LIB::OrdSet output_edges = outputEdges(n);
    SBG::LIB::OrdSet output_set_edges = _graph.Emap().image(output_edges);
    return output_set_edges;
}

template<typename NodeUserData, typename EdgeUserData>
SBG::LIB::OrdSet CanonDSBG<NodeUserData, EdgeUserData>::outputNodes(const SetNode<NodeUserData>& n) {
    SBG::LIB::OrdSet output_edges = outputEdges(n);
    SBG::LIB::OrdSet output_vertex = _graph.mapD().image(output_edges); //TODO: we can change SBG::LIB::OrdSet for OrdSet (already aliased).
    return output_vertex;
}

template<typename NodeUserData, typename EdgeUserData>
SBG::LIB::OrdSet CanonDSBG<NodeUserData, EdgeUserData>::outputSetNodes(const SetNode<NodeUserData>& n) {
    SBG::LIB::OrdSet output_vertex = outputNodes(n);
    SBG::LIB::OrdSet output_set_vertex = _graph.Vmap().image(output_vertex);
    return output_set_vertex;
}

//outputnodes (setnode) -> vector SetNode
//outputedges (without last operation of outputnodes).

template<typename NodeUserData, typename EdgeUserData>
std::ostream &operator<<(std::ostream &out, const CanonDSBG<NodeUserData, EdgeUserData> &g)
{
  out << g.graph_;

  return out;
}
//TODO: see build_sb_graph in sbg_partitioner to see how to shift edges and 
//TODO: see advection2D in sbg-partitioner for json example (last entry).
//TODO: save map of node offsets (check build_sb_graph).

} // namespace API

} // namespace SBG
