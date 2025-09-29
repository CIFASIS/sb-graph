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

#include "algorithms/cutvertex/cv_fact.hpp"
#include "algorithms/matching/matching_fact.hpp"
#include "algorithms/scc/scc_fact.hpp"
#include "algorithms/toposort/ts_fact.hpp"
#include "eval/impl_context.hpp"
#include "util/debug.hpp"

namespace SBG {

namespace Eval {

ImplContext::ImplContext() : set_fact_(), pw_fact_(), match_fact_(), scc_fact_()
  , ts_fact_(), cv_fact_() {}

// Getters ---------------------------------------------------------------------

LIB::SetAF& ImplContext::set_fact() const
{
  Util::ERROR_UNLESS(set_fact_ != nullptr
    , "ImplContext: set_fact_ has not been set\n");
  return *set_fact_;
}

LIB::PWMapAF& ImplContext::pw_fact() const
{
  Util::ERROR_UNLESS(pw_fact_ != nullptr
    , "ImplContext: pw_fact_ has not been set\n");
  return *pw_fact_;
}

LIB::MatchingFact& ImplContext::match_fact() const
{
  Util::ERROR_UNLESS(match_fact_ != nullptr
    , "ImplContext: match_fact_ has not been set\n");
  return *match_fact_;
}

LIB::SCCFact& ImplContext::scc_fact() const
{
  Util::ERROR_UNLESS(scc_fact_ != nullptr
    , "ImplContext: scc_fact_ has not been set\n");
  return *scc_fact_;
}

LIB::TSFact& ImplContext::ts_fact() const
{
  Util::ERROR_UNLESS(ts_fact_ != nullptr
    , "ImplContext: ts_fact_ has not been set\n");
  return *ts_fact_;
}

LIB::CVFact& ImplContext::cv_fact() const
{
  Util::ERROR_UNLESS(cv_fact_ != nullptr
    , "ImplContext: cv_fact_ has not been set\n");
  return *cv_fact_;
}

// Setters ---------------------------------------------------------------------

void ImplContext::set_set_fact(LIB::SetFactPtr fact)
{
  set_fact_ = std::move(fact);
}

void ImplContext::set_pw_fact(PWMapFactPtr fact)
{
  pw_fact_ = std::move(fact);
}

void ImplContext::set_match_fact(MatchFactPtr fact)
{
  match_fact_ = std::move(fact);
}

void ImplContext::set_scc_fact(SCCFactPtr fact)
{
  scc_fact_ = std::move(fact);
}

void ImplContext::set_ts_fact(TSFactPtr fact)
{
  ts_fact_ = std::move(fact);
}

void ImplContext::set_cv_fact(CVFactPtr fact)
{
  cv_fact_ = std::move(fact);
}

} // namespace Eval

} // namespace SBG
