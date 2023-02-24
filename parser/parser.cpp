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
#include <parser/parser.hpp>

// Adapt structures -------------------------------------------------------------------------------

BOOST_FUSION_ADAPT_STRUCT(Parser::ConstantDef, (std::string, identifier_)(SBG::INT, value_))

BOOST_FUSION_ADAPT_STRUCT(Parser::BinOp, (Parser::Expr, left_)(Parser::Op, op_)(Parser::Expr, right_))

BOOST_FUSION_ADAPT_STRUCT(Parser::Interval, (Parser::Expr, lo_)(Parser::Expr, step_)(Parser::Expr, hi_))

BOOST_FUSION_ADAPT_STRUCT(Parser::MultiInterval, (Parser::Intervals, inters_))

BOOST_FUSION_ADAPT_STRUCT(Parser::Set, (Parser::MultiInters, asets_))

BOOST_FUSION_ADAPT_STRUCT(Parser::SetVertex, (std::string, name_)(Parser::Set, range_))

BOOST_FUSION_ADAPT_STRUCT(Parser::SetEdge, (std::string, vb_)(Parser::MultiInterval, mb_)(std::string, vf_)(Parser::MultiInterval, mf_))

BOOST_FUSION_ADAPT_STRUCT(Parser::SetGraph, (std::string, modifier_)(Parser::ConstantDefs, constants_)(std::vector<Parser::SetVertex>, svertices_)(std::vector<Parser::SetEdge>, sedges_))


// Self-defined structures ------------------------------------------------------------------------

