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
#include <parser/compact_parser.hpp>

// Adapt structures -------------------------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(Parser::ConstantDef, (std::string, identifier_)(SBG::INT, value_))

BOOST_FUSION_ADAPT_STRUCT(Parser::BinOp, (Parser::Expr, left_)(Parser::Op, op_)(Parser::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(Parser::Interval, (Parser::Expr, lo_)(Parser::Expr, step_)(Parser::Expr, hi_))

BOOST_FUSION_ADAPT_STRUCT(Parser::MultiInterval, (Parser::Intervals, inters_))

BOOST_FUSION_ADAPT_STRUCT(Parser::Set, (Parser::MultiInters, asets_))

BOOST_FUSION_ADAPT_STRUCT(Parser::CompSetVertex, (std::string, name_)(Parser::PWLExp, size_))

BOOST_FUSION_ADAPT_STRUCT(Parser::VarUsage, (std::string, name_)(Parser::MultiInterval, subs_))

BOOST_FUSION_ADAPT_STRUCT(Parser::CompSetEdge, (Parser::VarUsage, left_)(Parser::VarUsage, right_))

BOOST_FUSION_ADAPT_STRUCT(Parser::CompSetGraph, (std::string, modifier_)(Parser::ConstantDefs, constants_)(Parser::CVertices, svertices_)(Parser::CEdges, sedges_))


// Self-defined structures ------------------------------------------------------------------------

namespace Parser {

CompSetVertex::CompSetVertex() : name_(), size_() {}
CompSetVertex::CompSetVertex(std::string name, PWLExp size) : name_(name), size_(size) {}

member_imp(Parser::CompSetVertex, std::string, name);
member_imp(Parser::CompSetVertex, PWLExp, size);

std::ostream &operator<<(std::ostream &out, const Parser::CompSetVertex &sv)
{
  out << sv.name() << " " << sv.size() << ";\n";

  return out;
}

VarUsage::VarUsage() : name_(), subs_() {}
VarUsage::VarUsage(std::string name, MultiInterval subs) : name_(name), subs_(subs) {}

member_imp(Parser::VarUsage, std::string, name);
member_imp(Parser::VarUsage, MultiInterval, subs);

bool VarUsage::operator==(const VarUsage &other) const
{
  return name() == other.name() && subs() == other.subs();
}

bool VarUsage::operator<(const VarUsage &other) const
{
  if (name() < other.name())
    return true;

  return subs() < other.subs();
}

std::ostream &operator<<(std::ostream &out, const Parser::VarUsage &vu)
{
  out << vu.name() << vu.subs();

  return out;
}

CompSetEdge::CompSetEdge() : left_(), right_() {}
CompSetEdge::CompSetEdge(VarUsage left, VarUsage right) : left_(), right_() {}

member_imp(Parser::CompSetEdge, VarUsage, left);
member_imp(Parser::CompSetEdge, VarUsage, right);

bool CompSetEdge::operator==(const CompSetEdge &other) const
{
  return left() == other.left() && right() == other.right();
}

bool CompSetEdge::operator<(const CompSetEdge &other) const
{
  return left() < other.left() && right() < other.right();
}

std::ostream &operator<<(std::ostream &out, const CompSetEdge &se) 
{
  out << se.left() << " - "  << se.right() << ";\n";

  return out;
}

CompSetGraph::CompSetGraph() : modifier_(), constants_(), svertices_(), sedges_(), cenv_() {}

member_imp(Parser::CompSetGraph, std::string, modifier);
member_imp(Parser::CompSetGraph, ConstantDefs, constants)
member_imp(Parser::CompSetGraph, CVertices, svertices);
member_imp(Parser::CompSetGraph, CEdges, sedges);
member_imp(Parser::CompSetGraph, ConstantsEnv, cenv);

void CompSetGraph::createConstantsEnv()
{
  BOOST_FOREACH (ConstantDef cd, constants())
    cenv_ref()[cd.identifier()] = cd.value();  
}

std::ostream &operator<<(std::ostream &out, const Parser::CompSetGraph &sg) 
{
  out << sg.modifier() << ";\n\n" << sg.constants() << "\n";

  BOOST_FOREACH (Parser::CompSetVertex sv, sg.svertices())
    out << sv;
  out << "\n";

  BOOST_FOREACH (Parser::CompSetEdge se, sg.sedges())
    out << se;
  out << "\n";

  return out;
}

// Grammar ----------------------------------------------------------------------------------------

comp_sbg_parser::comp_sbg_parser() : comp_sbg_parser::base_type(comp_sbg) 
{ 
  ident = qi::lexeme[qi::alpha >> *qi::alnum];

  constant_def %= ident >> '=' >> qi::int_ >> ';';

  literal %= qi::int_ | ident; 

  mult %= qi::int_ >> mult_symbol >> literal; 

  lexpr %= (mult | literal) >> add_symbols >> ('(' >> lexpr >> ')' | qi::int_);

  expr %= lexpr | mult | literal;

  inter %= expr
    >> ':' >> expr 
    >> ':' >> expr;

  multi_inter = '['
    >> inter [phx::bind(&MultiInterval::addInter, qi::_val, qi::_1)] 
    >> *(',' >> inter [phx::bind(&MultiInterval::addInter, qi::_val, qi::_1)])
    >> ']';

  pw_lexp %= '[' >> expr >> *(',' >> expr) >> ']';

  vertex %= ident >> pw_lexp >> ';';

  var_usage %= ident >> multi_inter;

  edge %= var_usage >> '-' >> var_usage >>';';

  comp_sbg %= keywords >> ';' >> *constant_def >> *vertex >> *edge;
};

} // namespace Parser
