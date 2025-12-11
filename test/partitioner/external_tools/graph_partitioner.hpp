/*****************************************************************************

 This file is part of SBG Partitioner.

 SBG Partitioner is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 SBG Partitioner is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with SBG Partitioner.  If not, see <http://www.gnu.org/licenses/>.

 *****************************************************************************/

#include <vector>
#include <string>
#include <metis.h>
#include <scotch/scotch.h>

using grp_t = int;

enum class PartitionMethod {
    Metis,
    HMetis,
    Scotch,
    Kahip,
    Unknown
};

struct Partition {
  public:
  std::vector<grp_t> values;

  void resize(grp_t size) { values.resize(size); }
};

class GraphPartitioner {
  public:
  explicit GraphPartitioner(const std::string &name);

  Partition createPartition(const std::string& partition_method, unsigned int partitions);

  static std::string validPartitionMethodsStr();

  private:
  PartitionMethod partitionMethod(const std::string& partition_method) const;

  void generateInputGraph();

  bool endsWithJson();

  void readGraphFromJson();
  void readGraph();

  void savePartitionToFile(const Partition &partition, const std::string& method_name) const;
  void readPartitionFile(const std::string &file_name, Partition &partition) const;

  void partitionUsingMetis(Partition &partition);
  void partitionUsingHMetis(Partition &partition);
  void createHMetisGraphFile(const std::string &file_name);
  void executeKhmetis(const std::string &h_graph_name) const;

  void partitionUsingScotch(Partition &partition);

  void partitionUsingKaHip(Partition &partition);

  std::string _name;
  grp_t _edges;
  grp_t _nbr_parts;
  grp_t _nbr_vtxs;
  double _imbalance;
  
  std::vector<grp_t> _xadj;
  std::vector<grp_t> _adjncy;
  std::vector<grp_t> _vwgt;
  std::vector<grp_t> _ewgt;
};
