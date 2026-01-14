# Spiking Neurons example to get partition results using classic partitioners.

# General configuration to access qss-sover methods (the solver needs to be installed first). 
# libconf is needded as an external dep, run: `pip install libconf` first.`

import argparse
import os
import re
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


def update_generated_c_code(file_path):
    """
    Fix the current bug in the solver that creates wrong expressions for event dependencies in graph merge.

    Args:
        file_path (str): The path to the file to be modified.
    """
    # Read the file content
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Define the lines to be replaced
    old_line1 = 'modelData->HZ[_idx_event4(_d1)][events[_idx_event_4(_d1)]++ = _idx_event_1(_d1-100);'
    new_line1 = 'modelData->HZ[_idx_event4(_d1)][events[_idx_event_4(_d1)]++ = _idx_event_1(_d1+100);'

    # Define the patterns to match the wrong expressions
    pattern = r'modelData->HZ\[_idx_event5\(_d1\)]\[events\[_idx_event_5\(_d1\)]\+\+ = _idx_event_1\(_d1-([0-9]+)\);'
    event_6_pattern = r'modelData->HZ\[_idx_event6\(_d1\)]\[events\[_idx_event_6\(_d1\)]\+\+ = _idx_event_2\(_d1-([0-9]+)\);'

    # Replace the old lines with the new lines
    modified_lines = []
    for line in lines:
        if line.strip() == old_line1:
            modified_lines.append(new_line1 + '\n')
        elif re.match(pattern, line):
            match = re.search(pattern, line)
            c = int(match.group(1))
            if 101 <= c <= 109:
                new_line = re.sub(r'-([0-9]+)', f'+{c}', line)
                modified_lines.append(new_line)
            else:
                modified_lines.append(line)
        elif re.match(event_6_pattern, line):
            match = re.search(event_6_pattern, line)
            c = int(match.group(1))
            if 101 <= c <= 109:
                new_line = re.sub(r'-([0-9]+)', f'+{c}', line)
                modified_lines.append(new_line)
            else:
                modified_lines.append(line)
        else:
            modified_lines.append(line)

    # Write the modified content back to the file
    with open(file_path, 'w') as file:
        file.writelines(modified_lines)

def run_experiments(path, model_name):
    
    # Model constants size.
    model_size = [10000]

    # We won't iterate here, if we need to we can do it. 
    model_random_inputs = [100]

    # Same as the previous model constant.
    model_connection_offset = [100]

    # Partitioners
    model_partitioners = ['Metis', 'Scotch']
    print(path)

    model_full_path = path+'/'+model_name+'.mo'

    for s in model_size:
        for p in model_partitioners:
            # The parameter should be the full path name if not stored in the solver default folders.
            model_constant = solver_model.constants(model_full_path)
            model_constant['N'] = s
            model_constant['M'] = model_random_inputs[0]
            model_constant['OFFSET'] = model_connection_offset[0]

            solver_model.set_constants(model_full_path, model_constant)
            model_annotations = solver_model.annotations(model_full_path)
            model_annotations['MMO_PartitionMethod'] = p
            # Store the number of LPS (partitions) we are using.
            lps = model_annotations['MMO_LPS']
            solver_model.set_annotations(model_full_path, model_annotations)
    
            # Fix current code generation bug before running the model.
            model_c_code = MMOC_BUILD + '/' + model_name + '/' + model_name + '.c'
            update_generated_c_code(model_c_code)

            # Compile the model to generate the excecutabe ini file.
            solver_sim.compile_model(model_full_path)
    
            # Now update the ini file to set the 'partitionOnly' flag to avoid running the simulation. 
            model_config = solver_model.config(model_name)
            model_config['partitionOnly'] = 1
            solver_model.set_config(model_name, model_config)

            # Now go ahead and execute the model. 
            solver_sim.execute_model(model_full_path)

            # Finally, get the partitioner stats into a dict. 
            model_full_size = 6 * s + model_constant['M'] + 2 * (s - model_constant['OFFSET'] - model_constant['CONN'])
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
    parser.add_argument('model_name', type=str, help='Model name whitout .mo extension.')

    args = parser.parse_args()

    run_experiments(args.path, args.model_name)

if __name__ == '__main__':
    main()
