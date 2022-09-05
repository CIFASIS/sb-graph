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
#include <boost/foreach.hpp>

#include <sbg/multi_interval.hpp>

namespace SBG {

// MultiIntervals ---------------------------------------------------------------------------------

MI_TEMPLATE
MI_TEMP_TYPE::MultiInterImp1() : ndim_(0), inters_(Intervals()) {}

MI_TEMPLATE
MI_TEMP_TYPE::MultiInterImp1(INTERS_TYPE inters)
{
  set_inters(Intervals());
  set_ndim(0);

  bool areEmptyInters = false;

  BOOST_FOREACH (INTER_IMP i, inters)
    if (i.empty()) areEmptyInters = true;

  if (!areEmptyInters) {
    set_inters(inters);
    set_ndim(inters.size());
  }
}

MI_TEMPLATE
MI_TEMP_TYPE::MultiInterImp1(ORD_CT<INT> v)
{
  set_inters(Intervals());
  set_ndim(0);

  if (v.size() > 0) {
    Intervals inters;
    IntervalsIt itints = inters.begin();

    BOOST_FOREACH (INT i, v) {
      itints = inters.insert(itints, INTER_IMP(i, 1, i));
      ++itints;
    }

    set_inters(inters);
    set_ndim(inters.size());
  }
}

member_imp_temp(MI_TEMPLATE, MI_TEMP_TYPE, INTERS_TYPE, inters);
member_imp_temp(MI_TEMPLATE, MI_TEMP_TYPE, int, ndim);

MI_TEMPLATE
void MI_TEMP_TYPE::addInter(INTER_IMP i)
{
  inters_ref().insert(inters_ref().end(), i);
  set_ndim(ndim() + 1);
}

MI_TEMPLATE
bool MI_TEMP_TYPE::empty()
{ 
  BOOST_FOREACH (INTER_IMP i, inters())
    if (i.empty())
      return true;

  return inters().empty();
}

MI_TEMPLATE
bool MI_TEMP_TYPE::isIn(ORD_CT<INT_IMP> elem)
{
  if ((int)elem.size() != ndim()) return false;

  parallel_foreach2 (elem, inters_ref()) 
    if (!(boost::get<1>(items)).isIn(boost::get<0>(items))) return false;

  return true;
}

MI_TEMPLATE
int MI_TEMP_TYPE::card()
{
  if (inters().empty()) return 0;

  int res = 1;

  BOOST_FOREACH (INTER_IMP i, inters())
    res *= i.card();

  return res;
}

MI_TEMPLATE
MI_TEMP_TYPE MI_TEMP_TYPE::cap(MI_TEMP_TYPE mi2)
{
  MultiInterImp1 res;

  if (empty() || mi2.empty()) return MultiInterImp1();

  if (ndim() == mi2.ndim()) {
    parallel_foreach2 (inters_ref(), mi2.inters_ref()) {
      INTER_IMP capres = boost::get<0>(items).cap(boost::get<1>(items));

      if (capres.empty()) return MultiInterImp1();

      res.addInter(capres);
    }
  }

  return res;
}

// TODO: ver si se puede aprovechar el ordenamiento
MI_TEMPLATE
UNIQUE_ORD_CT<MI_TEMP_TYPE> MI_TEMP_TYPE::diff(MI_TEMP_TYPE mi2)
{
  UNIQUE_ORD_CT<MultiInterImp1> resmis;

  if (ndim() != mi2.ndim()) return resmis;

  if (empty()) return resmis;

  if (mi2.empty()) {
    resmis.insert(*this);
    return resmis;
  }

  MultiInterImp1 capmis = cap(mi2);

  // First MI is contained by the second MI
  if (inters() == capmis.inters()) return resmis;

  // Empty intersection, no need of computation
  if (capmis.empty()) {
    resmis.insert(*this);
    return resmis;
  }

  // Differences of each dimension
  IntervalsIt itcap = capmis.inters_ref().begin();
  ORD_CT<UNIQUE_ORD_CT<INTER_IMP>> diffs;
  typename ORD_CT<UNIQUE_ORD_CT<INTER_IMP>>::iterator itdiffs = diffs.begin();

  parallel_foreach2 (inters_ref(), capmis.inters_ref()) {
    itdiffs = diffs.insert(itdiffs, boost::get<0>(items).diff(boost::get<1>(items)));
    ++itdiffs;
  }

  IntervalsIt it1 = inters_ref().begin();
  ++it1;
  itdiffs = diffs.begin();

  int count = 0;
  // Traverse dimensions
  BOOST_FOREACH (UNIQUE_ORD_CT<INTER_IMP> nthdiff, diffs) {
    // Intervals in the difference of nth dimension
    BOOST_FOREACH (INTER_IMP ith, nthdiff) {
      if (!ith.empty()) {
        Intervals resi;

        itcap = capmis.inters_ref().begin();
        IntervalsIt itcapbeg = itcap;

        // Complete with intersection before nth dimension (so the MIs in the result are disjoint)
        if (count > 0) {
          std::advance(itcap, count);
          resi.insert(resi.begin(), itcapbeg, itcap);
        }

        // Complete nth dimension with interval in the difference
        resi.insert(resi.end(), ith);

        // Complete the rest of dimensions with intervals of the first argument
        resi.insert(resi.end(), it1, inters_ref().end());

        resmis.insert(resmis.end(), MultiInterImp1(resi));
      }
    }

    ++count;
    ++it1;
  }

  return resmis;
}

MI_TEMPLATE
ORD_CT<INT_IMP> MI_TEMP_TYPE::minElem()
{
  ORD_CT<INT_IMP> res;

  BOOST_FOREACH (INTER_IMP i, inters()) {
    if (i.empty()) return ORD_CT<INT_IMP>();

    res.insert(res.end(), i.minElem());
  }

  return res;
}

MI_TEMPLATE
ORD_CT<INT_IMP> MI_TEMP_TYPE::nextElem(ORD_CT<INT_IMP> cur)
{
  ORD_CT<INT_IMP> res;

  parallel_foreach2 (inters_ref(), cur) {
    if (boost::get<0>(items).empty()) return ORD_CT<INT_IMP>();

    res.insert(res.end(), boost::get<0>(items).nextElem(boost::get<1>(items)));
  }

  return res;
}

MI_TEMPLATE
ORD_CT<INT_IMP> MI_TEMP_TYPE::maxElem()
{
  ORD_CT<INT_IMP> res;

  BOOST_FOREACH (INTER_IMP i, inters()) {
    if (i.empty()) return ORD_CT<INT_IMP>();

    res.insert(res.end(), i.maxElem());
  }

  return res;
}

// Two multi-intervals can be normalized only if their intervals are the
// same for each dimension, except for one, in which their respective intervals
// can be normalized
MI_TEMPLATE
MI_TEMP_TYPE MI_TEMP_TYPE::normalize(MI_TEMP_TYPE mi2)
{
  IntervalsIt it1 = inters_ref().begin();
  IntervalsIt it2 = mi2.inters_ref().begin();

  if (*this == mi2) return mi2;

  if (ndim() == mi2.ndim()) {
    int diffCount = 0;  // Count of different intervals through dimensions
    int dimDiff = 0;    // Last dimension with spoted difference
    INTER_IMP i1;
    INTER_IMP i2;
    for (int j = 0; j < ndim(); j++) {
      if (*it1 != *it2) {
        ++diffCount;
        dimDiff = j + 1;
        i1 = *it1;
        i2 = *it2;
      }

      ++it1;
      ++it2;
    }

    if (diffCount == 1) {
      INTER_IMP normalized = i1.normalize(i2);

      if (!normalized.empty()) return mi2.replace(normalized, dimDiff);
    }
  }

  return MultiInterImp1();
}

MI_TEMPLATE
MI_TEMP_TYPE MI_TEMP_TYPE::crossProd(MI_TEMP_TYPE mi2)
{
  Intervals res;

  res.insert(res.end(), inters_ref().begin(), inters_ref().end());
  res.insert(res.end(), mi2.inters_ref().begin(), mi2.inters_ref().end());

  return MultiInterImp1(res);
}

MI_TEMPLATE
MI_TEMP_TYPE MI_TEMP_TYPE::replace(INTER_IMP i, int dim)
{
  Intervals res;
  IntervalsIt itres = res.begin();
  int count = 1;

  BOOST_FOREACH (INTER_IMP ii, inters()) {
    if (dim == count)
      itres = res.insert(itres, i);
    else
      itres = res.insert(itres, ii);

    ++itres;
    ++count;
  }

  return MultiInterImp1(res);
}

MI_TEMPLATE
bool MI_TEMP_TYPE::operator==(const MI_TEMP_TYPE &other) const { return inters() == other.inters(); }

MI_TEMPLATE
bool MI_TEMP_TYPE::operator!=(const MI_TEMP_TYPE &other) const { return inters() != other.inters(); }

MI_TEMPLATE
bool MI_TEMP_TYPE::operator<(const MI_TEMP_TYPE &other) const
{
  MultiInterImp1 aux1 = *this;
  MultiInterImp1 aux2 = other;

  parallel_foreach2 (aux1.inters_ref(), aux2.inters_ref()) {
    auto elem1 = boost::get<0>(items).minElem(), elem2 = boost::get<1>(items).minElem();
    if (elem1 < elem2) return true;

    else if (elem2 < elem1) return false;
  }

  return false;
}

template struct MultiInterImp1<OrdCT, UniqueOrdCT, Interval, INT>;

MI_TEMPLATE
std::ostream &operator<<(std::ostream &out, const MI_TEMP_TYPE &mi)
{
  ORD_CT<INTER_IMP> is = mi.inters();
  typename ORD_CT<INTER_IMP>::iterator it = is.begin();

  if (is.size() == 0) return out;

  if (is.size() == 1) {
    out << *it;
    return out;
  }

  out << *it;
  ++it;
  while (next(it, 1) != is.end()) {
    out << "x" << *it;
    ++it;
  }
  out << "x" << *it;

  return out;
}

template std::ostream &operator<<(std::ostream &out, const MultiInterval &mi);

std::size_t hash_value(const MultiInterval &mi)
{
  std::size_t seed = 0;

  BOOST_FOREACH (Interval i, mi.inters())
    boost::hash_combine(seed, hash_value(i));

  return seed;
}

std::ostream &operator<<(std::ostream &out, const UNIQUE_ORD_MI &mis)
{
  UNIQUE_ORD_MI auxmis = mis;
  MultiInterval mi1 = *(mis.begin());

  out << "{¿";
  if (auxmis.size() == 1)
    out << mi1;

  else if (auxmis.size() > 1) {
    auto itmi = auxmis.begin();
    for (; std::next(itmi) != auxmis.end(); ++itmi)
      out << *itmi << ", ";
    out << *itmi;
  }
  out << "?}";

  return out;
}

std::ostream &operator<<(std::ostream &out, const UNORD_MI &mis)
{
  UNORD_MI auxmis = mis;
  MultiInterval mi1 = *(mis.begin());

  out << "{";
  if (auxmis.size() == 1)
    out << mi1;

  else if (auxmis.size() > 1) {
    auxmis.erase(mi1);

    BOOST_FOREACH (MultiInterval mi, auxmis)
      out << mi << ", ";
    out << mi1;
  }
  out << "}";

  return out;
}

}  // namespace SBG
