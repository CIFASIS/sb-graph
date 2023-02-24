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
* This is a really tiny parser for SBGs. It was developed as a test tool.
* Currently, only a filename can be provided, input can't be received
* from the command line.
*
* The parser doesn't check for the following conditions that must be satisfied:
*   a. Set-vertices should be declared with disjoint ranges.
*   b. Set-edges are declared in array subscript fashion. For example, if we
*      declare a set-vertex v0 {[100:5:200]} and an edge v0[1:1:10], then the
*      referrenced interval is [100:5:150].
*
* Refer to parser/sbg_syntax.txt if clarification of the SBG syntax is needed. 
*/

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/repository/include/qi_kwd.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <sbg/descs.hpp>
#include <sbg/interval.hpp>
#include <sbg/multi_interval.hpp>
#include <sbg/set.hpp>
#include <sbg/pw_map.hpp>
#include <sbg/sbg.hpp>
#include <sbg/util/defs.hpp>

namespace asc = boost::spirit::ascii;
namespace phx = boost::phoenix;
namespace qi = boost::spirit::qi;

namespace Parser {

// Self-defined structures ------------------------------------------------------------------------
// These structures weren't needed, but I didn't know how to make spirit work with the SBG
// declarations. They're intermmediate representations, that should be converted to SBG structs

struct ConstantDef {
  member_class(std::string, identifier);
  member_class(SBG::INT, value);

  ConstantDef();
  ConstantDef(std::string identifier, SBG::INT value);
};
std::ostream &operator<<(std::ostream &out, const ConstantDef &cd);  

typedef std::vector<ConstantDef> ConstantDefs;
std::ostream &operator<<(std::ostream &out, const ConstantDefs &cds);  

using Literal = boost::variant<std::string, SBG::INT>;

enum class Op {
  add, mult, sub
};

extern const char* OpNames[];

std::ostream &operator<<(std::ostream &out, const Op &op);

struct BinOp;

typedef boost::variant<Literal, boost::recursive_wrapper<BinOp>> Expr;

struct BinOp {
  member_class(Expr, left);
  member_class(Op, op);
  member_class(Expr, right);

  BinOp();
  BinOp(Expr left, Op op, Expr right);
};

std::ostream &operator<<(std::ostream &out, const BinOp &le);

struct Interval {
  member_class(Expr, lo);
  member_class(Expr, step);
  member_class(Expr, hi);

  Interval();
  Interval(Expr lo, Expr step, Expr hi);
};

std::ostream &operator<<(std::ostream &out, const Interval &i);  

typedef std::vector<Interval> Intervals;

typedef std::vector<SBG::INT> Ints; 

struct MultiInterval {
  member_class(Intervals, inters);

  MultiInterval();
  MultiInterval(Intervals inters);

  void addInter(Interval i);
};

std::ostream &operator<<(std::ostream &out, const MultiInterval &mi);  

typedef std::vector<MultiInterval> MultiInters;

struct Set {
  member_class(MultiInters, asets);

  Set();
  Set(MultiInters asets);

  void addAtomSet(MultiInterval mi);
};

std::ostream &operator<<(std::ostream &out, const Set &s);  

struct SetVertex {
  member_class(std::string, name);
  member_class(Set, range);

  SetVertex();
  SetVertex(std::string name, Set range);
};

std::ostream &operator<<(std::ostream &out, const SetVertex &sv);  

struct SetEdge {
  member_class(std::string, vb);
  member_class(MultiInterval, mb);
  member_class(std::string, vf);
  member_class(MultiInterval, mf);

  SetEdge();
  SetEdge(std::string vb, MultiInterval mb, std::string vf, MultiInterval mf);
};

std::ostream &operator<<(std::ostream &out, const SetEdge &se);  

typedef std::map<std::string, SBG::INT> ConstantsEnv; 
typedef std::vector<SetVertex> vrtcs;
typedef std::vector<SetEdge> edges;

struct SetGraph {
  member_class(std::string, modifier);
  member_class(ConstantDefs, constants);
  member_class(vrtcs, svertices);
  member_class(edges, sedges);

  member_class(ConstantsEnv, cenv);

  SetGraph();

  void createConstantsEnv();
};

std::ostream &operator<<(std::ostream &out, const SetGraph &sg);  

// Grammar ----------------------------------------------------------------------------------------

typedef typename std::string::const_iterator str_it;

struct sbg_parser : qi::grammar<str_it, SetGraph(), asc::space_type> {
  qi::rule<str_it, std::string(), asc::space_type> ident;
  qi::rule<str_it, ConstantDef(), asc::space_type> constant_def;

  qi::rule<str_it, Literal(), asc::space_type> literal;
  qi::rule<str_it, BinOp(), asc::space_type> binop;
  qi::rule<str_it, Expr(), asc::space_type> expr;

  qi::rule<str_it, Interval(), asc::space_type> inter;
  qi::rule<str_it, MultiInterval(), asc::space_type> multi_inter;
  qi::rule<str_it, Set(), asc::space_type> set;

  qi::rule<str_it, SetVertex(), asc::space_type> vertex;
  qi::rule<str_it, SetEdge(), asc::space_type> edge;

  qi::rule<str_it, SetGraph(), asc::space_type> sbg;

  sbg_parser();

  struct op_symbols_struct : qi::symbols<char, Parser::Op> {
    op_symbols_struct(){
      this->add("+", Parser::Op::add)("*", Parser::Op::mult)("-", Parser::Op::sub);
    }
  } op_symbols;

  struct keywords_struct : qi::symbols<char, std::string> {
    keywords_struct(){
      this->add("directed", "directed")("undirected", "undirected");
    }
  } keywords;
};

} // namespace Parser
