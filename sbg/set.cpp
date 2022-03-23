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

#include <iostream>

#include <boost/foreach.hpp>

#include <sbg/set.hpp>

namespace SBG {

// Implementation 1: Non canonic sets -------------------------------------------------------------

#define AS_TYPE1                  \
   typename SET_TEMP_TYPE1::AtomSets

SET_TEMPLATE1
SET_TEMP_TYPE1::SetImp1() : ndim_(0), asets_() {}

SET_TEMPLATE1
SET_TEMP_TYPE1::SetImp1(MI_IMP as) : ndim_(as.ndim()) { asets_ref().insert(as); }

SET_TEMPLATE1
SET_TEMP_TYPE1::SetImp1(AtomSets asets)
{
  if (!asets.empty()) {
    int dim1 = (*(asets.begin())).ndim();
    bool equalDims = true;
    // Check if all atomic sets have the same dimension
    BOOST_FOREACH (MI_IMP as, asets) 
      if (dim1 != as.ndim()) equalDims = false;

    if (equalDims && dim1 != 0) {
      asets_ = asets;
      ndim_ = dim1;
    }

    else {
      AtomSets emptyASets;
      asets_ = emptyASets;
      ndim_ = 0;
    }
  }

  else {
    asets_ = asets;
    ndim_ = 0;
  }
}

SET_TEMPLATE1
SET_TEMP_TYPE1::SetImp1(ORD_CT<INT> v)
{
  AtomSets asets;
  MI_IMP as(v);

  if (!v.empty())
    asets.insert(asets.begin(), as);

  asets_ = asets;
  ndim_ = as.ndim();
}

member_imp_temp(SET_TEMPLATE1, SET_TEMP_TYPE1, AS_TYPE1, asets);
member_imp_temp(SET_TEMPLATE1, SET_TEMP_TYPE1, int, ndim);

SET_TEMPLATE1
void SET_TEMP_TYPE1::addAtomSet(MI_IMP aset2)
{
  if (!aset2.empty() && aset2.ndim() == ndim() && !empty())
    asets_.insert(aset2);

  else if (!aset2.empty() && empty()) {
    asets_.insert(aset2);
    ndim_ = aset2.ndim();
  }
}

SET_TEMPLATE1
void SET_TEMP_TYPE1::addAtomSets(AS_TYPE1 sets2)
{
  BOOST_FOREACH (MI_IMP as, sets2)
    addAtomSet(as);
}

SET_TEMPLATE1
bool SET_TEMP_TYPE1::empty()
{
  if (asets_ref().empty()) return true;

  bool res = true;
  BOOST_FOREACH (MI_IMP as, asets()) {
    if (!as.empty()) res = false;
  }

  return res;
}

SET_TEMPLATE1
bool SET_TEMP_TYPE1::isIn(ORD_CT<INT_IMP> elem)
{
  BOOST_FOREACH (MI_IMP as, asets()) 
    if (as.isIn(elem)) return true;

  return false;
}

SET_TEMPLATE1
int SET_TEMP_TYPE1::card()
{
  int res = 0;

  BOOST_FOREACH (MI_IMP as, asets())
    res += as.card();

  return res;
}

SET_TEMPLATE1 
bool SET_TEMP_TYPE1::subseteq(SET_TEMP_TYPE1 set2)
{
  SetImp1 sdiff = (*this).diff(set2);

  if (sdiff.empty()) return true;

  return false;
}

SET_TEMPLATE1
bool SET_TEMP_TYPE1::subset(SET_TEMP_TYPE1 set2)
{
  SetImp1 sdiff1 = (*this).diff(set2);
  SetImp1 sdiff2 = set2.diff(*this);

  if (sdiff1.empty() && !sdiff2.empty()) return true;

  return false;
}

SET_TEMPLATE1
SET_TEMP_TYPE1 SET_TEMP_TYPE1::cap(SET_TEMP_TYPE1 set2)
{
  MI_IMP aux1, aux2;

  if (empty() || set2.empty()) {
    SetImp1 emptyRes;
    return emptyRes;
  }

  AtomSets res;

  BOOST_FOREACH (MI_IMP as1, asets()) {
    BOOST_FOREACH (MI_IMP as2, set2.asets()) {
      MI_IMP capres = as1.cap(as2);

      if (!capres.empty()) res.insert(capres);
    }
  }

  return SetImp1(res);
}

SET_TEMPLATE1
SET_TEMP_TYPE1 SET_TEMP_TYPE1::diff(SET_TEMP_TYPE1 set2)
{
  SetImp1 res;
  AtomSets capres = cap(set2).asets();

  if (!capres.empty()) {
    BOOST_FOREACH (MI_IMP as1, asets()) {
      AtomSets aux;
      aux.insert(as1);

      BOOST_FOREACH (MI_IMP as2, capres) {
        SetImp1 newSets;

        BOOST_FOREACH (MI_IMP as3, aux) {
          AtomSets diffres = as3.diff(as2);
          newSets.addAtomSets(diffres);
        }

        aux = newSets.asets();
      }

      res.addAtomSets(aux);
    }
  }

  else
    res.addAtomSets(asets());

  return res;
}

SET_TEMPLATE1
SET_TEMP_TYPE1 SET_TEMP_TYPE1::cup(SET_TEMP_TYPE1 set2)
{
  SetImp1 res = *this;
  SetImp1 aux = set2.diff(*this);

  if (!aux.empty()) res.addAtomSets(aux.asets());

  return res;
}

SET_TEMPLATE1
ORD_CT<INT_IMP> SET_TEMP_TYPE1::minElem()
{
  ORD_CT<INT_IMP> res;

  if (empty()) return res;

  MI_IMP min = *(asets_ref().begin());

  BOOST_FOREACH (MI_IMP as1, asets()) 
    if (as1.minElem() < min.minElem()) min = as1;

  return min.minElem();
} 

SET_TEMPLATE1
ORD_CT<INT_IMP> SET_TEMP_TYPE1::maxElem()
{
  ORD_CT<INT_IMP> res;

  if (empty()) return res;

  MI_IMP max = *(asets_ref().begin());

  BOOST_FOREACH (MI_IMP as1, asets()) 
    if (max.maxElem() < as1.maxElem()) max = as1;

  return max.maxElem();
} 

SET_TEMPLATE1
SET_TEMP_TYPE1 SET_TEMP_TYPE1::normalize()
{
  UNORD_CT<MI_IMP> res = asets();
  UNORD_CT<MI_IMP> toInsert, toDelete;

  UNORD_CT<MI_IMP> empty;
  
  do {
    bool first = true;
    toInsert = empty;
    toDelete = empty;
 
    SetImp1 aux(res);
    BOOST_FOREACH (MI_IMP as1, res) {
      BOOST_FOREACH (MI_IMP as2, res) {
        MI_IMP normalized = as1.normalize(as2);

        if (!normalized.empty() && first && as1 != as2) {
          toInsert.insert(normalized);
          toDelete.insert(as1);
          toDelete.insert(as2);
 
          first = false;
        }
      }
    }

    BOOST_FOREACH (MI_IMP ins, toInsert) 
      res.insert(ins);

    BOOST_FOREACH (MI_IMP del, toDelete) 
      res.erase(del);
  }
  while (!toDelete.empty());

  return SetImp1(res);
}

SET_TEMPLATE1
SET_TEMP_TYPE1 SET_TEMP_TYPE1::crossProd(SET_TEMP_TYPE1 set2)
{
  AtomSets res;

  BOOST_FOREACH (MI_IMP as1, asets()) {
    BOOST_FOREACH (MI_IMP as2, set2.asets()) {
      MI_IMP auxres = as1.crossProd(as2);
      res.insert(auxres);
    }
  }

  return SetImp1(res);
}

SET_TEMPLATE1
bool SET_TEMP_TYPE1::operator==(const SET_TEMP_TYPE1 &other) const
{
  SetImp1 aux1 = *this;
  SetImp1 aux2 = other;
  SetImp1 diff1 = aux1.diff(aux2);
  SetImp1 diff2 = aux2.diff(aux1);

  if (diff1.empty() && diff2.empty())
    return true;

  return false; 
}

SET_TEMPLATE1
bool SET_TEMP_TYPE1::operator!=(const SET_TEMP_TYPE1 &other) const
{
  return !(*this == other); 
}

SET_TEMPLATE1
bool SET_TEMP_TYPE1::operator<(const SET_TEMP_TYPE1 &other) const 
{
  SetImp1 aux1 = *this;
  SetImp1 aux2 = other;

  if (!aux1.empty() && !aux2.empty()) {
    MI_IMP minAs1 = *(aux1.asets_ref().begin());

    BOOST_FOREACH (MI_IMP as, aux1.asets())
      if (as < minAs1) minAs1 = as;

    MI_IMP minAs2 = *(aux2.asets_ref().begin());

    BOOST_FOREACH (MI_IMP as, aux2.asets())
      if (as < minAs2) minAs2 = as;

    return minAs1 < minAs2;
  }

  return false;
}

SET_TEMPLATE1
size_t SET_TEMP_TYPE1::hash()
{
  size_t seed = 0;
  boost::hash_combine(seed, asets_ref().size());
  return seed; 
}

template struct SetImp1<OrdCT, UnordCT, MultiInterval, INT>;

SET_TEMPLATE1
std::ostream &operator<<(std::ostream &out, const SET_TEMP_TYPE1 &set)
{
  UNORD_CT<MI_IMP> asets = set.asets();
  typename UNORD_CT<MI_IMP>::iterator it = asets.begin();
  MI_IMP aux;

  if (asets.size() == 0) {
    out << "{}";
    return out;
  }

  if (asets.size() == 1) {
    aux = *it;
    out << "{" << aux << "}";
    return out;
  }

  aux = *it;
  out << "{" << aux;
  ++it;
  while (std::next(it, 1) != asets.end()) {
    aux = *it;
    out << ", " << aux;
    ++it;
  }
  aux = *it;
  out << ", " << aux << "}";

  return out;
}


// Implementation 2: Canonic Sets -----------------------------------------------------------------

#define AS_TYPE2                  \
   typename SET_TEMP_TYPE2::AtomSets

SET_TEMPLATE2
SET_TEMP_TYPE2::SetImp2() : ndim_(0), asets_() {}

SET_TEMPLATE2
SET_TEMP_TYPE2::SetImp2(MI_IMP as) : ndim_(as.ndim())
{
  AtomSets empty;
  empty.insert(as);
  asets_ = empty;
}

SET_TEMPLATE2
SET_TEMP_TYPE2::SetImp2(AtomSets asets)
{
  if (!asets.empty()) {
    int dim1 = (*(asets.begin())).ndim();
    bool equalDims = true;
    // Check if all atomic sets have the same dimension
    BOOST_FOREACH (MI_IMP as, asets) 
      if (dim1 != as.ndim()) equalDims = false;

    if (equalDims && dim1 != 0) {
      AtomSets aux;

      BOOST_FOREACH (MI_IMP as, asets)
        aux.insert(as);

      asets_ = aux;
      ndim_ = dim1;
    }

    else {
      AtomSets emptyASets;
      asets_ = emptyASets;
      ndim_ = 0;
    }
  }

  else {
    asets_ = asets;
    ndim_ = 0;
  }
}

SET_TEMPLATE2
SET_TEMP_TYPE2::SetImp2(UNORD_CT<MI_IMP> asets)
{
  if (!asets.empty()) {
    int dim1 = (*(asets.begin())).ndim();
    bool equalDims = true;
    // Check if all atomic sets have the same dimension
    BOOST_FOREACH (MI_IMP as, asets) 
      if (dim1 != as.ndim()) equalDims = false;

    if (equalDims && dim1 != 0) {
      AtomSets aux;

      BOOST_FOREACH (MI_IMP as, asets)
        aux.insert(as);

      asets_ = aux;
      ndim_ = dim1;
    }

    else {
      AtomSets emptyASets;
      asets_ = emptyASets;
      ndim_ = 0;
    }
  }

  else {
    AtomSets aux;

    BOOST_FOREACH (MI_IMP as, asets)
      aux.insert(as);

    asets_ = aux;
    ndim_ = 0;
  }
}

SET_TEMPLATE2
SET_TEMP_TYPE2::SetImp2(ORD_CT<INT> v)
{
  AtomSets asets;
  MI_IMP as(v);

  if (!v.empty())
    asets.insert(as);

  asets_ = asets;
  ndim_ = as.ndim();
}

member_imp_temp(SET_TEMPLATE2, SET_TEMP_TYPE2, AS_TYPE2, asets);
member_imp_temp(SET_TEMPLATE2, SET_TEMP_TYPE2, int, ndim);

SET_TEMPLATE2
void SET_TEMP_TYPE2::addAtomSet(MI_IMP aset2)
{
  if (!aset2.empty() && aset2.ndim() == ndim() && !empty())
    asets_.insert(aset2);

  else if (!aset2.empty() && empty()) {
    asets_.insert(aset2);
    ndim_ = aset2.ndim();
  }
}

SET_TEMPLATE2
void SET_TEMP_TYPE2::addAtomSets(AS_TYPE2 sets2)
{
  BOOST_FOREACH (MI_IMP as, sets2)
    addAtomSet(as);
}

SET_TEMPLATE2
bool SET_TEMP_TYPE2::empty()
{
  if (asets_ref().empty()) return true;

  bool res = true;
  BOOST_FOREACH (MI_IMP as, asets()) {
    if (!as.empty()) res = false;
  }

  return res;
}

SET_TEMPLATE2
bool SET_TEMP_TYPE2::isIn(ORD_CT<INT_IMP> elem)
{
  BOOST_FOREACH (MI_IMP as, asets()) 
    if (as.isIn(elem)) return true;

  return false;
}

SET_TEMPLATE2
int SET_TEMP_TYPE2::card()
{
  int res = 0;

  BOOST_FOREACH (MI_IMP as, asets())
    res += as.card();

  return res;
}

SET_TEMPLATE2 
bool SET_TEMP_TYPE2::subseteq(SET_TEMP_TYPE2 set2)
{
  SetImp2 sdiff = (*this).diff(set2);

  if (sdiff.empty()) return true;

  return false;
}

SET_TEMPLATE2
bool SET_TEMP_TYPE2::subset(SET_TEMP_TYPE2 set2)
{
  SetImp2 sdiff1 = (*this).diff(set2);
  SetImp2 sdiff2 = set2.diff(*this);

  if (sdiff1.empty() && !sdiff2.empty()) return true;

  return false;
}

SET_TEMPLATE2
SET_TEMP_TYPE2 SET_TEMP_TYPE2::cap(SET_TEMP_TYPE2 set2)
{
  MI_IMP aux1, aux2;

  if (empty() || set2.empty()) {
    SetImp2 emptyRes;
    return emptyRes;
  }

  AtomSets res;

  BOOST_FOREACH (MI_IMP as1, asets()) {
    BOOST_FOREACH (MI_IMP as2, set2.asets()) {
      MI_IMP capres = as1.cap(as2);

      if (!capres.empty()) res.insert(capres);
    }
  }

  return SetImp2(res);
}

SET_TEMPLATE2
SET_TEMP_TYPE2 SET_TEMP_TYPE2::diff(SET_TEMP_TYPE2 set2)
{
  SetImp2 res;
  AtomSets capres = cap(set2).asets();

  if (!capres.empty()) {
    BOOST_FOREACH (MI_IMP as1, asets()) {
      AtomSets aux;
      aux.insert(as1);

      BOOST_FOREACH (MI_IMP as2, capres) {
        SetImp2 newSets;

        BOOST_FOREACH (MI_IMP as3, aux) {
          UnordAtomSets diffres = as3.diff(as2);
          BOOST_FOREACH (MI_IMP d, diffres)
            newSets.addAtomSet(d);
            //newSets.addAtomSets(diffres);
        }

        aux = newSets.asets();
      }

      res.addAtomSets(aux);
    }
  }

  else
    res.addAtomSets(asets());

  return res;
}

SET_TEMPLATE2
SET_TEMP_TYPE2 SET_TEMP_TYPE2::cup(SET_TEMP_TYPE2 set2)
{
  SetImp2 res = *this;
  SetImp2 aux = set2.diff(*this);

  if (!aux.empty()) res.addAtomSets(aux.asets());

  return res;
}

SET_TEMPLATE2
ORD_CT<INT_IMP> SET_TEMP_TYPE2::minElem()
{
  ORD_CT<INT_IMP> res;

  if (empty()) return res;

  MI_IMP min = *(asets_ref().begin());

  BOOST_FOREACH (MI_IMP as1, asets()) 
    if (as1.minElem() < min.minElem()) min = as1;

  return min.minElem();
} 

SET_TEMPLATE2
ORD_CT<INT_IMP> SET_TEMP_TYPE2::nextElem(ORD_CT<INT_IMP> cur)
{
  ORD_CT<INT_IMP> res;

  if (empty()) return res;
  assert(asets().size() == 1);

  MI_IMP aset = *(asets_ref().begin());

  return aset.nextElem(cur);
}

SET_TEMPLATE2
ORD_CT<INT_IMP> SET_TEMP_TYPE2::maxElem()
{
  ORD_CT<INT_IMP> res;

  if (empty()) return res;

  MI_IMP max = *(asets_ref().begin());

  BOOST_FOREACH (MI_IMP as1, asets()) 
    if (max.maxElem() < as1.maxElem()) max = as1;

  return max.maxElem();
} 

SET_TEMPLATE2
SET_TEMP_TYPE2 SET_TEMP_TYPE2::normalize()
{
  UNIQUE_ORD_CT<MI_IMP> res = asets();
  UNIQUE_ORD_CT<MI_IMP> toInsert, toDelete;

  UNIQUE_ORD_CT<MI_IMP> empty;
  
  do {
    bool first = true;
    toInsert = empty;
    toDelete = empty;
 
    SetImp2 aux(res);
    BOOST_FOREACH (MI_IMP as1, res) {
      BOOST_FOREACH (MI_IMP as2, res) {
        MI_IMP normalized = as1.normalize(as2);

        if (!normalized.empty() && first && as1 != as2) {
          toInsert.insert(normalized);
          toDelete.insert(as1);
          toDelete.insert(as2);
 
          first = false;
        }
      }
    }

    BOOST_FOREACH (MI_IMP ins, toInsert) 
      res.insert(ins);

    BOOST_FOREACH (MI_IMP del, toDelete) 
      res.erase(del);
  }
  while (!toDelete.empty());

  return SetImp2(res);
}

SET_TEMPLATE2
SET_TEMP_TYPE2 SET_TEMP_TYPE2::crossProd(SET_TEMP_TYPE2 set2)
{
  AtomSets res;

  BOOST_FOREACH (MI_IMP as1, asets()) {
    BOOST_FOREACH (MI_IMP as2, set2.asets()) {
      MI_IMP auxres = as1.crossProd(as2);
      res.insert(auxres);
    }
  }

  return SetImp2(res);
}

SET_TEMPLATE2
UNIQUE_ORD_CT<SET_TEMP_TYPE2> SET_TEMP_TYPE2::atomize()
{
  UNIQUE_ORD_CT<SET_TEMP_TYPE2> atomize;

  for (MI_IMP as : asets()) {
    SetImp2 s(as);
    atomize.insert(s);
  }

  return atomize;
}

SET_TEMPLATE2
bool SET_TEMP_TYPE2::operator==(const SET_TEMP_TYPE2 &other) const
{
  SetImp2 aux1 = *this;
  SetImp2 aux2 = other;
  SetImp2 diff1 = aux1.diff(aux2);
  SetImp2 diff2 = aux2.diff(aux1);

  if (diff1.empty() && diff2.empty())
    return true;

  return false; 
}

SET_TEMPLATE2
bool SET_TEMP_TYPE2::operator!=(const SET_TEMP_TYPE2 &other) const
{
  return !(*this == other); 
}

SET_TEMPLATE2
bool SET_TEMP_TYPE2::operator<(const SET_TEMP_TYPE2 &other) const 
{
  SetImp2 aux1 = *this;
  SetImp2 aux2 = other;

  if (!aux1.empty() && !aux2.empty()) {
    MI_IMP minAs1 = *(aux1.asets_ref().begin());

    BOOST_FOREACH (MI_IMP as, aux1.asets())
      if (as < minAs1) minAs1 = as;

    MI_IMP minAs2 = *(aux2.asets_ref().begin());

    BOOST_FOREACH (MI_IMP as, aux2.asets())
      if (as < minAs2) minAs2 = as;

    return minAs1 < minAs2;
  }

  return false;
}

SET_TEMPLATE2
size_t SET_TEMP_TYPE2::hash()
{
  size_t seed = 0;
  boost::hash_combine(seed, asets_ref().size());
  return seed; 
}

template struct SetImp2<OrdCT, UniqueOrdCT, UnordCT, MultiInterval, INT>;

SET_TEMPLATE2
std::ostream &operator<<(std::ostream &out, const SET_TEMP_TYPE2 &set)
{
  UNIQUE_ORD_CT<MI_IMP> asets = set.asets();
  typename UNIQUE_ORD_CT<MI_IMP>::iterator it = asets.begin();
  MI_IMP aux;

  if (asets.size() == 0) {
    out << "{}";
    return out;
  }

  if (asets.size() == 1) {
    aux = *it;
    out << "{" << aux << "}";
    return out;
  }

  aux = *it;
  out << "{" << aux;
  ++it;
  while (std::next(it, 1) != asets.end()) {
    aux = *it;
    out << ", " << aux;
    ++it;
  }
  aux = *it;
  out << ", " << aux << "}";

  return out;
}

// Sets -------------------------------------------------------------------------------------------

template std::ostream &operator<<(std::ostream &out, const Set &set);

size_t hash_value(const Set &set) { 
  Set aux = set;
  return aux.hash(); 
}

Set createSet(Interval i)
{
  MultiInterval mi;
  mi.addInter(i);
  Set s;
  s.addAtomSet(mi);

  return s;
}

Set createSet(MultiInterval mi)
{
  Set s;
  s.addAtomSet(mi);

  return s;
}

}  // namespace SBG
