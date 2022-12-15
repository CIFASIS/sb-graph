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
*
* The parser doesn't check for the following conditions that must be satisfied:
*   a. Set-vertices should be declared with disjunct ranges
*   b. Set-edges are declared in array subscript fashion. For example, if we
*      declare a set-vertex v0 {[100:5:200]} and an edge v0[1:1:10], then the
*      referrenced interval is [100:5:150].
*/

#include <parser/parser.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

// Adapt structures -------------------------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(SBG::Interval, (SBG::INT, lo_)(SBG::INT, step_)(SBG::INT, hi_)(bool, empty_))

BOOST_FUSION_ADAPT_STRUCT(SBG::MultiInterval, (SBG::MultiInterval::Intervals, inters_)(int, ndim_))

BOOST_FUSION_ADAPT_STRUCT(SBG::Set, (SBG::Set::AtomSets, asets_)(int, ndim_))

BOOST_FUSION_ADAPT_STRUCT(SBG::SVDesc, (std::string, text_))

BOOST_FUSION_ADAPT_STRUCT(SBG::SetVertex, (std::string, name_)(int, id_)(SBG::Set, range_)(int, index_)(SBG::SVDesc, desc_))

BOOST_FUSION_ADAPT_STRUCT(SBG::SEDesc, (std::string, text_))

BOOST_FUSION_ADAPT_STRUCT(Parser::SetEdge, (int, id_)(std::string, vb_)(SBG::MultiInterval, mb_)(std::string, vf_)(SBG::MultiInterval, mf_))

BOOST_FUSION_ADAPT_STRUCT(Parser::SetGraph, (std::vector<SBG::SetVertex>, svertices_)(std::vector<Parser::SetEdge>, sedges_))

namespace Parser {

// Self-defined structures ------------------------------------------------------------------------

SetEdge::SetEdge() {}

SetEdge::SetEdge(int id, std::string vb, SBG::MultiInterval mb, std::string vf, SBG::MultiInterval mf) : id_(id), vb_(vb), mb_(mb), vf_(vf), mf_(mf)  {}

member_imp(Parser::SetEdge, int, id);
member_imp(Parser::SetEdge, std::string, vb);
member_imp(Parser::SetEdge, SBG::MultiInterval, mb);
member_imp(Parser::SetEdge, std::string, vf);
member_imp(Parser::SetEdge, SBG::MultiInterval, mf);

std::ostream &operator<<(std::ostream &out, const SetEdge &se) 
{
  SetEdge auxse = se;

  out << se.vb() << se.mb() << " - " << se.vf() << se.mf() << "\n";

  return out;
}

SetGraph::SetGraph() {}

member_imp(Parser::SetGraph, vrtcs, svertices);
member_imp(Parser::SetGraph, edges, sedges);

std::ostream &operator<<(std::ostream &out, const Parser::SetGraph &sg) 
{
  Parser::SetGraph auxsg = sg;

  BOOST_FOREACH (SBG::SetVertex v, sg.svertices())
    out << v << "\n";

  BOOST_FOREACH (Parser::SetEdge e, sg.sedges())
    out << e << "\n";

  return out;
}

// Grammar ----------------------------------------------------------------------------------------

//namespace qi = boost::spirit::qi;
//namespace phx = boost::phoenix;

sbg_parser::sbg_parser() : sbg_parser::base_type(sbg) 
{ 
    inter %= qi::int_
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

    vertex %= ident 
      >> qi::attr(vertex_counter) [phx::ref(vertex_counter) += 1]
      >> set >> qi::attr(0) >> svdesc >> ';';

    edge %= qi::attr(edge_counter) [phx::ref(edge_counter) += 1] 
      >> ident >> multi_inter >> '-' >> ident >> multi_inter >> ';';  

    sbg %= *vertex >> *edge;
};

} // namespace Parser
