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

/*!
* This is a really tiny parser for SBGs. It was developed as a test tool.
* Currently, only a filename can be provided, input can't be provided
* from the command line.
*/

#include <boost/bind/bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <fstream>
#include <iostream>
#include <string>

#include <sbg/descs.hpp>
#include <sbg/interval.hpp>
#include <sbg/multi_interval.hpp>
#include <sbg/set.hpp>
#include <sbg/pw_map.hpp>
#include <sbg/sbg.hpp>

// Adapt structures -------------------------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(SBG::Interval, (SBG::INT, lo_)(SBG::INT, step_)(SBG::INT, hi_)(bool, empty_))

BOOST_FUSION_ADAPT_STRUCT(SBG::MultiInterval, (SBG::MultiInterval::Intervals, inters_)(int, ndim_))

BOOST_FUSION_ADAPT_STRUCT(SBG::Set, (SBG::Set::AtomSets, asets_)(int, ndim_))

BOOST_FUSION_ADAPT_STRUCT(SBG::SVDesc, (std::string, text_))

BOOST_FUSION_ADAPT_STRUCT(SBG::SetVertex, (std::string, name_)(int, id_)(SBG::Set, range_)(int, index_)(SBG::SVDesc, desc_))

BOOST_FUSION_ADAPT_STRUCT(SBG::SEDesc, (std::string, text_))

BOOST_FUSION_ADAPT_STRUCT(SBG::SetEdge, (std::string, name_)(int, id_)(SBG::PWLMap, map_f)(SBG::PWLMap, map_u)(int, index_)(SBG::SEDesc, desc_))

// Grammar ----------------------------------------------------------------------------------------

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

namespace client
{
  template <typename Iterator>
  struct sbg_parser : qi::grammar<Iterator, SBG::SetVertex(), boost::spirit::ascii::space_type> {
    sbg_parser() : sbg_parser::base_type(vertex) { 
      inter = qi::int_
        >> ':' >> qi::int_ 
        >> ':' >> qi::int_ 
        >> qi::attr(false);

      multi_inter = '['
        >> inter [phx::bind(&SBG::MultiInterval::addInter, qi::_val, qi::_1)] 
        >> *(',' >> inter [phx::bind(&SBG::MultiInterval::addInter, qi::_val, qi::_1)])
        >> ']';

      set = '{'
        >> ((multi_inter               [phx::bind(&SBG::Set::addAtomSet, qi::_val, qi::_1)] 
               >> *(',' >> multi_inter [phx::bind(&SBG::Set::addAtomSet, qi::_val, qi::_1)]))
            | qi::eps)
        >> '}';

      ident = qi::lexeme[qi::alpha >> *qi::alnum];

      svdesc = qi::lexeme['"' >> *(qi::char_ - '"') >> '"'];

      vertex = ident 
        >> qi::attr(vertex_counter) [phx::ref(vertex_counter) += 1]
        >> set >> qi::attr(0) >> svdesc >> ';';

      sedesc = qi::lexeme['"' >> +(qi::char_ - '"') >> '"'];

      //edge = (ident - ':') >> ':' >> multi_inter >> '-' 
      //  >> (ident - ':') >> ':' >> multi_inter >> ';' >> '\n';  
    }

    boost::spirit::qi::rule<Iterator, SBG::Interval(), boost::spirit::ascii::space_type> inter;
    boost::spirit::qi::rule<Iterator, SBG::MultiInterval(), boost::spirit::ascii::space_type> multi_inter;
    boost::spirit::qi::rule<Iterator, SBG::Set(), boost::spirit::ascii::space_type> set;

    boost::spirit::qi::rule<Iterator, std::string(), boost::spirit::ascii::space_type> ident;

    int vertex_counter = 1;
    boost::spirit::qi::rule<Iterator, SBG::SVDesc(), boost::spirit::ascii::space_type> svdesc;
    boost::spirit::qi::rule<Iterator, SBG::SetVertex(), boost::spirit::ascii::space_type> vertex;

    int edge_counter = 1;
    boost::spirit::qi::rule<Iterator, SBG::SEDesc(), boost::spirit::ascii::space_type> sedesc;
    boost::spirit::qi::rule<Iterator, SBG::SetEdge(), boost::spirit::ascii::space_type> edge;
  };
}

// Main program -----------------------------------------------------------------------------------

int main(int argc, char** argv) {
  std::cout << "\n/////////////////////////////////////////////////////////\n\n";
  std::cout << "A simple SBG parser\n\n";
  std::cout << "/////////////////////////////////////////////////////////\n\n";

  typedef std::string::const_iterator iterator_type;
  typedef client::sbg_parser<iterator_type> sbg_parser;

  sbg_parser g; // Our grammar

  if (argv[optind] != NULL) {
    std::string str;

    std::string fname = argv[optind];
    if (fname != "") {
      std::ifstream in(fname.c_str());
      if (in.fail()) {
        std::cerr << "Unable to open file " << fname << std::endl;
        exit(-1);
      }
      in.unsetf(std::ios::skipws);

      std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
      std::string::const_iterator iter = str.begin();
      std::string::const_iterator end = str.end();

      SBG::SetVertex result;
      bool r = phrase_parse(iter, end, g, boost::spirit::ascii::space, result);
      if (r && iter == end) {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        //std::cout << "result = " << result << std::endl;
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
  }

  else 
    std::cout << "A filename should be provided\n";

  std::cout << "Bye... :-) \n\n";
  return 0;
}
