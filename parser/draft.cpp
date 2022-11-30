/*=============================================================================
    Copyright (c) 2002-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A complex number micro parser.
//
//  [ JDG May 10, 2002 ]    spirit1
//  [ JDG May 9, 2007 ]     spirit2
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>

#include <iostream>
#include <string>
#include <complex>

#include <sbg/interval.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Our complex number parser/compiler
///////////////////////////////////////////////////////////////////////////////

namespace client
{
  template <typename Iterator>
  bool parse_interval(Iterator first, Iterator last, SBG::Interval& i) {
    using boost::spirit::qi::int_;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::ascii::space;
    using boost::phoenix::ref;

    SBG::INT lo = 0, st = 0, hi = 0;
    bool b = phrase_parse(first, last,

    //  Begin grammar
    (
      '[' 
       >> int_[ref(lo) = _1]
       >> ':' >> int_[ref(st) = _1] 
       >> ':' >> int_[ref(hi) = _1] 
       >> ']'
    ),
    //  End grammar

    space);

    if (!b || first != last) // fail if we did not get a full match
      return false;

    i = SBG::Interval(lo, st, hi);
    return b;
  }
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

  std::string str;
  while (getline(std::cin, str))
  {
    if (str.empty() || str[0] == 'q' || str[0] == 'Q')
      break;

    SBG::Interval i;
    if (client::parse_interval(str.begin(), str.end(), i)) {
      std::cout << "-------------------------\n";
      std::cout << "Parsing succeeded\n";
      std::cout << "got: " << i << std::endl;
      std::cout << "\n-------------------------\n";
    }

    else {
      std::cout << "-------------------------\n";
      std::cout << "Parsing failed\n";
      std::cout << "-------------------------\n";
    }
  }

  std::cout << "Bye... :-) \n\n";
  return 0;
}
