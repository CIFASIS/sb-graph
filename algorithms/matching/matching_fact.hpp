/** @file matching_fact.hpp

 @brief <b>Matching Algorithm Factory</b>

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

#ifndef SBG_MATCHING_FACT_HPP
#define SBG_MATCHING_FACT_HPP

#include "algorithms/matching/matching.hpp"

namespace SBG {

namespace LIB {

#define MATCH_FACT MatchFactory::instance().match_fact()

class MatchingFact {
  public:
  virtual ~MatchingFact() = default;
  MatchingFact() = default;

  virtual Matching createMatchAlgorithm() const = 0;
};

class BFSMatchingFact : public MatchingFact {
  public:
  BFSMatchingFact() = default;

  Matching createMatchAlgorithm() const override;
};

using MatchFactPtr = std::unique_ptr<MatchingFact>;

/**
 * @brief Single instance of match factory to be used by clients in need of
 * creating an instance of a matching algorithm. A client includes this file and
 * calls MATCH_FACT.createMatchAlgorithm(args).
 */
class MatchFactory {
  public:
  ~MatchFactory() = default;

  static MatchFactory& instance() {
    static MatchFactory instance_;
    return instance_;
  }
  MatchingFact& match_fact();
  void set_match_fact(MatchFactPtr match_fact);

  private:
  MatchFactory();

  MatchFactPtr match_fact_;
};

} // namespace LIB

}  // namespace SBG

#endif
