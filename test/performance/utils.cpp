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

#include "algorithms/matching/matching.hpp"
#include "algorithms/matching/match_data.hpp"
#include "eval/base_type.hpp"
#include "eval/file_evaluator.cpp"
#include "eval/pretty_print.hpp"
#include "sbg/bipartite_sbg.hpp"
#include "sbg/expression.hpp"
#include "sbg/interval.hpp"
#include "sbg/natural.hpp"
#include "sbg/pw_map.hpp"
#include "sbg/rational.hpp"
#include "sbg/set.hpp"

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <regex>
#include <sys/wait.h>
#include <string>

namespace SBG {

namespace perf {

namespace detail {

using SBG::LIB::NAT;
using SBG::LIB::RATIONAL;
using SBG::LIB::detail::Interval;
using SBG::LIB::Set;
using SBG::LIB::Expression;
using SBG::LIB::PWMap;
using SBG::LIB::BipartiteSBG;
using SBG::LIB::Matching;
using SBG::LIB::MatchData;

////////////////////////////////////////////////////////////////////////////////
// File Manipulation -----------------------------------------------------------
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

BipartiteSBG generateSBG(std::string filename, int N, int copies)
{
  std::streambuf* original_buf = std::cout.rdbuf();
  std::ofstream nullStream("/dev/null");
  std::cout.rdbuf(nullStream.rdbuf());

  // Set value of N in file
  updateN(filename, N);

  // Get graph from file
  BipartiteSBG g;
  SBG::Eval::ProgramIO eval_result = SBG::Eval::parseEvalFile(filename); 
  for (const SBG::Eval::ExprResult& ev : eval_result.exprs()) {
    SBG::Eval::ExprBaseType e = std::get<1>(ev);
    if (std::holds_alternative<BipartiteSBG>(e)) {
      g = std::get<BipartiteSBG>(e);
    }
  }

  std::cout.rdbuf(original_buf);

  return copy(copies, g);
}

MatchData calculateMatching(std::string filename, int N, int copies)
{
  // Calculate matching
  BipartiteSBG match_sbg = generateSBG(filename, N, copies);
  Matching match_algorithm;
  MatchData match_result = match_algorithm.calculate(match_sbg);

  return match_result;  
}

////////////////////////////////////////////////////////////////////////////////
// Set Construction ------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

std::pair<Set, Set> nonDisjointPieces(NAT set_sz)
{
  NAT inter_sz = 100;

  Set s1;
  Set s2;
  for (unsigned int h = 0; h < set_sz; ++h) {
    Interval i1{h*inter_sz, 1, (h + 1)*inter_sz - 1};
    Set jth_s1{i1.begin(), i1.step(), i1.end()};
    s1 = s1.disjointCup(jth_s1);

    NAT off = inter_sz/2;
    Interval i2{off + (h*inter_sz), 1, off + (h + 1)*inter_sz - 1};
    Set jth_s2{i2.begin(), i2.step(), i2.end()};
    s2 = s2.disjointCup(jth_s2);
  }

  Set second_dim{0, 1, inter_sz - 1};
  s1 = s1.cartesianProduct(second_dim);
  s2 = s2.cartesianProduct(second_dim);

  return std::make_pair(std::move(s1), std::move(s2));
}

std::pair<Set, Set> interlacedPieces(NAT set_sz)
{
  NAT inter_sz = 100;

  Set s1;
  Set s2;
  for (SBG::LIB::NAT j = 0; j < set_sz; j += 2) {
    Set jth_s1{j*inter_sz, 1, (j + 1)*inter_sz - 1};
    s1 = std::move(s1.disjointCup(jth_s1));
    Set jth_s2{(j + 1)*inter_sz, 1, (j + 2)*inter_sz - 1};
    s2 = std::move(s2.disjointCup(jth_s2));
  }

  return std::make_pair(std::move(s1), std::move(s2));
}

////////////////////////////////////////////////////////////////////////////////
// PWMap Construction ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

PWMap denseDom(NAT map_sz)
{
  NAT inter_sz = 100;
  NAT set_sz = 10;

  PWMap pw;
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set domain;
    NAT offset = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i{offset + (h*inter_sz), 1, offset + (h + 1)*inter_sz - 1};
      Set jth_domain{i.begin(), i.step(), i.end()};
      domain = std::move(domain).disjointCup(std::move(jth_domain));
    }
    Expression id{RATIONAL{1}, RATIONAL{0}};

    pw.emplace(domain, id);
  }

