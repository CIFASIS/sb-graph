#!/usr/bin/env python3
"""
Tuned simulation pipeline script running the QSS Solver model.
Accepts the model path and the partition filename as arguments to 
dynamically extract sizing configuration and update model constants.

Requirements:
* QSS Solver installed with python bindings available.

Run:
python ./sim_models.py --model ./airconds_cont/airconds_cont.mo --partition airconds_cont_100_.json-sbg-4-1213.partition
"""

import argparse
import glob
import os
import sys
import logging
import re
from pathlib import Path
import shutil

# Add the qss_solver module to the Python path
# Adjust this path according to your installation
sys.path.insert(0, '/opt/CIFASIS-CONICET/qss-solver/src/python')

try:
    from qss_solver import (
        file_handlers,
        compile_model, execute_model, run,
        annotations, set_annotations,
        constants, set_constants, parameters, set_parameters,
        config, set_config,
        simulation_log, output_files
    )
except ImportError as e:
    print(f"Error importing qss_solver modules: {e}")
    print("Make sure the qss_solver package is in your Python path")
    sys.exit(1)


def concatenate_proc_logs_from_dir(directory, model_name, output_filename=None):
    """Concatenates files with pattern: {model_name}-{proc_number}.log"""
    directory = os.path.expanduser(directory)
    search_pattern = os.path.join(directory, f"{model_name}-*.log")
    files = glob.glob(search_pattern)
    
    if not files:
        print(f"No log files found for model '{model_name}' in: {directory}")
        return

    def extract_proc_num(filename):
        match = re.search(r'-(\d+)\.log$', filename)
        return int(match.group(1)) if match else -1

    files.sort(key=extract_proc_num)

    if not output_filename:
        output_filename = os.path.join(directory, f"{model_name}-combined.log")
    else:
        if not os.path.isabs(output_filename) and os.sep not in output_filename:
            output_filename = os.path.join(directory, output_filename)

    try:
        with open(output_filename, 'a') as outfile:
            for fname in files:
                if os.path.abspath(fname) == os.path.abspath(output_filename):
                    continue
                print(f"Appending {os.path.basename(fname)}...")
                with open(fname, 'r') as infile:
                    outfile.write(infile.read())
                    outfile.write("\n")
        
        print(f"\nSuccessfully created: {output_filename}")
        return output_filename
    except Exception as e:
        print(f"An error occurred while concatenating logs: {e}")


def setup_environment():
    """Configure required environment variables for QSS Solver."""
    qss_base = Path.home() / "qss-solver"
    env_vars = {
        'MMOC_BIN': os.path.join(qss_base, 'bin'),
        'MMOC_MODELS': os.path.join(qss_base, 'models'),
        'MMOC_OUTPUT': os.path.join(qss_base, 'output'),
        'MMOC_BUILD': os.path.join(qss_base, 'build')
    }
    
    for var, value in env_vars.items():
        os.environ[var] = value
        print(f"Set {var} = {value}")
    
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
        with open(file_path, 'r') as f:
            content = f.read()

        cleaned = content.replace("Parallel Simulation Statistics:", "")
        cleaned = re.sub(r'\(\d+\.?\d*\s*%\)', '', cleaned)
        cleaned = cleaned.replace("%", "").replace("MBytes", "")
        
        final_lines = [line.strip() for line in cleaned.splitlines()]
        final_output = "\n".join(final_lines)

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
    directory = os.path.expanduser(directory)
    search_pattern = os.path.join(directory, f"{model_name}-*.log")
    pattern = re.compile(rf"^{re.escape(model_name)}-(\d+)\.log$")
    
    matched_files = []
    for filepath in glob.glob(search_pattern):
        filename = os.path.basename(filepath)
        if pattern.match(filename):
            matched_files.append(filepath)
            
    matched_files.sort(key=lambda x: int(pattern.match(os.path.basename(x)).group(1)))

    if not matched_files:
        print(f"No files found matching the pattern in {directory}")
        return

    print(f"Found {len(matched_files)} files. Starting processing...")
    for log_path in matched_files:
        proc_func(log_path)
    print("All files processed successfully.")


def extract_size_from_partition(partition_name):
    """
    Extracts the 'size' value from the partition filename layout:
    model_name_size_.json-method-4-1213.partition
    """
    # Pattern tracks the digits between the model prefix separator and the trailing target items
    partition_p = Path(partition_name).resolve()
    print(str(partition_p.name))
    pattern = r'(\d+)_?\.json-[^-]+-(\d+)-'
    # match = re.search(r'(\d+)_?\.json-', partition_name)
    match = re.search(pattern, str(partition_p))
    if match:
        size = int(match.group(1))
        parts = int(match.group(2))
        return size, parts
    else:
        raise ValueError(
            f"Could not parse size and parts from partition filename layout: '{partition_name}'"
        )
    if match:
        return int(match.group(1))
    else:
        raise ValueError(
            f"Could not parse size variant integer from partition filename layout: '{partition_name}'"
        )


