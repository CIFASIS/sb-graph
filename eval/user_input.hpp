/** @file user_input.hpp

 @brief <b>User input definitions</b>
 
 This module defines structures to represent the user input for the desired
 implementations of sets, pws and algorithms.

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

#ifndef SBGRAPH_EVAL_USER_INPUT_HPP_
#define SBGRAPH_EVAL_USER_INPUT_HPP_

#include "boost/optional.hpp"

namespace SBG {

namespace Eval {

namespace detail {

class EvalUserInput {
public:
  using MaybeInt = boost::optional<int>;

  EvalUserInput();

  // Getters
  MaybeInt set_impl() const;
  MaybeInt pw_impl() const;
  MaybeInt match_impl() const;
  MaybeInt scc_impl() const;
  MaybeInt ts_impl() const;
  MaybeInt cv_impl() const;
  // Setters
  void set_set_impl(MaybeInt set_impl);
  void set_pw_impl(MaybeInt pw_impl);
  void set_match_impl(MaybeInt match_impl);
  void set_scc_impl(MaybeInt scc_impl);
  void set_ts_impl(MaybeInt ts_impl);
  void set_cv_impl(MaybeInt cv_impl);

private:
  MaybeInt set_impl_;
  MaybeInt pw_impl_;
  MaybeInt match_impl_;
  MaybeInt scc_impl_;
  MaybeInt ts_impl_;
  MaybeInt cv_impl_;
};

} // detail

} // namespace Eval

} // namespace SBG

#endif // SBGRAPH_EVAL_USER_INPUT_HPP_
