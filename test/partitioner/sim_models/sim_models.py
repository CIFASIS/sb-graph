#!/usr/bin/env python3
"""
Initial example script that runs `airconds_cont` model which has a parallel config on the sim, 
we should use this as a template to generate the entire pipeline from partition calcaultion to
simulation results.
Extensions:
* Modify the model to allow different sizes.
* Modify the config to setup different number of LPs
* Filter the simulation results.

Requirements:
* Download and install QSS Solver from: https://github.com/CIFASIS/qss-solver/releases/tag/v6.0.2

Run:
python ./sim_models.py 
"""

import glob
import os
import sys
import logging
import re
from pathlib import Path

# Add the qss_solver module to the Python path
# Adjust this path according to your installation
sys.path.insert(0, '/opt/CIFASIS-CONICET/qss-solver/src/python')

try:
    from qss_solver import (
        file_handlers,
        # Simulation functions
        compile_model, execute_model, run,
        # Model manipulation functions
        annotations, set_annotations,
        constants, parameters, set_constant, set_parameter, set_constants, set_parameters,
        config, set_config,
        # Result functions
        simulation_log, output_files
    )
except ImportError as e:
    print(f"Error importing qss_solver modules: {e}")
    print("Make sure the qss_solver package is in your Python path")
    sys.exit(1)

def concatenate_proc_logs_from_dir(directory, model_name, output_filename=None):
    """
    Concatenates files with pattern: {model_name}-{proc_number}.log 
    from a specific input directory.
    """
    # 1. Ensure the directory path is absolute or correctly relative
    directory = os.path.expanduser(directory)
    
    # 2. Define the search pattern within that directory
    search_pattern = os.path.join(directory, f"{model_name}-*.log")
    
    # 3. Find all matching files
    files = glob.glob(search_pattern)
    
    if not files:
        print(f"No log files found for model '{model_name}' in: {directory}")
        return

    # 4. Sort files numerically by the proc_number
    def extract_proc_num(filename):
        # We look for the number at the end of the filename string
        match = re.search(r'-(\d+)\.log$', filename)
        return int(match.group(1)) if match else -1

    files.sort(key=extract_proc_num)

    # 5. Determine output path (defaulting to the same input directory)
    if not output_filename:
        output_filename = os.path.join(directory, f"{model_name}-combined.log")
    else:
        # If a filename is provided without a path, put it in the input dir
        if not os.path.isabs(output_filename) and os.sep not in output_filename:
            output_filename = os.path.join(directory, output_filename)

    try:
        with open(output_filename, 'a') as outfile:
            for fname in files:
                # We skip the output file if it already exists to avoid recursion
                if os.path.abspath(fname) == os.path.abspath(output_filename):
                    continue
                    
                print(f"Appending {os.path.basename(fname)}...")
                with open(fname, 'r') as infile:
                    outfile.write(infile.read())
                    outfile.write("\n")
        
        print(f"\nSuccessfully created: {output_filename}")
        return output_filename

    except Exception as e:
        print(f"An error occurred: {e}")

def setup_environment():
    """
    Configure required environment variables for QSS Solver.
    
    Adjust these paths according to your QSS Solver installation.
    """
    # Base installation directory
    qss_base = Path.home() / "qss-solver"

    # Required environment variables
    env_vars = {
        'MMOC_BIN': os.path.join(qss_base, 'bin'),
        'MMOC_MODELS': os.path.join(qss_base, 'models'),
        'MMOC_OUTPUT': os.path.join(qss_base, 'output'),
        'MMOC_BUILD': os.path.join(qss_base, 'build')
    }
    
    # Set environment variables
    for var, value in env_vars.items():
        os.environ[var] = value
        print(f"Set {var} = {value}")
    
    # Create directories if they don't exist
    for var in ['MMOC_OUTPUT', 'MMOC_BUILD']:
        path = os.environ[var]
        Path(path).mkdir(parents=True, exist_ok=True)
        print(f"Ensured directory exists: {path}")
    
    return env_vars

def configure_logging():
    """Configure logging for detailed output."""
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(levelname)s - %(message)s',
        handlers=[
            logging.StreamHandler(sys.stdout),
            logging.FileHandler('simulation_example.log')
        ]
    )

