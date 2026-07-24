/** @file esbg.hpp

 @brief <b>Extended Set-Based Graph (ESBG)</b>

 An ESBG wraps a directed SBG (DSBG) and keeps, in parallel, user-defined data
 attached to each set-vertex and each set-edge. The user data is indexed by
 the identifier (a MD_NAT) that the underlying DSBG assigns to every new
 set-vertex / set-edge through its Vmap_ / Emap_ piecewise maps.

 The underlying DSBG is mutated in place (assigned to itself after every
 addSV / addSE call) to avoid copying user-data maps on each insertion.

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

#ifndef SBG_STRUCT_ESBG_HPP
#define SBG_STRUCT_ESBG_HPP

#include <map>
#include <optional>
#include <utility>

#include "sbg/directed_sbg.hpp"
#include "sbg/set_fact.hpp"

namespace SBG {

namespace Struct {

////////////////////////////////////////////////////////////////////////////////
// Extended Set-Based Graph ----------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Directed SBG augmented with per-set-vertex and per-set-edge user data.
 *
 * @tparam NodeUserData Type of user data attached to each set-vertex.
 * @tparam EdgeUserData Type of user data attached to each set-edge.
 */
template <typename NodeUserData, typename EdgeUserData>
class ESBG {
  public:
  using NodeUserDataMap = std::map<SBG::LIB::MD_NAT, NodeUserData>;
  using EdgeUserDataMap = std::map<SBG::LIB::MD_NAT, EdgeUserData>;

  ESBG() = default;

  explicit ESBG(SBG::LIB::DSBG graph) : graph_(std::move(graph)) {}

  // Accessors -----------------------------------------------------------------

  const SBG::LIB::DSBG& graph() const { return graph_; }
  SBG::LIB::DSBG& graph() { return graph_; }

  const NodeUserDataMap& nodeUserDataMap() const { return node_user_data_map_; }
  const EdgeUserDataMap& edgeUserDataMap() const { return edge_user_data_map_; }

  /**
   * @brief Returns the user data associated with the set-vertex identified by
   * \p id, or std::nullopt if no data is registered for that id.
   */
  std::optional<NodeUserData> nodeUserData(const SBG::LIB::MD_NAT& id) const
  {
    auto it = node_user_data_map_.find(id);
    if (it == node_user_data_map_.end()) return std::nullopt;
    return it->second;
  }

  std::optional<EdgeUserData> edgeUserData(const SBG::LIB::MD_NAT& id) const
  {
    auto it = edge_user_data_map_.find(id);
    if (it == edge_user_data_map_.end()) return std::nullopt;
    return it->second;
  }

  // Mutators ------------------------------------------------------------------

  /**
   * @brief Adds a new set-vertex composed by \p vertices, registering
   * \p user_data under the identifier assigned by the underlying DSBG.
   * @return Reference to *this, allowing chained calls.
   *
   * Precondition: graph_.V().intersection(\p vertices) = {}.
   */
  ESBG& addSV(const SBG::LIB::Set& vertices, NodeUserData user_data)
  {
    graph_ = graph_.addSV(vertices);
    SBG::LIB::MD_NAT id = graph_.Vmap().image(vertices).maxElem();
    node_user_data_map_.emplace(std::move(id), std::move(user_data));
    return *this;
  }

  /**
   * @brief Adds a new set-edge described by \p pw1 (begin) and \p pw2 (end),
   * registering \p user_data under the identifier assigned by the underlying
   * DSBG.
   * @return Reference to *this, allowing chained calls.
   *
   * Precondition: dom(\p pw1) = dom(\p pw2) and disjoint with graph_.E().
   */
  ESBG& addSE(const SBG::LIB::PWMap& pw1, const SBG::LIB::PWMap& pw2,
              EdgeUserData user_data)
  {
    SBG::LIB::Set new_edges = pw1.dom();
    graph_ = graph_.addSE(pw1, pw2);
    SBG::LIB::MD_NAT id = graph_.Emap().image(new_edges).maxElem();
    edge_user_data_map_.emplace(std::move(id), std::move(user_data));
    return *this;
  }

  /**
   * @brief Erase vertices \p vs and all incident edges from the graph,
   * dropping their associated user data entries.
   */
  ESBG& eraseVertices(const SBG::LIB::Set& vs)
  {
    graph_ = graph_.eraseVertices(vs);

    SBG::LIB::Set surviving_v_ids = graph_.Vmap().image();
    SBG::LIB::Set surviving_e_ids = graph_.Emap().image();

    eraseStaleEntries(node_user_data_map_, surviving_v_ids);
    eraseStaleEntries(edge_user_data_map_, surviving_e_ids);
    return *this;
  }

  protected:
  SBG::LIB::DSBG graph_;
  NodeUserDataMap node_user_data_map_;
  EdgeUserDataMap edge_user_data_map_;

  private:
  template <typename Map>
  static void eraseStaleEntries(Map& m, const SBG::LIB::Set& surviving_ids)
  {
    auto& fact = SBG::LIB::SetFactory::instance().set_fact();
    for (auto it = m.begin(); it != m.end();) {
      SBG::LIB::Set single = fact.createSet(it->first);
      if (single.intersection(surviving_ids).isEmpty())
        it = m.erase(it);
      else
        ++it;
    }
  }
};

} // namespace Struct

} // namespace SBG

#endif
