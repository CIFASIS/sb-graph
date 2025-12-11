# Airconds and Advection examples to get partition results using classic partitioners.

# General configuration to access qss-sover methods (the solver needs to be installed first). 
# libconf is needded as an external dep, run: `pip install libconf` first.`

import argparse
import os
import sys

MMOC_SRC = os.environ['MMOC_SRC']
MMOC_OUTPUT = os.environ['MMOC_OUTPUT']
sys.path.append(MMOC_SRC+'/python')
sys.path.append(MMOC_SRC+'/python/qss_solver')

import qss_solver
import qss_solver.results as solver_results
import qss_solver.model as solver_model
import qss_solver.simulate as solver_sim

def run_experiments(path, model_name):
    # Model constants size.
    model_size = [1000, 10000, 100000, 1000000, 10000000]

    # Partitioners
    model_partitioners = ['Metis', 'Scotch']
    print(path)

    model_full_path = path+'/'+model_name+'.mo'

    for s in model_size:
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
    
            # Compile the model to generate the excecutabe ini file.
            solver_sim.compile_model(model_full_path)
    
            # Now update the ini file to set the 'partitionOnly' flag to avoid running the simulation. 
            model_config = solver_model.config(model_name)
            model_config['partitionOnly'] = 1
            solver_model.set_config(model_name, model_config)

            # Now go ahead and execute the model. 
            solver_sim.execute_model(model_full_path)

            # Finally, get the partitioner stats into a dict. 
            # Full size of the advection model.
            model_full_size = s
            if model_name == 'airconds':
                # Update full size for airconds model.
                model_full_size = s*4
            partitioner_log_file = model_name+'-'+str(model_full_size)+'-'+lps+'-partition-stats.log' 
            partition_log = solver_results.simulation_log(MMOC_OUTPUT+'/'+model_name+'/'+partitioner_log_file)
            print('Partition method: '+ p)
            print('Model size: ' + str(s))
            print('Results')
            print(partition_log)


# Example usage: python3 ./airconds_experimets.py /home/joaquin/work/sbg-partitioner/src/external_tools/experiments/airconds airconds

def main():
    parser = argparse.ArgumentParser(description='Run experiments for classic partitioners.')
    parser.add_argument('path', type=str, help='Full path to the folder where the airconds model .mo file is located.')
    parser.add_argument('model_name', type=str, help='Model name whitout .mo extension. It should be one of \'advection\' or \'airconds\'')

    args = parser.parse_args()

    run_experiments(args.path, args.model_name)

if __name__ == '__main__':
    main()