def clean_parallel_log(file_path):
    try:
        # 1. Read the existing content
        with open(file_path, 'r') as f:
            content = f.read()

        # 2. Apply cleaning logic
        # Remove "Parallel Simulation Statistics:"
        cleaned = content.replace("Parallel Simulation Statistics:", "")
        
        # Remove (DD.DD%) patterns
        cleaned = re.sub(r'\(\d+\.?\d*\s*%\)', '', cleaned)
        
        # Remove specific symbols and units
        cleaned = cleaned.replace("%", "").replace("MBytes", "")
        
        # Clean up whitespace line by line
        final_lines = [line.strip() for line in cleaned.splitlines()]
        final_output = "\n".join(final_lines)

        # 3. Overwrite the original file with cleaned data
        with open(file_path, 'w') as f:
            f.write(final_output)
            
        print(f"Successfully cleaned and updated: {file_path}")

    except FileNotFoundError:
        print(f"Error: The file '{file_path}' was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

def sim_results(log_file):
    clean_parallel_log(log_file)
    print("\nSimulation results:")
    log_data = simulation_log(log_file)
    if log_data:
        for key, value in log_data.items():
            print(f"  {key}: {value}")

def process_model_logs(directory, model_name, proc_func):
    """
    Lists files matching {model_name}-{natural_number}.log and 
    applies processing_func to each file path.
    """
    directory = os.path.expanduser(directory)
    # Pattern to find potential matches
    search_pattern = os.path.join(directory, f"{model_name}-*.log")
    
    # Regex to strictly match {model_name}-{natural_number}.log
    # \d+ ensures it is a natural number (1 or more digits)
    pattern = re.compile(rf"^{re.escape(model_name)}-(\d+)\.log$")
    
    matched_files = []
    
    # 1. List and filter files
    for filepath in glob.glob(search_pattern):
        filename = os.path.basename(filepath)
        if pattern.match(filename):
            matched_files.append(filepath)
            
    # 2. Sort numerically (LP 0, LP 1, LP 2...)
    matched_files.sort(key=lambda x: int(pattern.match(os.path.basename(x)).group(1)))

    if not matched_files:
        print(f"No files found matching the pattern in {directory}")
        return

    # 3. Apply your custom function
    print(f"Found {len(matched_files)} files. Starting processing...")
    for log_path in matched_files:
        proc_func(log_path)
        
    print("All files processed successfully.")

def execute_simulation(model_name):
    """
    Basic example: compile and run a model with default settings.
    
    Args:
        model_name (str): full path to the model.
    """
    print(f"\n=== Basic Simulation Example: {model_name} ===")
    
    model_file = model_name

    # Step 1: Check current model annotations
    print("Current model annotations:")
    current_ann = annotations(model_file)
    if current_ann:
        for key, value in current_ann.items():
            print(f"  {key}: {value}")
    else:
        print("  No annotations found")
    
    # Step 2: Compile the model
    print(f"\nCompiling model: {model_file}")
    if compile_model(model_file):
        print("  Compilation successful")
    else:
        print("  Compilation failed")
        return False
    
    # Step 3: Run the simulation
    print(f"\nRunning simulation: {model_file}")
    if execute_model(model_file):
        print("  Simulation completed successfully")
    else:
        print("  Simulation failed")
        return False
    
    # Step 4: Get simulation results

    model = file_handlers.get_file_name(model_name)
    model_log_path = file_handlers.get_full_path(model, 'MMOC_LOG')    
    sim_results(model_log_path)
    process_model_logs(file_handlers.get_base_path(model_log_path, 'MMOC_LOG'), model, sim_results)    
    return True

def example_advanced_configuration(model_name):
    """
    Advanced example: modify model parameters and annotations before simulation.
    
    Args:
        model_name (str): Name of the Modelica model file (without .mo extension)
    """
    print(f"\n=== Advanced Configuration Example: {model_name} ===")
    
    model_file = f"{model_name}.mo"
    
    # Step 1: Read current model parameters and constants
    print("Current model parameters:")
    try:
        current_params = parameters(model_file)
        if current_params:
            for key, value in current_params.items():
                print(f"  {key}: {value}")
        else:
            print("  No parameters found")
    except Exception as e:
        print(f"  Error reading parameters: {e}")
    
    print("\nCurrent model constants:")
    try:
        current_constants = constants(model_file)
        if current_constants:
            for key, value in current_constants.items():
                print(f"  {key}: {value}")
        else:
            print("  No constants found")
    except Exception as e:
        print(f"  Error reading constants: {e}")
    
    # Step 2: Modify model annotations
    new_annotations = {
        'startTime': 0.0,
        'stopTime': 10.0,
        'tolerance': 1e-8,
        'stepSize': 1e-4
    }
    
    print(f"\nSetting new annotations:")
    for key, value in new_annotations.items():
        print(f"  {key}: {value}")
    
    set_annotations(model_file, new_annotations)
    
    # Step 3: Modify model parameters (example values)
    new_parameters = {
        'k': 2.5,
        'omega': 1.0,
        'damping': 0.1
    }
    
    print(f"\nSetting new parameters:")
    for key, value in new_parameters.items():
        print(f"  {key}: {value}")
    
    try:
        set_parameters(model_file, new_parameters)
    except Exception as e:
        print(f"  Warning: Could not set parameters: {e}")
    
    # Step 4: Run the complete simulation
    print(f"\nRunning complete simulation: {model_file}")
    success = run(model_file, "")  # With optimization flags
    
    if success:
        print("  Simulation completed successfully")
        
        # Step 5: Process results
        log_data = simulation_log(model_name)
        if log_data:
            print("\nPerformance metrics:")
            for key, value in log_data.items():
                print(f"  {key}: {value} ms")
    else:
        print("  Simulation failed")
    
    return success

def example_batch_simulation(model_name, parameter_sets):
    """
    Batch simulation example: run multiple simulations with different parameter sets.
    
    Args:
        model_name (str): Name of the Modelica model file
        parameter_sets (list): List of dictionaries containing parameter sets
    """
    print(f"\n=== Batch Simulation Example: {model_name} ===")
    
    model_file = f"{model_name}.mo"
    results = []
    
    for i, params in enumerate(parameter_sets):
        print(f"\n--- Simulation {i+1}/{len(parameter_sets)} ---")
        print(f"Parameters: {params}")
        
        # Set parameters for this run
        try:
            set_parameters(model_file, params)
        except Exception as e:
            print(f"  Warning: Could not set parameters: {e}")
            continue
        
        # Run simulation
        success = run(model_file)
        
        if success:
            # Collect results
            log_data = simulation_log(model_name)
            result = {
                'run': i+1,
                'parameters': params,
                'simulation_time': log_data.get('Simulation time', 0) if log_data else 0,
                'compilation_time': log_data.get('Compilation time', 0) if log_data else 0
            }
            results.append(result)
            print(f"  Success: Sim time = {result['simulation_time']} ms")
        else:
            print(f"  Failed")
    
    # Summary
    print(f"\n=== Batch Simulation Summary ===")
    print(f"Total runs: {len(parameter_sets)}")
    print(f"Successful runs: {len(results)}")
    
    if results:
        avg_sim_time = sum(r['simulation_time'] for r in results) / len(results)
        avg_comp_time = sum(r['compilation_time'] for r in results) / len(results)
        print(f"Average simulation time: {avg_sim_time:.2f} ms")
        print(f"Average compilation time: {avg_comp_time:.2f} ms")
    
    return results

def main():
    """Main function demonstrating various simulation scenarios."""
    print("QSS Solver Python API - Autonomous Simulation Example")
    print("=" * 60)
    
    # Setup
    configure_logging()
    env_vars = setup_environment()
    
    
    model_name = str(Path.cwd()) + "/airconds_cont/airconds_cont.mo"  
        
    try:
        execute_simulation(model_name)
                
    except Exception as e:
        print(f"Error during simulation: {e}")
        logging.error(f"Simulation error: {e}", exc_info=True)
    
    print("\n" + "=" * 60)
    print("Example completed successfully!")
    print(f"Check the log file: simulation_example.log")

if __name__ == "__main__":
    main()
