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

#include <getopt.h>
#include <iostream>
#include <vector>

#include "graph_partitioner.hpp"

void usage()
{
  std::cout << "Usage: grap_partitioner -f <file_name> -n <num_partitions> -m <partition_method> -i <imbalance>" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -f <file_name>        SBG input JSON file name or generated graph input." << std::endl;
  std::cout << "  -n <num_partitions>   Number of partitions" << std::endl;
  std::cout << "  -m <partition_method> Partition method, possible values: " << GraphPartitioner::validPartitionMethodsStr() << std::endl;
  std::cout << "  -i <imbalance>        Partition imbalance" << std::endl;
  std::cout << "  -h                    Display this information and exit" << std::endl;
}

void parseArgs(int argc, char* argv[], std::string& sbg_json_input, int& partitions, std::string& partition_method, int& imbalance)
{
  int opt;

  while ((opt = getopt(argc, argv, "f:n:m:i:h")) != -1) {
    switch (opt) {
    case 'f':
      sbg_json_input = optarg;
      break;
    case 'n':
      partitions = std::stoi(optarg);
      break;
    case 'm':
      partition_method = optarg;
      break;
    case 'i':
      imbalance = std::stoi(optarg);
      break;
    default:
      usage();
      exit(EXIT_SUCCESS);
    }
  }

  if (sbg_json_input.empty() || partitions <= 0 || partition_method.empty() || imbalance < 0) {
    usage();
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char* argv[])
{
  std::string json_file_name;
  int partitions;
  std::string partition_method;
  int imbalance;

  parseArgs(argc, argv, json_file_name, partitions, partition_method, imbalance);

  GraphPartitioner partitioner(json_file_name);
  Partition partition = partitioner.createPartition(partition_method, partitions);

  return 0;
}