/** @file ts_fact.hpp

 @brief <b>Topological Sort Algorithm Factory</b>

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

#ifndef SBG_TS_FACT_HPP
#define SBG_TS_FACT_HPP

#include "topo_sort.hpp"

namespace SBG {

namespace LIB {

#define TS_FACT TSFactory::instance().ts_fact()

class TSFact {
  public:
  virtual ~TSFact() = default;
  TSFact() = default;

  virtual TopoSort createTSAlgorithm() const = 0;
};

class MinVertexTSFact : public TSFact {
  public:
  MinVertexTSFact() = default;

  TopoSort createTSAlgorithm() const override;
};

using TSFactPtr = std::unique_ptr<TSFact>;

/**
 * @brief Single instance of ts factory to be used by clients in need of
 * creating an instance of a ts algorithm. A client includes this file and
 * calls TS_FACT.createTSAlgorithm(args).
 */
class TSFactory {
  public:
  ~TSFactory() = default;

  static TSFactory& instance() {
    static TSFactory instance_;
    return instance_;
  }
  TSFact& ts_fact();
  void set_ts_fact(TSFactPtr ts_fact);

  private:
  TSFactory();

  TSFactPtr ts_fact_;
};


} // namespace LIB

}  // namespace SBG

#endif
