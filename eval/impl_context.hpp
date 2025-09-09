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

namespace SBG {

namespace Eval {

using SetFactPtr = std::unique_ptr<LIB::SetAF>;
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
  LIB::SetAF& setFact() const;
  LIB::PWMapAF& pwFact() const;
  LIB::MatchingFact& matchFact() const;
  LIB::SCCFact& sccFact() const;
  LIB::TSFact& tsFact() const;
  LIB::CVFact& cvFact() const;
  // Setters
  void setSetFact(SetFactPtr fact);
  void setPwFact(PWMapFactPtr fact); 
  void setMatchFact(MatchFactPtr fact); 
  void setSccFact(SCCFactPtr fact); 
  void setTSFact(TSFactPtr fact); 
  void setCVFact(CVFactPtr fact);

  private:
  SetFactPtr set_fact_;
  PWMapFactPtr pw_fact_;
  MatchFactPtr match_fact_;
  SCCFactPtr scc_fact_;
  TSFactPtr ts_fact_;
  CVFactPtr cv_fact_; 
};

} // namespace Eval

} // namespace SBG

#endif