  return pw;
}

std::pair<PWMap, PWMap> minAdjMaps(NAT map_sz)
{
  NAT inter_sz = 100;
  NAT set_sz = 10;

  Set second_dim{0, 1, inter_sz - 1};
  PWMap pw1;
  PWMap pw2;
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set domain1;
    Set domain2;
    NAT offset = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1{offset + (h*inter_sz), 1, offset + (h + 1)*inter_sz - 1};
      Set jth_domain1{i1.begin(), i1.step(), i1.end()};
      domain1 = std::move(domain1).disjointCup(std::move(jth_domain1));

      NAT offset2 = offset + inter_sz/2;
      Interval i2{offset2 + (h*inter_sz), 1, offset2 + (h + 1)*inter_sz - 1};
      Set jth_domain2{i2.begin(), i2.step(), i2.end()};
      domain2 = std::move(domain2).disjointCup(std::move(jth_domain2));
    }

    Expression id{RATIONAL{1}, RATIONAL{0}};
    Expression minus_one{RATIONAL{1}, RATIONAL{-1, 1}};

    pw1.emplace(domain1, id);
    pw2.emplace(domain2, minus_one);
  }

  return {pw1, pw2};
}

std::pair<PWMap, PWMap> interlacedMaps(NAT map_sz)
{
  NAT inter_sz = 100;
  NAT set_sz = 10;

  Set second_dim{0, 1, inter_sz - 1};
  PWMap pw1;
  PWMap pw2;
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set domain1;
    Set domain2;
    NAT offset = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; h += 2) {
      Interval i1{offset + (h*inter_sz), 1, offset + (h + 1)*inter_sz - 1};
      Set jth_domain1{i1.begin(), i1.step(), i1.end()};
      domain1 = std::move(domain1).disjointCup(std::move(jth_domain1));

      NAT offset2 = offset + inter_sz;
      Interval i2{offset2 + (h*inter_sz), 1, offset2 + (h + 1)*inter_sz - 1};
      Set jth_domain2{i2.begin(), i2.step(), i2.end()};
      domain2 = std::move(domain2).disjointCup(std::move(jth_domain2));
    }
    domain1 = domain1.cartesianProduct(second_dim);
    domain2 = domain2.cartesianProduct(second_dim);

    Expression multidim_id{2, 1, 0};

    pw1.emplace(domain1, multidim_id);
    pw2.emplace(domain2, multidim_id);
  }

  return {pw1, pw2};
}

std::pair<PWMap, PWMap> nonDisjointMaps(NAT map_sz)
{
  NAT inter_sz = 100;
  NAT set_sz = 10;

  Set second_dim{0, 1, inter_sz - 1};
  PWMap pw1;
  PWMap pw2;
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set domain1;
    Set domain2;
    NAT offset = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i1{offset + (h*inter_sz), 1, offset + (h + 1)*inter_sz - 1};
      Set jth_domain1{i1.begin(), i1.step(), i1.end()};
      domain1 = std::move(domain1).disjointCup(std::move(jth_domain1));

      NAT offset2 = offset + inter_sz/2;
      Interval i2{offset2 + (h*inter_sz), 1, offset2 + (h + 1)*inter_sz - 1};
      Set jth_domain2{i2.begin(), i2.step(), i2.end()};
      domain2 = std::move(domain2).disjointCup(std::move(jth_domain2));
    }
    domain1 = domain1.cartesianProduct(second_dim);
    domain2 = domain2.cartesianProduct(second_dim);

    Expression multidim_id{2, 1, 0};

    pw1.emplace(domain1, multidim_id);
    pw2.emplace(domain2, multidim_id);
  }

  return {pw1, pw2};
}

PWMap reducibleMaps(NAT map_sz)
{
  NAT inter_sz = 100;
  NAT set_sz = 10;

  Set second_dim{0, 1, inter_sz - 1};
  PWMap pw;
  for (unsigned int j = 0; j < map_sz; ++j) {
    Set domain;
    NAT offset = j*set_sz*inter_sz;
    for (unsigned int h = 0; h < set_sz; ++h) {
      Interval i{offset + (h*inter_sz), 1, offset + (h + 1)*inter_sz - 1};
      Set jth_domain{i.begin(), i.step(), i.end()};
      domain = std::move(domain).disjointCup(std::move(jth_domain));
    }
    domain = domain.cartesianProduct(second_dim);

    Expression plus_one{RATIONAL{1}, RATIONAL{1}};
    Expression id{RATIONAL{1}, RATIONAL{0}};
    Expression expr = plus_one.cartesianProduct(id);

    pw.emplace(domain, expr);
  }

  return pw;
}



} // namespace detail

} // namespace perf

} // namespace SBG
