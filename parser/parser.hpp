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
* Currently, only a filename can be provided, input can't be received
* from the command line.
*
* The parser doesn't check for the following conditions that must be satisfied:
*   a. Set-vertices should be declared with disjunct ranges
*   b. Set-edges are declared in array subscript fashion. For example, if we
*      declare a set-vertex v0 {[100:5:200]} and an edge v0[1:1:10], then the
*      referrenced interval is [100:5:150].
*/

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

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

struct SetEdge {
  member_class(int, id);
  member_class(std::string, vb);
  member_class(SBG::MultiInterval, mb);
  member_class(std::string, vf);
  member_class(SBG::MultiInterval, mf);

  SetEdge();
  SetEdge(int id, std::string vb, SBG::MultiInterval mb, std::string vf, SBG::MultiInterval mf);
};

std::ostream &operator<<(std::ostream &out, const SetEdge &se);  

typedef std::vector<SBG::SetVertex> vrtcs;
typedef std::vector<SetEdge> edges;

struct SetGraph {
  member_class(std::string, modifier);
  member_class(vrtcs, svertices);
  member_class(edges, sedges);

  SetGraph();
};

std::ostream &operator<<(std::ostream &out, const SetGraph &sg);  

// Grammar ----------------------------------------------------------------------------------------

typedef typename std::string::const_iterator str_it;

struct sbg_parser : qi::grammar<str_it, SetGraph(), asc::space_type> {
  qi::rule<str_it, SBG::Interval(), asc::space_type> inter;
  qi::rule<str_it, SBG::MultiInterval(), asc::space_type> multi_inter;
  qi::rule<str_it, SBG::Set(), asc::space_type> set;

  qi::rule<str_it, std::string(), asc::space_type> ident;

  qi::rule<str_it, SBG::SVDesc(), asc::space_type> svdesc;
  qi::rule<str_it, SBG::SetVertex(), asc::space_type> vertex;

  qi::rule<str_it, SetEdge(), asc::space_type> edge;

  qi::rule<str_it, SetGraph(), asc::space_type> sbg;

  sbg_parser();
};

} // namespace Parser
