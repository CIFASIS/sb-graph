/** @file mfvs_fact.hpp

 @brief <b>Minimum Feedback Vertex Set Algorithm Factory</b>

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

#ifndef SBGRAPH_ALGORITHMS_MFVS_MFVS_FACT_HPP_
#define SBGRAPH_ALGORITHMS_MFVS_MFVS_FACT_HPP_

#include "min_feedback_vertex_set.hpp"

namespace SBG {

namespace LIB {

#define MFVS_FACT MFVSFactory::instance()

class GreedyMFVSFact {
public:
  GreedyMFVSFact() = default;

  MinFeedbackVertexSet createMFVSAlgorithm() const;
};

/**
 * @brief Single instance of cv factory to be used by clients in need of
 * creating an instance of a cv algorithm. A client includes this file and
 * calls MFVS_FACT.createMFVSAlgorithm(args).
 */
class MFVSFactory {
public:
  ~MFVSFactory() = default;

  static MFVSFactory& instance();
  const MFVSKind& kind() const;

  void set_mfvs_fact(MFVSKind kind);

  MinFeedbackVertexSet createMFVSAlgorithm() const;

private:
  using MFVSFactImpl = std::variant<GreedyMFVSFact>;

  MFVSFactory();

  MFVSKind _kind;
  MFVSFactImpl _impl;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_ALGORITHMS_MFVS_MFVS_FACT_HPP_
