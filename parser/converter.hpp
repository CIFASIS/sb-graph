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

struct ConstVisitor : public boost::static_visitor<bool> {
  ConstVisitor();

  bool operator()(std::string var) const;
  bool operator()(SBG::INT i) const;
  bool operator()(Parser::Literal l) const;
  bool operator()(Parser::BinOp bop) const;
};

struct VarVisitor : public boost::static_visitor<bool> {
  VarVisitor();

  bool operator()(std::string var) const;
  bool operator()(SBG::INT i) const;
  bool operator()(Parser::Literal l) const;
  bool operator()(Parser::BinOp bop) const;
};

struct MultVisitor : public boost::static_visitor<bool> {
  MultVisitor();

  bool operator()(Parser::Literal l) const;
  bool operator()(Parser::BinOp bop) const;
};

struct LExprVisitor : public boost::static_visitor<bool> {
  LExprVisitor();

  bool operator()(Parser::Literal l) const;
  bool operator()(Parser::BinOp bop) const;
};

struct SlopeVisitor : public boost::static_visitor<SBG::INT> {
  SlopeVisitor();

  SBG::INT operator()(Parser::Literal l) const;
  SBG::INT operator()(Parser::BinOp bop) const;
};

struct OffsetVisitor : public boost::static_visitor<SBG::INT> {
  OffsetVisitor();

  SBG::INT operator()(Parser::Literal l) const;
  SBG::INT operator()(Parser::BinOp bop) const;
};

SBG::INT convertExpr(Parser::Expr e, ConstantsEnv cenv);
SBG::Interval convertInterval(Parser::Interval i, ConstantsEnv cenv);
SBG::MultiInterval convertMI(Parser::MultiInterval mi, ConstantsEnv cenv);
SBG::Set convertSet(Parser::Set s, ConstantsEnv cenv);
SBG::LMap convertPWLExp(Parser::PWLExp pwl, ConstantsEnv cenv);

// Converter --------------------------------------------------------------------------------------

typedef boost::variant<SBG::SBGraph, SBG::DSBGraph> Grph;
typedef boost::variant<SBG::SetEdge, SBG::DSetEdge> VariantEdge;

typedef std::map<std::string, SBG::SetVertexDesc> VertexMap;
typedef std::map<std::string, SBG::DSetVertexDesc> DVertexMap;

struct Converter {
  member_class(SBG::OrdCT<SBG::INT>, offset);
  member_class(Parser::SetGraph, sg);

  Converter();
  Converter(Parser::SetGraph sg);

  Grph convertGraph();

  private:
  SBG::SetVertex convertVertex(Parser::SetVertex sv);
  VariantEdge convertEdge(Parser::SetEdge se);

  SBG::SBGraph convertUndirectedGraph();
  SBG::DSBGraph convertDirectedGraph();
};

} // namespace Parser
