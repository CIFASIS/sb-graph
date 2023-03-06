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

/**
* This is a really tiny parser for compact SBGs. It was developed as a test tool.
* These are compact SBG in the sense that the set-vertex domains are composed
* of consecutive values (thus only multi-intervals are needed to represent them). 
*
* Currently, only a filename can be provided, input can't be received
* from the command line.
*
* The parser doesn't check for the following conditions that must be satisfied:
*   a. Set-vertices should be declared with disjoint ranges.
*   b. Set-edges are declared in array subscript fashion. For example, if we
*      declare a set-vertex v0 {[100:5:200]} and an edge v0[1:1:10], then the
*      referrenced interval is [100:5:150].
*
* Refer to parser/compact_syntax.txt if clarification of the SBG syntax is needed. 
*
*/

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/repository/include/qi_kwd.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <parser/converter.hpp>

namespace asc = boost::spirit::ascii;
namespace phx = boost::phoenix;
namespace qi = boost::spirit::qi;

namespace Parser {

// Self-defined structures ------------------------------------------------------------------------

struct CompSetVertex {
  member_class(std::string, name);
  member_class(PWLExp, size);

  CompSetVertex();
  CompSetVertex(std::string name, PWLExp size);
};

std::ostream &operator<<(std::ostream &out, const CompSetVertex &sv);  

struct VarUsage {
  member_class(std::string, name);
  member_class(MultiInterval, subs);

  VarUsage();
  VarUsage(std::string name, MultiInterval subs);

  eq_class(VarUsage);
  lt_class(VarUsage);
};

std::ostream &operator<<(std::ostream &out, const VarUsage &sv);  

struct CompSetEdge {
  member_class(VarUsage, left);
  member_class(VarUsage, right);

  CompSetEdge();
  CompSetEdge(VarUsage left, VarUsage right);

  eq_class(CompSetEdge);
  lt_class(CompSetEdge);
};

std::ostream &operator<<(std::ostream &out, const CompSetEdge &se);  

typedef std::vector<CompSetVertex> CVertices;
typedef std::vector<CompSetEdge> CEdges;

struct CompSetGraph {
  member_class(std::string, modifier);
  member_class(ConstantDefs, constants);
  member_class(CVertices, svertices);
  member_class(CEdges, sedges);

  member_class(ConstantsEnv, cenv);

  CompSetGraph();

  void createConstantsEnv();
};

std::ostream &operator<<(std::ostream &out, const CompSetGraph &sg);  

// Grammar ----------------------------------------------------------------------------------------

typedef typename std::string::const_iterator str_it;

struct comp_sbg_parser : qi::grammar<str_it, CompSetGraph(), asc::space_type> {
  qi::rule<str_it, std::string(), asc::space_type> ident;
  qi::rule<str_it, ConstantDef(), asc::space_type> constant_def;

  qi::rule<str_it, Literal(), asc::space_type> literal;
  qi::rule<str_it, BinOp(), asc::space_type> mult;
  qi::rule<str_it, BinOp(), asc::space_type> lexpr;
  qi::rule<str_it, Expr(), asc::space_type> expr;

  qi::rule<str_it, Interval(), asc::space_type> inter;
  qi::rule<str_it, MultiInterval(), asc::space_type> multi_inter;
  qi::rule<str_it, Set(), asc::space_type> set;

  qi::rule<str_it, CompSetVertex(), asc::space_type> vertex;
  qi::rule<str_it, PWLExp(), asc::space_type> pw_lexp;
  qi::rule<str_it, VarUsage(), asc::space_type> var_usage;
  qi::rule<str_it, CompSetEdge(), asc::space_type> edge;

  qi::rule<str_it, CompSetGraph(), asc::space_type> comp_sbg;

  comp_sbg_parser();

  struct mult_symbol_struct : qi::symbols<char, Parser::Op> {
    mult_symbol_struct(){
      this->add("*", Parser::Op::mult);
    }
  } mult_symbol;

  struct add_symbols_struct : qi::symbols<char, Parser::Op> {
    add_symbols_struct(){
      this->add("+", Parser::Op::add)("-", Parser::Op::sub);
    }
  } add_symbols;

  struct keywords_struct : qi::symbols<char, std::string> {
    keywords_struct(){
      this->add("directed", "directed")("undirected", "undirected");
    }
  } keywords;
};

} // namespace Parser
