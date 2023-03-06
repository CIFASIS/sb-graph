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
#include <parser/converter.hpp>

namespace Parser {

// Visitors ---------------------------------------------------------------------------------------

LitVisitor::LitVisitor(ConstantsEnv cenv) : cenv_(cenv) {}

member_imp(LitVisitor, ConstantsEnv, cenv);

SBG::INT LitVisitor::operator()(SBG::INT i) const
{
  return i;
}

SBG::INT LitVisitor::operator()(std::string identifier) const
{
  return cenv()[identifier];
}

ExprVisitor::ExprVisitor(ConstantsEnv cenv) : cenv_(cenv) {}

member_imp(ExprVisitor, ConstantsEnv, cenv);

SBG::INT ExprVisitor::operator()(Parser::Literal l) const
{
  LitVisitor lv(cenv()); 

  return boost::apply_visitor(lv, l);
}

SBG::INT ExprVisitor::operator()(Parser::BinOp bop) const
{
  SBG::INT res = 0;

  SBG::INT l = boost::apply_visitor(*this, bop.left());
  SBG::INT r = boost::apply_visitor(*this, bop.right());

  switch (bop.op()) {
    case Parser::Op::add:
      res =  l + r;
      break;

    case Parser::Op::mult:
      res =  l * r;
      break;

    case Parser::Op::sub:
      res = l - r;
      break;
  }

  return res;
}

ConstVisitor::ConstVisitor() {}

bool ConstVisitor::operator()(std::string var) const { return false; }

bool ConstVisitor::operator()(SBG::INT i) const { return true; }

bool ConstVisitor::operator()(Parser::Literal l) const 
{
  return boost::apply_visitor(*this, l);
}

bool ConstVisitor::operator()(Parser::BinOp bop) const { return false; }

VarVisitor::VarVisitor() {}

bool VarVisitor::operator()(std::string var) const { return true; }

bool VarVisitor::operator()(SBG::INT i) const { return false; }

bool VarVisitor::operator()(Parser::Literal l) const
{
  return boost::apply_visitor(*this, l);
}

bool VarVisitor::operator()(Parser::BinOp bop) const { return false; }

MultVisitor::MultVisitor() {}

bool MultVisitor::operator()(Parser::Literal l) const { return true; }

bool MultVisitor::operator()(Parser::BinOp bop) const 
{
  bool res = false;

  switch (bop.op()) {
    case Parser::Op::mult:
    {
      Expr l = bop.left(), r = bop.right();
      bool lcond = boost::apply_visitor(ConstVisitor(), bop.left());
      bool rcond = boost::apply_visitor(VarVisitor(), bop.right());
      res = lcond && rcond; 
      break;
    }

    default:
      res = false;
  }

  return res;
}

LExprVisitor::LExprVisitor() {}

bool LExprVisitor::operator()(Parser::Literal l) const { return true; }

bool LExprVisitor::operator()(Parser::BinOp bop) const
{
  bool res = false;

  switch (bop.op()) {
    case Parser::Op::add:
    {
      bool lcond = boost::apply_visitor(MultVisitor(), bop.left());
      bool rcond = boost::apply_visitor(ConstVisitor(), bop.right());
      res = lcond && rcond;
      break;
    }

    case Parser::Op::mult:
    {
      bool lcond = boost::apply_visitor(ConstVisitor(), bop.left());
      bool rcond = boost::apply_visitor(VarVisitor(), bop.right());
      res = lcond && rcond;
      break;
    }

    case Parser::Op::sub:
    {
      bool lcond = boost::apply_visitor(MultVisitor(), bop.left());
      bool rcond = boost::apply_visitor(ConstVisitor(), bop.right());
      res = lcond && rcond;
      break;
    }

    default:
      res = false;
  }

  return res;
}

SlopeVisitor::SlopeVisitor() {}

SBG::INT SlopeVisitor::operator()(Parser::Literal l) const
{
  SBG::INT result = 1;

  if (l.which() == 1)
    result = boost::get<SBG::INT>(l);

  return result;
}

SBG::INT SlopeVisitor::operator()(Parser::BinOp bop) const
{
  SBG::INT result;
  
  switch (bop.op()) {
    case Parser::Op::add: 
    {
      if (bop.left_ref().which() == 0)
        result = boost::apply_visitor(*this, boost::get<Parser::Literal>(bop.left()));
      else {
        BinOp lbop = boost::get<Parser::BinOp>(bop.left());
        result = boost::apply_visitor(*this, lbop.left());
      }
      break;
    }

    case Parser::Op::mult: 
    {
      result = boost::apply_visitor(*this, bop.left());
      break;
    }

    case Parser::Op::sub:
    {
      BinOp lbop = boost::get<BinOp>(bop.left());
      if (lbop.left_ref().which() == 0)
        result = boost::apply_visitor(*this, bop.left());
      else 
        result = boost::apply_visitor(*this, lbop.left());
      break;
    }

    default:
      result = 1;
  }

  return result;
}

OffsetVisitor::OffsetVisitor() {}

SBG::INT OffsetVisitor::operator()(Parser::Literal l) const
{
  SBG::INT result = 0;

  if (l.which() == 1)
    result = boost::get<SBG::INT>(l);

  return result;
}

SBG::INT OffsetVisitor::operator()(Parser::BinOp bop) const
{
  SBG::INT result;
  
  switch (bop.op()) {
    case Parser::Op::add:
      result = boost::apply_visitor(*this, bop.right());
      break;

    case Parser::Op::mult:
      result = 0;
      break;

    case Parser::Op::sub:
      result = boost::apply_visitor(*this, bop.right());
      break;

    default:
      result = 0;
  }

  return result;
}

// Operations -------------------------------------------------------------------------------------

SBG::INT convertExpr(Parser::Expr e, ConstantsEnv cenv)
{
  ExprVisitor ev(cenv);

  return boost::apply_visitor(ev, e);
}

SBG::Interval convertInterval(Parser::Interval i, ConstantsEnv cenv) 
{ 
  SBG::INT res_lo = convertExpr(i.lo(), cenv);
  SBG::INT res_step = convertExpr(i.step(), cenv);
  SBG::INT res_hi = convertExpr(i.hi(), cenv);

  return SBG::Interval(res_lo, res_step, res_hi); 
}

SBG::MultiInterval convertMI(Parser::MultiInterval mi, ConstantsEnv cenv)
{
  SBG::MultiInterval mi_res;

  BOOST_FOREACH (Parser::Interval i, mi.inters())
    mi_res.addInter(convertInterval(i, cenv));

  return mi_res;
}

SBG::Set convertSet(Parser::Set s, ConstantsEnv cenv)
{
  SBG::Set s_res;

  BOOST_FOREACH (Parser::MultiInterval mi, s.asets())
    s_res.addAtomSet(convertMI(mi, cenv));

  return s_res;
}

SBG::LMap convertPWLExp(PWLExp pwl, ConstantsEnv cenv)
{
  SBG::LMap lm_res;
  ExprVisitor ev(cenv);

  BOOST_FOREACH (Parser::Expr e, pwl) {
    // Check that it's a linear expression
    LExprVisitor lv;
    if (boost::apply_visitor(lv, e)) {
      SBG::INT m = boost::apply_visitor(SlopeVisitor(), e);
      SBG::INT h = boost::apply_visitor(OffsetVisitor(), e);

      lm_res.addGO(m, h); 
    }

    else{
      std::cerr << "ERROR [converter.cpp]: only linear expressions allowed\n";
      return SBG::LMap();
    }
  }

  return lm_res;
}

// Converter --------------------------------------------------------------------------------------

Converter::Converter() : dom_offset_(), sg_() {}

Converter::Converter(Parser::SetGraph sg) : dom_offset_() {
  set_sg(sg);
  sg_ref().createConstantsEnv();

  if (!sg.svertices().empty()) {
    Parser::SetVertex v1 = *(sg.svertices_ref().begin());
    Parser::MultiInterval mi1 = *(v1.range_ref().asets_ref().begin());
    
    for (unsigned int i = 0; i < mi1.inters_ref().size(); i++)
      dom_offset_ref().insert(dom_offset_ref().end(), 1);
  }
}

member_imp(Converter, SBG::OrdCT<SBG::INT>, dom_offset);
member_imp(Converter, Parser::SetGraph, sg);

// Vertices ---------------------------------------------------------------------------------------

SBG::SetVertex Converter::convertVertex(Parser::SetVertex sv) { return SBG::SetVertex(sv.name(), convertSet(sv.range(), sg_ref().cenv())); }

// Edges ------------------------------------------------------------------------------------------

VariantEdge Converter::convertEdge(Parser::SetEdge se)
{
  VariantEdge res;
  ConstantsEnv c = sg_ref().cenv();

  std::string nm = se.lname() + se.rname();
  SBG::Set dom = convertSet(se.dom(), c);
  SBG::LMap l = convertPWLExp(se.left(), c), r = convertPWLExp(se.right(), c);
  SBG::PWLMap lmap, rmap;
  lmap.addSetLM(dom, l);
  rmap.addSetLM(dom, r);

  if (sg_ref().modifier() == "undirected") {
    res = SBG::SetEdge(nm, lmap, rmap);
  }

  else if (sg_ref().modifier() == "directed") {
    res = SBG::DSetEdge(nm, lmap, rmap);
  }

  return res;
}

// Graphs -----------------------------------------------------------------------------------------

SBG::SBGraph Converter::convertUndirectedGraph() 
{
  SBG::SBGraph result;
  VertexMap vm;

  if (!sg_ref().svertices_ref().empty()) {
    // Create vertices
    BOOST_FOREACH (Parser::SetVertex sv, sg().svertices()) {
      SBG::SetVertexDesc vd = boost::add_vertex(result);
      result[vd] = convertVertex(sv);
      vm[sv.name()] = vd;
    }

    // Create edges
    std::vector<std::string> visited;
    BOOST_FOREACH (Parser::SetEdge se, sg().sedges()) { 
      std::string lvertex = se.lname(), rvertex = se.rname();
      auto itl = find(visited.begin(), visited.end(), lvertex);
      auto itr = find(visited.begin(), visited.end(), rvertex);

      if (itl == visited.end() || itr == visited.end()) {
        SBG::SetEdge res_se = boost::get<SBG::SetEdge>(convertEdge(se));
        SBG::PWLMap lmap = res_se.map_f(), rmap = res_se.map_u();

        // Lookup edges from the same set-edge
        BOOST_FOREACH (Parser::SetEdge se2, sg().sedges()) {
          std::string lvertex2 = se2.lname(), rvertex2 = se2.rname();
          SBG::SetEdge res_se2 = boost::get<SBG::SetEdge>(convertEdge(se2));
          SBG::PWLMap lmap2 = res_se2.map_f(), rmap2 = res_se2.map_u();

          bool same_vertices1 = lvertex == lvertex2 && rvertex == rvertex2;
          bool same_vertices2 = lvertex == rvertex2 && rvertex == lvertex2;
          bool same_maps = se.dom() == se2.dom(); 

          if ((same_vertices1 || same_vertices2) && !same_maps) {
            lmap = lmap.concat(lmap2);
            rmap = rmap.concat(rmap2);
          }
        }

        SBG::SetEdgeDesc ed;
        bool b;
        boost::tie(ed, b) = boost::add_edge(vm[lvertex], vm[rvertex], result); 
        result[ed] = SBG::SetEdge(lvertex + rvertex, lmap, rmap);
      }
 
      visited.insert(visited.end(), lvertex);
      visited.insert(visited.end(), rvertex);
    }
  }

  return result;
}

SBG::DSBGraph Converter::convertDirectedGraph() 
{
  SBG::DSBGraph result;
  DVertexMap dvm;

  if (!sg_ref().svertices_ref().empty()) {
    // Create vertices
    BOOST_FOREACH (Parser::SetVertex sv, sg().svertices()) {
      SBG::DSetVertexDesc vd = boost::add_vertex(result);
      result[vd] = convertVertex(sv);
      dvm[sv.name()] = vd;
    }

    // Create edges
    std::vector<std::string> visited;
    BOOST_FOREACH (Parser::SetEdge se, sg().sedges()) { 
      std::string lvertex = se.lname(), rvertex = se.rname();
      auto itl = find(visited.begin(), visited.end(), lvertex);
      auto itr = find(visited.begin(), visited.end(), rvertex);

      if (itl == visited.end() || itr == visited.end()) {
        SBG::DSetEdge res_se = boost::get<SBG::DSetEdge>(convertEdge(se));
        SBG::PWLMap lmap = res_se.map_b(), rmap = res_se.map_d();

        // Lookup edges from the same set-edge
        BOOST_FOREACH (Parser::SetEdge se2, sg().sedges()) {
          std::string lvertex2 = se2.lname(), rvertex2 = se2.rname();
          SBG::DSetEdge res_se2 = boost::get<SBG::DSetEdge>(convertEdge(se2));
          SBG::PWLMap lmap2 = res_se2.map_b(), rmap2 = res_se2.map_d();

          bool same_vertices = lvertex == lvertex2 && rvertex == rvertex2;
          bool same_maps = se.dom() == se2.dom(); 
          if (same_vertices && !same_maps) {
            lmap = lmap.concat(lmap2);
            rmap = rmap.concat(rmap2);
          }
        }

        SBG::DSetEdgeDesc ed;
        bool b;
        boost::tie(ed, b) = boost::add_edge(dvm[lvertex], dvm[rvertex], result); 
        result[ed] = SBG::DSetEdge(lvertex + rvertex, lmap, rmap);
      }
 
      visited.insert(visited.end(), lvertex);
      visited.insert(visited.end(), rvertex);
    }
  }

  return result;
}

Grph Converter::convertGraph()
{
  Grph result;

  if (sg_ref().modifier() == "undirected") {
    result = convertUndirectedGraph();
  }

  else if (sg_ref().modifier() == "directed") {
    result = convertDirectedGraph();
  }
 
  return result;
}

} // namespace Parser
