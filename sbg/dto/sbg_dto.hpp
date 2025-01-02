/** @file sbg_dto.hpp

 @brief <b>Set-based graph DTO implementation</b>

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

#ifndef SBG_DTO_SBG_HPP //TODO: change this and/or file name.
#define SBG_DTO_SBG_HPP

#include <sbg/dto/pw_map_dto.hpp>
#include <sbg/dto/pw_mdinter_dto.hpp>
#include <sbg/sbg.hpp>

#include <map>

namespace SBG {
namespace API {

template<typename UserData>
struct SetNode {
  SetDTO nodes;
  UserData user_data;
};

template<typename UserData>
struct SetEdge {
  PWMapDTO map_1;
  PWMapDTO map_2;
  UserData user_data;
};

using SetPiece = SBG::LIB::SetPiece //TODO: check if we leave this here.
using OrdSet = SBG::LIB::OrdSet;
using CanonPWMap = SBG::LIB::CanonPWMap;

template<typename NodeUserData, typename EdgeUserData>
class CanonDSBG {
public:
  using NodeUserDataMap = std::map<OrdSet, NodeUserData>;
  using EdgeUserDataMap = std::map<std::pair<CanonPWMap, CanonPWMap>, EdgeUserData>;

  CanonDSBG();
  void createCanonDSBG(const std::string& document);

  // Node operations
  void addNode(const SetNode<NodeUserData>& node);
  OrdSet outputNodes(const SetNode<NodeUserData>& node);
  OrdSet outputSetNodes(const SetNode<NodeUserData>& node);

  // Edge operations
  void addEdge(const SetEdge<EdgeUserData>& edge);
  OrdSet outputEdges(const SetNode<NodeUserData>& node);
  OrdSet outputSetEdges(const SetNode<NodeUserData>& node);

  // Graph operations
  bool isEmpty() const;

protected:
  NodeUserDataMap _node_user_data_map;
  EdgeUserDataMap _edge_user_data_map;
  SBG::LIB::CanonDSBG _graph;
  friend std::ostream &operator<<(std::ostream &out, const CanonDSBG<NodeUserData, EdgeUserData> &graph);
};

} // namespace API
} // namespace SBG