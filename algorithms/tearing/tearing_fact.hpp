/** @file tearing_fact.hpp

 @brief <b>Tearing Algorithm Factory</b>

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

#ifndef SBG_TEARING_FACT_HPP
#define SBG_TEARING_FACT_HPP

#include "tearing.hpp"

namespace SBG {

namespace LIB {

#define TEARING_FACT TearingFactory::instance().tearing_fact()

class TearingFact {
  public:
  virtual ~TearingFact() = default;
  TearingFact() = default;

  virtual Tearing createTearingAlgorithm() const = 0;
  virtual std::string prettyPrint() const = 0;
};

class TearingV1Fact : public TearingFact {
  public:
  TearingV1Fact() = default;

  Tearing createTearingAlgorithm() const override;
  std::string prettyPrint() const override;
};

using TearingFactPtr = std::unique_ptr<TearingFact>;

/**
 * @brief Single instance of tearing factory to be used by clients in need of
 * creating an instance of a tearing algorithm. A client includes this file and
 * calls TEARING_FACT.createTearingAlgorithm(args).
 */
class TearingFactory {
  public:
  ~TearingFactory() = default;

  static TearingFactory& instance() {
    static TearingFactory instance_;
    return instance_;
  }
  TearingFact& tearing_fact();
  void set_tearing_fact(TearingFactPtr tearing_fact);

  private:
  TearingFactory();

  TearingFactPtr tearing_fact_;
};

} // namespace LIB

}  // namespace SBG

#endif
