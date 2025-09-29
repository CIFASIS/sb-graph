/** @file impl_context.hpp

 @brief <b>Implementations context</b>

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

#ifndef EVAL_IMPL_CONTEXT_HPP
#define EVAL_IMPL_CONTEXT_HPP

#include "algorithms/cutvertex/cv_fact.hpp"
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/ts_fact.hpp"
#include "sbg/af_set.hpp"

namespace SBG {

namespace Eval {

using PWMapFactPtr = std::unique_ptr<LIB::PWMapAF>;
using MatchFactPtr = std::unique_ptr<LIB::MatchingFact>;
using SCCFactPtr = std::unique_ptr<LIB::SCCFact>;
using TSFactPtr = std::unique_ptr<LIB::TSFact>;
using CVFactPtr = std::unique_ptr<LIB::CVFact>;

/** 
 * @brief Class used to keep track of choosen implementations by the user.
 */
class ImplContext {
  public:
  ImplContext();

  // Getters
  LIB::SetAF& set_fact() const;
  LIB::PWMapAF& pw_fact() const;
  LIB::MatchingFact& match_fact() const;
  LIB::SCCFact& scc_fact() const;
  LIB::TSFact& ts_fact() const;
  LIB::CVFact& cv_fact() const;
  // Setters
  void set_set_fact(LIB::SetFactPtr fact);
  void set_pw_fact(PWMapFactPtr fact); 
  void set_match_fact(MatchFactPtr fact); 
  void set_scc_fact(SCCFactPtr fact); 
  void set_ts_fact(TSFactPtr fact); 
  void set_cv_fact(CVFactPtr fact);

  private:
  LIB::SetFactPtr set_fact_;
  PWMapFactPtr pw_fact_;
  MatchFactPtr match_fact_;
  SCCFactPtr scc_fact_;
  TSFactPtr ts_fact_;
  CVFactPtr cv_fact_; 
};

} // namespace Eval

} // namespace SBG

#endif
