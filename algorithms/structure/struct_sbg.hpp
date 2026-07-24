/** @file struct_sbg.hpp

 @brief <b>Structure SBG</b>

 Specialisation of ESBG for structural / dependency analysis. On top of the
 user data attached to each set-vertex by the underlying ESBG, the StructSBG
 keeps a per-set-vertex StructNodeData entry describing the role of the
 node within the structure graph and a transient \p visited flag used by
 traversal algorithms.

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

#ifndef SBG_STRUCT_STRUCT_SBG_HPP
#define SBG_STRUCT_STRUCT_SBG_HPP

#include "algorithms/structure/esbg.hpp"

namespace SBG {

namespace Struct {

////////////////////////////////////////////////////////////////////////////////
// Structure SBG ---------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

enum class NodeType {
  Equation,
  Influencer,
  Algebraic,
  Influencee,
  Statement,
  Derivative
};

struct StructNodeData {
  NodeType type;
  bool visited = false;
};

template <typename NodeUserData, typename EdgeUserData>
class StructSBG : public ESBG<NodeUserData, EdgeUserData> {
  public:
  using Base = ESBG<NodeUserData, EdgeUserData>;
  using StructNodeDataMap = std::map<SBG::LIB::MD_NAT, StructNodeData>;

  StructSBG() = default;

  explicit StructSBG(SBG::LIB::DSBG graph) : Base(std::move(graph)) {}

  const StructNodeDataMap& structNodeDataMap() const
  {
    return struct_node_data_map_;
  }

  std::optional<StructNodeData>
  structNodeData(const SBG::LIB::MD_NAT& id) const
  {
    auto it = struct_node_data_map_.find(id);
    if (it == struct_node_data_map_.end()) return std::nullopt;
    return it->second;
  }

  /**
   * @brief Adds a new set-vertex with both its user data and its structural
   * metadata.
   */
  StructSBG& addSV(const SBG::LIB::Set& vertices, NodeUserData user_data,
                   StructNodeData struct_data)
  {
    Base::addSV(vertices, std::move(user_data));
    SBG::LIB::MD_NAT id = this->graph_.Vmap().image(vertices).maxElem();
    struct_node_data_map_.emplace(std::move(id), std::move(struct_data));
    return *this;
  }

  using Base::addSV;
  using Base::addSE;

  StructSBG& eraseVertices(const SBG::LIB::Set& vs)
  {
    Base::eraseVertices(vs);

    SBG::LIB::Set surviving_v_ids = this->graph_.Vmap().image();
    auto& fact = SBG::LIB::SetFactory::instance().set_fact();
    for (auto it = struct_node_data_map_.begin();
         it != struct_node_data_map_.end();) {
      SBG::LIB::Set single = fact.createSet(it->first);
      if (single.intersection(surviving_v_ids).isEmpty())
        it = struct_node_data_map_.erase(it);
      else
        ++it;
    }
    return *this;
  }

  void markVisited(const SBG::LIB::MD_NAT& id, bool value = true)
  {
    auto it = struct_node_data_map_.find(id);
    if (it != struct_node_data_map_.end()) it->second.visited = value;
  }

  void resetVisited()
  {
    for (auto& kv : struct_node_data_map_) kv.second.visited = false;
  }

  protected:
  StructNodeDataMap struct_node_data_map_;
};

} // namespace Struct

} // namespace SBG

#endif
