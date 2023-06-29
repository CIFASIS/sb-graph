/** @file skipper.hpp

 @brief <b>Parser skipper definitions</b>

 The current skipper allows for C multi-line and C++ single line comments
 plus spaces.

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

#ifndef SKIPPER_HPP
#define SKIPPER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/repository/include/qi_kwd.hpp>

namespace SBG {

namespace Parser {

namespace asc = boost::spirit::ascii;
namespace phx = boost::phoenix;
namespace qi = boost::spirit::qi;

typedef std::string::const_iterator StrIt;

template <typename Iterator>
struct Skipper : qi::grammar<Iterator> {
  Skipper();

  qi::rule<Iterator> single_line_comment;
  qi::rule<Iterator> block_comment;
  qi::rule<Iterator> start;
};

} // namespace Parser

} // namespace SBG

#endif
