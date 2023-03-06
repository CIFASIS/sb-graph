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

BOOST_FUSION_ADAPT_STRUCT(Parser::SetEdge, (std::string, lname_)(Parser::PWLExp, left_)(Parser::Set, dom_)(std::string, rname_)(Parser::PWLExp, right_))

BOOST_FUSION_ADAPT_STRUCT(Parser::SetGraph, (std::string, modifier_)(Parser::ConstantDefs, constants_)(Parser::Vrtcs, svertices_)(Parser::Edges, sedges_))


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
    out << cd << ";\n";

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

bool BinOp::operator==(const BinOp &other) const
{
  return left() == other.left() && op() == other.op() && right() == other.right();
}

bool BinOp::operator<(const BinOp &other) const
{
  std::stringstream cmp_this;
  std::stringstream cmp_other;

  cmp_this << *this;
  cmp_other << other;

  return cmp_this.str() < cmp_other.str();
}

std::ostream &operator<<(std::ostream &out, const BinOp &bop)
{ 
  out << bop.left() << bop.op() << bop.right();

  return out;
}

// ------------------------------------------------------------------------------------------------

Interval::Interval() : lo_(), step_(), hi_() {}
Interval::Interval(Expr lo, Expr step, Expr hi) : lo_(lo), step_(step), hi_(hi) {}

member_imp(Parser::Interval, Expr, lo);
member_imp(Parser::Interval, Expr, step);
member_imp(Parser::Interval, Expr, hi);

bool Interval::operator==(const Interval &other) const 
{ 
  return lo() == other.lo() && step() == other.step() && hi() == other.hi(); 
}

bool Interval::operator<(const Interval &other) const
{
  return lo() < other.lo();
}

std::ostream &operator<<(std::ostream &out, const Parser::Interval &i)
{
  out << i.lo() << ":" << i.step() << ":" << i.hi();

  return out;
}

MultiInterval::MultiInterval() : inters_() {}
MultiInterval::MultiInterval(Intervals inters) : inters_(inters) {}

member_imp(Parser::MultiInterval, Intervals, inters);

void MultiInterval::addInter(Interval i) { inters_ref().insert(inters_ref().end(), i); }

bool MultiInterval::operator<(const MultiInterval &other) const
{
  MultiInterval aux = *this, aux_other = other;

  parallel_foreach2 (aux.inters_ref(), aux_other.inters_ref()) {
    Interval i = boost::get<0>(items), i_other = boost::get<1>(items);

    if (i.lo() < i_other.lo())
      return true;
  }

  return false;
}

bool MultiInterval::operator==(const MultiInterval &other) const { return inters() == other.inters(); }

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

bool Set::operator==(const Set &other) const { return asets() == other.asets(); }

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
  out << sv.name() << " " << sv.range() << ";\n";

  return out;
}

std::ostream &operator<<(std::ostream &out, const Parser::PWLExp &pwl) 
{
  PWLExp aux = pwl;

  out << "[";
  int sz = aux.size(), i = 0;
  if (sz > 0) {
    for (; i < sz - 1; i++) {
      out << aux[i] << ", ";
    }
    out << aux[i];
  }
  out << "]";

 return out; 
}

SetEdge::SetEdge() : left_(), dom_(), right_() {}
SetEdge::SetEdge(std::string lname, PWLExp left, Set dom, std::string rname, PWLExp right) : lname_(lname), left_(left), dom_(), rname_(rname), right_(right) {}

member_imp(Parser::SetEdge, std::string, lname);
member_imp(Parser::SetEdge, PWLExp, left);
member_imp(Parser::SetEdge, Set, dom);
member_imp(Parser::SetEdge, std::string, rname);
member_imp(Parser::SetEdge, PWLExp, right);

std::ostream &operator<<(std::ostream &out, const SetEdge &se) 
{
  out << se.lname() << se.left() << " " << se.dom() << " " << se.rname() << se.right() << ";\n";

  return out;
}

SetGraph::SetGraph() : modifier_(), constants_(), svertices_(), sedges_(), cenv_() {}

member_imp(Parser::SetGraph, std::string, modifier);
member_imp(Parser::SetGraph, ConstantDefs, constants)
member_imp(Parser::SetGraph, Vrtcs, svertices);
member_imp(Parser::SetGraph, Edges, sedges);
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
  out << "\n";

  BOOST_FOREACH (Parser::SetEdge se, sg.sedges())
    out << se;
  out << "\n";

  return out;
}

// Grammar ----------------------------------------------------------------------------------------

sbg_parser::sbg_parser() : sbg_parser::base_type(sbg) 
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

  set = '{'
    >> ((multi_inter               [phx::bind(&Set::addAtomSet, qi::_val, qi::_1)] 
           >> *(',' >> multi_inter [phx::bind(&Set::addAtomSet, qi::_val, qi::_1)]))
        | qi::eps)
    >> '}';

  vertex %= ident >> set >> ';';

  pw_lexp %= '[' >> expr >> *(',' >> expr) >> ']';

  edge %= ident >> pw_lexp >> set >> ident >> pw_lexp >> ';';

  sbg %= keywords >> ';' >> *constant_def >> *vertex >> *edge;
};

} // namespace Parser
