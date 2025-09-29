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

#ifndef SBG_SCC_FACT_HPP
#define SBG_SCC_FACT_HPP

#include "scc.hpp"

namespace SBG {

namespace LIB {

#define SCC_FACT SCCFactory::instance().getSCCFactory()

class SCCFact {
  public:
  virtual ~SCCFact() = default;
  SCCFact() = default;

  virtual SCC createSCCAlgorithm() const = 0;
};

class MinReachSCCFact : public SCCFact {
  public:
  MinReachSCCFact() = default;

  SCC createSCCAlgorithm() const override;
};

/**
 * @brief Single instance of scc factory to be used by clients in need of
 * creating an instance of a scc algorithm. A client includes this file and
 * calls SCC_FACT.createSCCAlgorithm(args).
 */
class SCCFactory {
  public:
  ~SCCFactory() = default;

  static SCCFactory& instance() {
    static SCCFactory instance_;
    return instance_;
  }
  SCCFact& getSCCFactory();
  void setSCCFactory(std::unique_ptr<SCCFact> scc_fact);

  private:
  SCCFactory();

  std::unique_ptr<SCCFact> scc_fact_;
};

} // namespace LIB

}  // namespace SBG

#endif
