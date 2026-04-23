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

#include "algorithms/mfvs/mfvs_fact.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace LIB {

////////////////////////////////////////////////////////////////////////////////
// Maximum Degree Cut Vertex Factory -------------------------------------------
////////////////////////////////////////////////////////////////////////////////

MinFeedbackVertexSet GreedyMFVSFact::createMFVSAlgorithm() const
{
  return MinFeedbackVertexSet{MFVSKind::kGreedy};
}

////////////////////////////////////////////////////////////////////////////////
// Factory for clients --------------------------------------------------------- 
////////////////////////////////////////////////////////////////////////////////

MFVSFactory::MFVSFactory()
  : _kind(MFVSKind::kGreedy), _impl(GreedyMFVSFact{}) {}

MFVSFactory& MFVSFactory::instance()
{
  static MFVSFactory _instance;
  return _instance;
}

const MFVSKind& MFVSFactory::kind() const { return _kind; }

void MFVSFactory::set_mfvs_fact(MFVSKind kind)
{
  _kind = kind;
  switch (kind) {
    case MFVSKind::kGreedy: {
      _impl = GreedyMFVSFact{};
    }

    default: {
      Util::ERROR("MFVSFactory::set_mfvs_fact: unsupported MFVS ", kind
        , " implementation");
      break;
    }
  }
}

MinFeedbackVertexSet MFVSFactory::createMFVSAlgorithm() const
{
  return std::visit([](const auto& a) { return a.createMFVSAlgorithm(); }
    , _impl);
}

} // namespace LIB

} // namespace SBG