def generate_sections_file(size, partitions, destination):
    """
    Generates the C file and copies it to the specified build directory.
    """
    filename = "sections.c"
    partition_size = size // partitions
    
    c_code = f"""int 
getSection(int i)
{{
    int partition_size = {partition_size};
    int ret = ((i - 1) / partition_size) + 1;
    return (ret);
}}
"""
    try:
        # 1. Write the file
        with open(filename, "w") as f:
            f.write(c_code)
        print(f"Successfully generated '{filename}' with partition_size {partition_size}.")
        
        # 2. Copy the file to the destination
        if not os.path.exists(destination):
            print(f"Error: Destination directory '{destination}' does not exist.")
            return

        shutil.copy(filename, destination)
        print(f"Successfully copied '{filename}' to '{destination}'.")
        
    except IOError as e:
        print(f"An error occurred during file operations: {e}")
    except PermissionError:
        print(f"Permission denied: Unable to write to '{destination}'.")


def execute_simulation(model_path, partition_name):
    """
    Compiles and executes the model dynamically, assigning N size constraints first.
    """
    print(f"\n=== Executing Automated Pipeline for: {model_path} ===")
    
    model_p = Path(model_path).resolve()
    model_file = str(model_p)
    model = file_handlers.get_file_name(model_file)

    partition_p = Path(partition_name).resolve()

    size, parts = extract_size_from_partition(partition_name)
    print(f"Extracted targeted structural size: {size}")
    print(f"Extracted parts: {parts}")

    # Derive target destination (<model_name>.part in the model's directory)
    target_partition_path = model_p.with_suffix('.part')
    build_target_path = new_path = Path(os.environ['MMOC_BUILD']) / model
    print("BUILD TARGET")
    print(build_target_path)

    # Step 1: Copy custom partition to the required solver location
    try:
        print(f"Copying partition: {partition_p.name} -> {target_partition_path}")
        shutil.copy2(Path(partition_name).resolve(), target_partition_path)
        shutil.copy2(target_partition_path, build_target_path)
    except Exception as e:
        print(f"Failed to copy partition file: {e}")
        return False
    
    # Step 1: Parse and adapt Constant N values 
    try:
        
        current_constants = constants(model_file) or {}
        print(f"Current structural Constants: {current_constants}")
        
        # Updating constant N size property
        set_constants(model_file, {"N": size, "SECTIONS" : parts})
        print(f"Successfully set Constant 'N' to: {size}")
        print(f"Successfully set Constant 'SECTIONS' to: {parts}")
        
    except Exception as e:
        print(f"Failed handling structural setup attributes: {e}")
        return False

    # Step 2: Read current model annotations
    print("\nCurrent model annotations:")
    current_ann = annotations(model_file)
    if "MMO_Output" in current_ann:
        del current_ann["MMO_Output"]
    current_ann["MMO_LPS"] = parts
    current_ann["MMO_Parallel"] = "true"
    set_annotations(model_file, current_ann)
    current_ann = annotations(model_file)
    
    if current_ann:
        for key, value in current_ann.items():
            print(f"  {key}: {value}")
    else:
        print("  No annotations found")

    if model == "airconds_cont":
        generate_sections_file(size, parts, build_target_path)

    # Step 3: Compile the model
    print(f"\nCompiling model: {model_file}")
    if compile_model(model_file):
        print("  Compilation successful")
    else:
        print("  Compilation failed")
        return False
    
    # Step 4: Run the simulation
    print(f"\nRunning simulation: {model_file}")
    if execute_model(model_file):
        print("  Simulation completed successfully")
    else:
        print("  Simulation failed")
        return False
    
    # Step 5: Get simulation results
    model_log_path = file_handlers.get_full_path(model, 'MMOC_LOG')    
    sim_results(model_log_path)
    process_model_logs(file_handlers.get_base_path(model_log_path, 'MMOC_LOG'), model, sim_results)    
    return True


def main():
    parser = argparse.ArgumentParser(description="QSS Solver Pipeline Utility")
    parser.add_argument(
        '-m', '--model', 
        type=str, 
        required=True, 
        help="Path layout pointing to target Modelica (.mo) file context"
    )
    parser.add_argument(
        '-p', '--partition', 
        type=str, 
        required=True, 
        help="Full or localized naming structure of the partitioned graph layout instance"
    )
    
    args = parser.parse_args()
    
    print("QSS Solver Python API - Autonomous Simulation Example")
    print("=" * 60)
    
    configure_logging()
    setup_environment()
        
    try:
        execute_simulation(args.model, args.partition)
    except Exception as e:
        print(f"Error during execution pipeline: {e}")
        logging.error(f"Simulation error trace: {e}", exc_info=True)
    
    print("\n" + "=" * 60)
    print("Pipeline execution cycle completed.")


if __name__ == "__main__":
    main()
