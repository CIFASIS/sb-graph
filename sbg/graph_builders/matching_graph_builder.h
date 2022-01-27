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

#include <sbg/sbg.h>

namespace SBG {

struct LinearMap {
  int constant;
  int slope;
  std::string iterator;
  bool operator<(const LinearMap &other) const {
    if (constant == other.constant) {
      return slope < other.slope;
    } 
    return constant < other.constant;
  }

};

typedef std::vector<LinearMap> LinearMaps;

struct VariableUsage {
  std::string name;
  LinearMaps usage;
  bool operator<(const VariableUsage &other) const {
    if (name == other.name) {
      for (unsigned int dim = 0; dim < usage.size(); dim++) {
        if (usage[dim] < other.usage[dim])
          return true;
      }
      return false;
    }
    return name < other.name;  
  }
};

struct RangeDef {
  std::string iterator;
  int begin;
  int step;
  int end;
};

struct EquationInfo {
  std::vector<RangeDef> size;
  std::set<VariableUsage> var_usage;
};

struct VariableInfo {
  std::string name;
  std::vector<int> size;
  bool is_state;
};

typedef std::list<EquationInfo> Equations;
typedef std::list<VariableInfo> Variables;

class MatchingGraphBuilder {
  public:
  MatchingGraphBuilder(Equations equations, Variables variables);
  MatchingGraphBuilder(Equations equations, Variables variables, std::string model_name);
  ~MatchingGraphBuilder() = default;

  virtual SBG::SBGraph build();

  protected:
  typedef std::pair<SBG::PWLMap, SBG::PWLMap> MatchingMaps;

  SBG::Set buildSet(VariableInfo variable, int offset, size_t max_dim);
  SBG::Set buildSet(EquationInfo eq, std::string eq_id, int offset, size_t max_dim);
  SBG::Set buildSet(SBG::MultiInterval variable);
  SBG::SetVertexDesc addVertex(std::string vertex_name, SBG::Set set, SBG::SBGraph& graph);
  void addEquation(EquationInfo eq, std::string id, SBG::Set set, SBG::SBGraph& graph);
  SBG::PWLMap buildPWLMap(SBG::ORD_REALS constants, SBG::ORD_REALS slopes, SBG::Set dom);
  MatchingMaps generatePWLMaps(VariableUsage var_usage, SBG::Set dom, SBG::Set unk_dom, int offset, std::string eq_id, size_t max_dim);
  SBG::Set generateMapDom(SBG::Set dom, SBG::Set unk_dom, int offset, size_t max_dim);
  void addDims(size_t max_dim, size_t exp_dim, SBG::MultiInterval& intervals, int offset);
  void addDims(size_t max_dim, size_t exp_dim, SBG::ORD_REALS& constant, SBG::ORD_REALS& slope);
  
  private:
  typedef std::pair<SBG::SetVertexDesc, EquationInfo> EquationDesc; 
  typedef std::map<std::string, int> Usage;
  typedef std::map<std::string, Usage> EqUsage;
  typedef std::map<std::string, SBG::SetVertexDesc> VarUsage;

  VarUsage _U;
  std::list<EquationDesc> _F;
  EqUsage _eq_usage;
  Equations _equations;
  Variables _variables;
  std::string _model_name;
};

}  // namespace SBG
