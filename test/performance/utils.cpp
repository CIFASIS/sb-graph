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

#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <regex>

#include "algorithms/misc/causalization_builders.hpp"
#include "eval/file_evaluator.cpp"
#include "eval/user_impl_map.hpp"

namespace Test {

namespace Internal {


////////////////////////////////////////////////////////////////////////////////
// Auxiliary functions ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

bool updateN(const std::string& filename, int N)
{
    std::ifstream input_file(filename);
    if (!input_file.is_open()) {
      std::cerr << "Error>> Unable to open file " << filename << "\n";
      return false;
    }

    std::string new_content = "";
    std::string line;
    std::regex pattern("^(\\s*N\\s*=\\s*).*");
    bool found = false;
    while (std::getline(input_file, line)) {
      std::smatch common;
      if (std::regex_search(line, common, pattern)) {
        line = common[1].str() + std::to_string(N) + ";";
        found = true;
      }
      new_content += line + "\n";
    }
    input_file.close();

    if (!found) {
      std::cout << "\n";
      return false;
    }

    std::ofstream output_file(filename);
    output_file << new_content;
    output_file.close();

    return true;
}

SBG::LIB::SBG generateSBG(std::string filename, int N, int copies)
{
  std::streambuf* original_buf = std::cout.rdbuf();
  std::ofstream nullStream("/dev/null");
  std::cout.rdbuf(nullStream.rdbuf());

  // Set value of N in file
  updateN(filename, N);

  // Get graph from file
  SBG::LIB::SBG g;
  SBG::Eval::ProgramIO eval_result = SBG::Eval::parseEvalFile(filename); 
  for (const SBG::Eval::ExprResult &ev : eval_result.exprs()) {
    SBG::Eval::ExprBaseType e = std::get<1>(ev);
    if (std::holds_alternative<SBG::LIB::SBG>(e)) {
      g = std::get<SBG::LIB::SBG>(e);
    }
  }

  std::cout.rdbuf(original_buf);

  return g.copy(copies);
}

SBG::LIB::MatchData calculateMatching(std::string filename, int N, int copies)
{
  // Calculate matching
  SBG::LIB::SBG match_sbg = generateSBG(filename, N, copies);
  SBG::LIB::Matching match_algorithm
    = SBG::LIB::MATCH_FACT.createMatchAlgorithm();
  SBG::LIB::MatchData match_result = match_algorithm.calculate(match_sbg);

  return match_result;  
}

} // namespace Internal

} // namespace Test
