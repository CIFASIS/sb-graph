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

// Simple test of atomize function for SBG

#include <parser/converter.hpp>
#include <sbg/atom_sbg.hpp>

int main()
{
  Parser::sbg_parser g; // Our grammar

  std::string fname = "../../../utest/files/atom.txt";

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

    Parser::SetGraph result;
    bool r = boost::spirit::qi::phrase_parse(iter, end, g, boost::spirit::ascii::space, result);

    Parser::Converter c(result);
    Parser::Grph g = c.convertGraph();

    SBG::AtomDSBGraph res = SBG::atomize(boost::get<SBG::DSBGraph>(g));
    if (r && iter == end) {
      std::cout << "-------------------------\n";
      std::cout << "Parsing succeeded\n";
      std::cout << "SBG atomizer result = \n\n" << "\n";
      std::cout << res;
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

  else 
    std::cout << "A filename should be provided\n";

  std::cout << "Bye... :-) \n\n";

  return 0;
}
