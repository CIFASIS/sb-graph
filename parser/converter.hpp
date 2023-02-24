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
#include "boost/variant.hpp"

#include <parser/parser.hpp>
#include <sbg/sbg.hpp>

namespace Parser {

// Visitors ---------------------------------------------------------------------------------------

struct LitVisitor : public boost::static_visitor<SBG::INT> {
  member_class(ConstantsEnv, cenv);

  LitVisitor(ConstantsEnv cenv);

  SBG::INT operator()(SBG::INT i) const;
  SBG::INT operator()(std::string identifier) const;
};

struct ExprVisitor : public boost::static_visitor<SBG::INT> {
  member_class(ConstantsEnv, cenv);

  ExprVisitor(ConstantsEnv cenv);

  SBG::INT operator()(Parser::Literal l) const;
  SBG::INT operator()(Parser::BinOp bop) const;
};

// Converter --------------------------------------------------------------------------------------

typedef boost::variant<SBG::SBGraph, SBG::DSBGraph> Grph;

struct Converter {
  member_class(SBG::OrdCT<SBG::INT>, dom_offset);
  member_class(Parser::SetGraph, sg);

  Converter();
  Converter(Parser::SetGraph sg);

  Grph convertGraph();

  private:
  SBG::INT convertExpr(Parser::Expr e);
  SBG::Interval convertInterval(Parser::Interval i);
  SBG::MultiInterval convertMI(Parser::MultiInterval mi);
  SBG::Set convertSet(Parser::Set s);

  SBG::SetVertex convertVertex(Parser::SetVertex sv);

  SBG::MultiInterval makeDom(SBG::MultiInterval mi1, SBG::MultiInterval mi2);
  SBG::LMap makeExp(SBG::MultiInterval dom, SBG::MultiInterval mi);
  void addEdge(SBG::SBGraph &g, Parser::SetEdge se);
  void addDirectedEdge(SBG::DSBGraph &dg, Parser::SetEdge se);

  SBG::SBGraph convertUndirectedGraph();
  SBG::DSBGraph convertDirectedGraph();
};

} // namespace Parser
