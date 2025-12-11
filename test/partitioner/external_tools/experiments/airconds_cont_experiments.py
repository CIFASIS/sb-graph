# Airconds Controller example to get partition results using classic partitioners.

# General configuration to access qss-sover methods (the solver needs to be installed first). 
# libconf is needded as an external dep, run: `pip install libconf` first.`

import argparse
import os
import shutil
import sys

MMOC_SRC = os.environ['MMOC_SRC']
MMOC_OUTPUT = os.environ['MMOC_OUTPUT']
MMOC_BUILD = os.environ['MMOC_BUILD']
sys.path.append(MMOC_SRC+'/python')
sys.path.append(MMOC_SRC+'/python/qss_solver')

import qss_solver
import qss_solver.results as solver_results
import qss_solver.model as solver_model
import qss_solver.simulate as solver_sim


def generate_section_file(path, size, sections):
    airs_per_section = size / sections
    sections_file_code = 'int getSection(int i) {'+'\n int ret = ((i-1)/'+str(airs_per_section)+')+1;\nreturn (ret);\n}'
    sections_file = path + '/sections.c'
    # Write the modified content back to the file
    with open(sections_file, 'w') as file:
        file.write(sections_file_code)

def run_experiments(path, model_name):
    # Model constants size.
    model_size = [1000, 10000, 100000, 1000000, 10000000]

    # Partitioners
    model_partitioners = ['Metis']

    model_full_path = path+'/'+model_name+'.mo'

    # Just in case we may want to change the number of sections.
    sections = [4]

    for s in model_size:
        for section in sections:
            generate_section_file(path, s, int(section))
            for p in model_partitioners:
                # The parameter should be the full path name if not stored in the solver default folders.
                model_constant = solver_model.constants(model_full_path)
                model_constant['N'] = s
                solver_model.set_constants(model_full_path, model_constant)
                model_annotations = solver_model.annotations(model_full_path)
                model_annotations['MMO_PartitionMethod'] = p
                # Store the number of LPS (partitions) we are using.
                lps = model_annotations['MMO_LPS']
                solver_model.set_annotations(model_full_path, model_annotations)

                # Copy the generated sections C file to build folder.
                sections_file = path + '/sections.c'
                build_airconds_cont_folder = MMOC_BUILD + '/'+model_name
                print(build_airconds_cont_folder)
                if not os.path.exists(build_airconds_cont_folder):
                    os.makedirs(build_airconds_cont_folder)
                shutil.copy(sections_file, build_airconds_cont_folder)        

                # Compile the model to generate the excecutabe ini file.
                solver_sim.compile_model(model_full_path)
                
                # Now update the ini file to set the 'partitionOnly' flag to avoid running the simulation. 
                model_config = solver_model.config(model_name)
                model_config['partitionOnly'] = 1
                solver_model.set_config(model_name, model_config)

                # Now go ahead and execute the model. 
                solver_sim.execute_model(model_full_path)

                # Finally, get the partitioner stats into a dict. 
                model_full_size = 3 * s + 5 + 2 * int(section)
                partitioner_log_file = model_name+'-'+str(model_full_size)+'-'+lps+'-partition-stats.log' 
                partition_log = solver_results.simulation_log(MMOC_OUTPUT+'/'+model_name+'/'+partitioner_log_file)
                print('Partition method: '+ p)
                print('Model size: ' + str(s))
                print('Results')
                print(partition_log)


# Example usage: python3 ./airconds_experimets.py /home/joaquin/work/sbg-partitioner/src/external_tools/experiments/airconds_cont airconds_cont

def main():
    parser = argparse.ArgumentParser(description='Run experiments for classic partitioners.')
    parser.add_argument('path', type=str, help='Full path to the folder where the airconds model .mo file is located.')
    parser.add_argument('model_name', type=str, help='Model name whitout .mo extension.')

    args = parser.parse_args()

    run_experiments(args.path, args.model_name)

if __name__ == '__main__':
    main()
