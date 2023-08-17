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

#include "parser/skipper.hpp"

namespace SBG {

namespace Parser {

template <typename Iterator>
Skipper<Iterator>::Skipper() : Skipper::base_type(start) {
  single_line_comment = "//" >> *(qi::char_ - qi::eol) >> (qi::eol | qi::eoi);
  block_comment = "/*" >> *(block_comment | qi::char_ - "*/") > "*/";

  start = asc::space | single_line_comment | block_comment;
};

template struct Skipper<StrIt>;

} // namespace Parser

} // namespace SBG
