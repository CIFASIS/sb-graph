/*****************************************************************************

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

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>

#include <iostream>
#include <string>
#include <complex>

#include <sbg/interval.hpp>
#include <sbg/multi_interval.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Our complex number parser/compiler
///////////////////////////////////////////////////////////////////////////////

// Adapt structures -------------------------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(SBG::Interval, (SBG::INT, lo_)(SBG::INT, step_)(SBG::INT, hi_)(bool, empty_))

BOOST_FUSION_ADAPT_STRUCT(SBG::MultiInterval, (SBG::MultiInterval::Intervals, inters_)(int, ndim_))

// Grammar ----------------------------------------------------------------------------------------

namespace client
{
  using boost::spirit::qi::bool_;
  using boost::spirit::qi::int_;
  using boost::spirit::qi::_1;
  using boost::spirit::qi::phrase_parse;
  using boost::spirit::ascii::space;
  using boost::phoenix::ref;

  template <typename Iterator>
  struct sbg_parser : boost::spirit::qi::grammar<Iterator, SBG::Interval(), boost::spirit::ascii::space_type> {
    sbg_parser() : sbg_parser::base_type(multi_inter) { 
      inter = '[' 
        >> int_
        >> ':' >> int_ 
        >> ':' >> int_ 
        >> ']';

      multi_inter = inter 
        >> *('x' >> inter);
    }

    boost::spirit::qi::rule<Iterator, SBG::Interval(), boost::spirit::ascii::space_type> inter;
    boost::spirit::qi::rule<Iterator, SBG::MultiInterval(), boost::spirit::ascii::space_type> multi_inter;
  };
}

////////////////////////////////////////////////////////////////////////////
//  Main program
////////////////////////////////////////////////////////////////////////////

int main() {
  std::cout << "/////////////////////////////////////////////////////////\n\n";
  std::cout << "A simple SBG parser\n\n";
  std::cout << "/////////////////////////////////////////////////////////\n\n";

  std::cout << "Give me a complex number of the form r or (r) or (r,i) \n";
  std::cout << "Type [q or Q] to quit\n\n";

  typedef std::string::const_iterator iterator_type;
  typedef client::sbg_parser<iterator_type> sbg_parser;

  sbg_parser g; // Our grammar

  std::string str;
  SBG::Interval result;
  while (getline(std::cin, str))
  {
    if (str.empty() || str[0] == 'q' || str[0] == 'Q')
      break;

    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = phrase_parse(iter, end, g, boost::spirit::ascii::space, result);

    if (r && iter == end) {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "result = " << result << std::endl;
        std::cout << "-------------------------\n";
    }

    else {
        std::string rest(iter, end);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \": " << rest << "\"\n";
        std::cout << "-------------------------\n";
    }
  }

  std::cout << "Bye... :-) \n\n";
  return 0;
}
