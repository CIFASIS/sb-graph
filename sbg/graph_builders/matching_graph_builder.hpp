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

#pragma once

#include <sbg/graph_builders/matching_io.hpp>
#include <sbg/sbg.hpp>

namespace SBG {

class MatchingGraphBuilder {
  public:
  MatchingGraphBuilder(IO::Equations equations, IO::Variables variables);
  MatchingGraphBuilder(IO::Equations equations, IO::Variables variables, std::string model_name);
  ~MatchingGraphBuilder() = default;

  virtual SBG::SBGraph build();

  /**
   * @brief Computes the matching information given a set of matched edges.
   *
   * Once the SBG matching is computed, it returns a set of matched edges that corresponds to the
   * SBGraph used to compute the matching. This function computes a vector of @c MatchingInfo structures
   * that maps the obtained matched edges with the @c Equations and @c Variables input vectors of this class.
   *
   * @param matched_edges The set of matched edges obtained by applying the SBGMatching algorithm,
   * from @c MatchingStruct.
   *
   * @return IO::Matching A vector of @c MatchinInfo structures that relates the input @c Equations and @c Variables
   * vectors to the matched set edges.
   */
  virtual IO::Matching computeMatchingInfo(SBG::Set matched_edges);

  protected:
  typedef std::pair<SBG::PWLMap, SBG::PWLMap> MatchingMaps;

  SBG::Set buildSet(IO::VariableInfo variable, int offset, size_t max_dim);
  SBG::Set buildSet(IO::EquationInfo eq, std::string eq_id, int offset, size_t max_dim);
  SBG::Set buildSet(SBG::MultiInterval variable);
  SBG::SetVertexDesc addVertex(std::string vertex_name, SBG::Set set, int vec_id);
  void addEquation(IO::EquationInfo eq, std::string id, SBG::Set set, int vec_id);
  SBG::PWLMap buildPWLMap(SBG::ORD_REALS constants, SBG::ORD_REALS slopes, SBG::Set dom);
  MatchingMaps generatePWLMaps(IO::VariableUsage var_usage, SBG::Set dom, SBG::Set unk_dom, int offset, std::string eq_id, size_t max_dim);
  SBG::Set generateMapDom(SBG::Set dom, SBG::Set unk_dom, int offset, size_t max_dim);
  void addDims(size_t max_dim, size_t exp_dim, SBG::MultiInterval& intervals, int offset);
  void addDims(size_t max_dim, size_t exp_dim, SBG::ORD_REALS& constant, SBG::ORD_REALS& slope);
  std::vector<IO::RangeDef> generateRange(SBG::Set dom, int offset);

  private:
  typedef std::pair<SBG::SetVertexDesc, IO::EquationInfo> EquationDesc;
  typedef std::map<std::string, int> Usage;
  typedef std::map<std::string, Usage> EqUsage;
  typedef std::map<std::string, SBG::SetVertexDesc> VarUsage;

  VarUsage _U;
  std::list<EquationDesc> _F;
  EqUsage _eq_usage;
  IO::Equations _equations;
  IO::Variables _variables;
  std::string _model_name;
  SBG::SBGraph _graph;
  IO::NodeMap _node_map;
  std::map<std::string, int> _eq_offset_map;
  std::map<std::string, int> _var_offset_map;
};

}  // namespace SBG
