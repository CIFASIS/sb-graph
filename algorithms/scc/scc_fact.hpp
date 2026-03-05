/** @file scc_fact.hpp

 @brief <b>SCC Algorithm Factory</b>

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

#ifndef SBGRAPH_ALGORITHMS_SCC_SCC_FACT_HPP_
#define SBGRAPH_ALGORITHMS_SBG_SCC_FACT_HPP_

#include "algorithms/scc/scc.hpp"

#include <string>

namespace SBG {

namespace LIB {

#define SCC_FACT SCCFactory::instance()

class MinReachSCCV1Fact {
public:
  MinReachSCCV1Fact() = default;

  SCC createSCCAlgorithm() const;
};

class MinReachSCCV2Fact {
public:
  MinReachSCCV2Fact() = default;

  SCC createSCCAlgorithm() const;
};

/**
 * @brief Single instance of scc factory to be used by clients in need of
 * creating an instance of a scc algorithm. A client includes this file and
 * calls SCC_FACT.createSCCAlgorithm(args).
 */
class SCCFactory {
public:
  ~SCCFactory() = default;

  static SCCFactory& instance();
  const SCCKind& kind() const;

  void set_scc_fact(SCCKind kind);

  SCC createSCCAlgorithm() const;

private:
  using SCCFactImpl = std::variant<MinReachSCCV1Fact, MinReachSCCV2Fact>;

  SCCFactory();

  SCCKind _kind;
  SCCFactImpl _impl;
};

} // namespace LIB

}  // namespace SBG

#endif // SBGRAPH_ALGORITHMS_SCC_SCC_FACT_HPP_
