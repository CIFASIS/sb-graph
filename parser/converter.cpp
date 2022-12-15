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

/*! This module is in charge of converting the parsing structs into an SBG
*   where the novel algorithms can be used.
*/

Converter::Converter() {
  set_offset(SBG::OrdCT<SBG::INT>());
  set_sg(Parser::SetGraph());
}

Converter::Converter(Parser::SetGraph sg) {
  set_offset(SBG::OrdCT<SBG::INT>());
  set_sg(sg);

  if (!sg.svertices().empty()) {
    SBG::SetVertex v1 = *(sg.svertices().begin());
    
    for (int i = 0; i < v1.range().ndim(); i++)
      offset_ref().insert(offset_ref().end(), 1);
  }
}

member_imp(Converter, SBG::OrdCT<SBG::INT>, offset);
member_imp(Converter, Parser::SetGraph, sg);

SBG::MultiInterval Converter::makeDom(SBG::MultiInterval mi1, SBG::MultiInterval mi2) {
  SBG::MultiInterval result;

  SBG::OrdCT<SBG::INT> max_card;
  SBG::OrdCT<SBG::INT>::iterator it_max = max_card.begin();

  // Get number of elements in each dimension
  parallel_foreach2 (mi1.inters_ref(), mi2.inters_ref()) {
    SBG::INT n1 = boost::get<0>(items).card(), n2 = boost::get<1>(items).card();

    if ((n1 > n2 && n2 == 1) || n1 == n2) {
      it_max = max_card.insert(it_max, n1);
      ++it_max;
    }

    else if (n2 > n1 && n1 == 1) {
      it_max = max_card.insert(it_max, n2);
      ++it_max;
    }

    else
      return SBG::MultiInterval();
  }

  // Apply offset
  SBG::OrdCT<SBG::INT> new_off;
  parallel_foreach2 (offset_ref(), max_card) {
    SBG::INT off = boost::get<0>(items), elems = boost::get<1>(items);
    result.addInter(SBG::Interval(off, 1, off + elems - 1));

    new_off.insert(new_off.end(), off + elems);
  }
  set_offset(new_off);

  return result;
}

SBG::LMap Converter::makeExp(SBG::MultiInterval dom, SBG::MultiInterval mi) {
  SBG::LMap result;

  parallel_foreach2 (dom.inters_ref(), mi.inters_ref()) {
    SBG::Interval id = boost::get<0>(items), i = boost::get<1>(items);
    SBG::INT m = i.step(), h = i.lo() - m * id.lo();

    result.addGO(m, h - 1);
  } 

  return result;
}

SBG::SetEdge Converter::convertEdge(Parser::SetEdge se) {
  SBG::MultiInterval dom = makeDom(se.mb(), se.mf()); 

  SBG::LMap lmb = makeExp(dom, se.mb());
  SBG::LMap lmf = makeExp(dom, se.mf());

  SBG::Set sdom;
  sdom.addAtomSet(dom);

  SBG::PWLMap mapb; 
  mapb.addSetLM(dom, lmb);
  SBG::PWLMap mapf; 
  mapf.addSetLM(dom, lmf);

  return SBG::SetEdge("", 1, mapb, mapf, 0);
}

SBG::SBGraph Converter::convertGraph()
{
  SBG::SBGraph result;

  if (!sg_ref().svertices_ref().empty()) {
    BOOST_FOREACH (SBG::SetVertex sv, sg().svertices()) {
      SBG::SetVertexDesc vd = boost::add_vertex(result);
      result[vd] = sv;
    }

    SBG::SetVertexDesc db = boost::vertex(0, result), df = db;
    BOOST_FOREACH (Parser::SetEdge se, sg().sedges()) {
      SBG::OrdCT<SBG::INT> off1, off2;
      BOOST_FOREACH (SBG::SetVertexDesc vd, boost::vertices(result)) {
        if (result[vd].name() == se.vb()) { 
          db = vd;
          off1 = result[db].range().minElem();
        }

        if (result[vd].name() == se.vf()) {
          df = vd;
          off2 = result[df].range().minElem();
        }
      }

      Parser::SetEdge offseted_se(se.id(), se.vb(), se.mb().offset(off1), se.vf(), se.mf().offset(off2));

      SBG::SetEdgeDesc ed;
      bool b;
      boost::tie(ed, b) = boost::add_edge(db, df, result);        
      result[ed] = convertEdge(offseted_se);
    }
  }

  return result;
}