namespace Parser {

ConstantDef::ConstantDef() : identifier_(), value_(0) {}
ConstantDef::ConstantDef(std::string identifier, SBG::INT value) : identifier_(identifier), value_(value) {}

member_imp(Parser::ConstantDef, std::string, identifier);
member_imp(Parser::ConstantDef, SBG::INT, value);

std::ostream &operator<<(std::ostream &out, const ConstantDef &cd) 
{
  out << "INT " << cd.identifier() << " = " << cd.value();

  return out;
}

std::ostream &operator<<(std::ostream &out, const ConstantDefs &cds)
{
  BOOST_FOREACH (ConstantDef cd, cds)
    std::cout << cd << ";\n";

  return out;
}

const char *OpNames[] = {"+", "*", "-"};

std::ostream &operator<<(std::ostream &out, const Op &op)
{
  out << OpNames[static_cast<int>(op)];

  return out;
}

BinOp::BinOp() : left_(), op_(), right_() {}
BinOp::BinOp(Expr left, Op op, Expr right) : left_(left), op_(op), right_(right) {}

member_imp(Parser::BinOp, Parser::Expr, left);
member_imp(Parser::BinOp, Parser::Op, op);
member_imp(Parser::BinOp, Parser::Expr, right);

std::ostream &operator<<(std::ostream &out, const BinOp &le)
{
  out << le.left() << le.op() << le.right();

  return out;
}

Interval::Interval() : lo_(), step_(), hi_() {}
Interval::Interval(Expr lo, Expr step, Expr hi) : lo_(lo), step_(step), hi_(hi) {}

member_imp(Parser::Interval, Expr, lo);
member_imp(Parser::Interval, Expr, step);
member_imp(Parser::Interval, Expr, hi);

std::ostream &operator<<(std::ostream &out, const Parser::Interval &i)
{
  out << i.lo() << ":" << i.step() << ":" << i.hi();

  return out;
}

MultiInterval::MultiInterval() : inters_() {}
MultiInterval::MultiInterval(Intervals inters) : inters_(inters) {}

member_imp(Parser::MultiInterval, Intervals, inters);

void MultiInterval::addInter(Interval i) { inters_ref().insert(inters_ref().end(), i); }

std::ostream &operator<<(std::ostream &out, const Parser::MultiInterval &mi)
{
  MultiInterval aux = mi;  

  out << "[";
  int sz = aux.inters_ref().size(), i = 0;
  if (sz > 0) {
    for (; i < sz - 1; i++) {
      out << aux.inters_ref()[i] << ", ";
    }
    out << aux.inters_ref()[i];
  }
  out << "]";

 return out; 
}

Set::Set() : asets_() {}
Set::Set(MultiInters asets) : asets_(asets) {}

member_imp(Parser::Set, MultiInters, asets);

void Set::addAtomSet(MultiInterval mi) { asets_ref().insert(asets_ref().end(), mi); }

std::ostream &operator<<(std::ostream &out, const Parser::Set &s)
{
  Set aux = s;

  out << "{";
  int sz = aux.asets_ref().size(), i = 0;
  if (sz > 0) {
    for (; i < sz - 1; i++) {
      out << aux.asets_ref()[i] << ", ";
    }
    out << aux.asets_ref()[i];
  }
  out << "}";

 return out; 
}

SetVertex::SetVertex() : name_(), range_() {}
SetVertex::SetVertex(std::string name, Set range) : name_(name), range_(range) {}

member_imp(Parser::SetVertex, std::string, name);
member_imp(Parser::SetVertex, Set, range);

std::ostream &operator<<(std::ostream &out, const Parser::SetVertex &sv)
{
  std::cout << sv.name() << " " << sv.range() << ";\n";

  return out;
}

SetEdge::SetEdge() : vb_(), mb_(), vf_(), mf_() {}
SetEdge::SetEdge(std::string vb, MultiInterval mb, std::string vf, MultiInterval mf) : vb_(vb), mb_(mb), vf_(vf), mf_(mf)  {}

member_imp(Parser::SetEdge, std::string, vb);
member_imp(Parser::SetEdge, MultiInterval, mb);
member_imp(Parser::SetEdge, std::string, vf);
member_imp(Parser::SetEdge, MultiInterval, mf);

std::ostream &operator<<(std::ostream &out, const SetEdge &se) 
{
  out << se.vb() << se.mb() << " - " << se.vf() << se.mf() << ";\n";

  return out;
}

SetGraph::SetGraph() : modifier_(), constants_(), svertices_(), sedges_(), cenv_() {}

member_imp(Parser::SetGraph, std::string, modifier);
member_imp(Parser::SetGraph, ConstantDefs, constants)
member_imp(Parser::SetGraph, vrtcs, svertices);
member_imp(Parser::SetGraph, edges, sedges);

member_imp(Parser::SetGraph, ConstantsEnv, cenv);

void SetGraph::createConstantsEnv()
{
  BOOST_FOREACH (ConstantDef cd, constants())
    cenv_ref()[cd.identifier()] = cd.value();  
}

std::ostream &operator<<(std::ostream &out, const Parser::SetGraph &sg) 
{
  out << sg.modifier() << ";\n\n" << sg.constants() << "\n";

  BOOST_FOREACH (Parser::SetVertex sv, sg.svertices())
    out << sv;
  std::cout << "\n";

  BOOST_FOREACH (Parser::SetEdge se, sg.sedges())
    out << se;
  std::cout << "\n";

  return out;
}

// Grammar ----------------------------------------------------------------------------------------

sbg_parser::sbg_parser() : sbg_parser::base_type(sbg) 
{ 
  ident = qi::lexeme[qi::alpha >> *qi::alnum];

  constant_def %= ident >> '=' >> qi::int_ >> ';';

  literal %= qi::int_ | ident; 

  binop %= literal >> op_symbols >> expr; 

  expr %= binop | literal;

  inter %= expr
    >> ':' >> expr 
    >> ':' >> expr;

  multi_inter = '['
    >> inter [phx::bind(&MultiInterval::addInter, qi::_val, qi::_1)] 
    >> *(',' >> inter [phx::bind(&MultiInterval::addInter, qi::_val, qi::_1)])
    >> ']';

  set = '{'
    >> ((multi_inter               [phx::bind(&Set::addAtomSet, qi::_val, qi::_1)] 
           >> *(',' >> multi_inter [phx::bind(&Set::addAtomSet, qi::_val, qi::_1)]))
        | qi::eps)
    >> '}';

  vertex %= ident >> set >> ';';

  edge %= ident >> multi_inter >> '-' >> ident >> multi_inter >> ';';  

  sbg %= keywords >> ';' >> *constant_def >> *vertex >> *edge;
};

} // namespace Parser
