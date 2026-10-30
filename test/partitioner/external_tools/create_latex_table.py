import pandas as pd
import os
import sys

def generate_latex_table(csv_file):
    df = pd.read_csv(csv_file)
    
    # We take the first two descriptive columns, Average, and Standard_Deviation
    # Based on your previous files: 
    # col 0: ID (N), col 1: Type, col -2: Average, col -1: Std Dev
    
    latex_lines = []
    
    # Header Construction
    latex_lines.append(r"\begin{table}[ht]")
    latex_lines.append(r"\centering")
    latex_lines.append(r"\caption{Summary of Results: Mean and Standard Deviation}")
    latex_lines.append(r"\label{tab:processed_results}")
    
    # Defining the tabular environment (4 columns: N, Type, Mean, Std)
    latex_lines.append(r"\begin{tabular}{lccc}")
    latex_lines.append(r"\toprule")
    latex_lines.append(r"\begin{tabular}[c]{@{}l@{}}Model \\ Name\end{tabular} & \begin{tabular}[c]{@{}l@{}}Number of \\ Partitions\end{tabular} & \begin{tabular}[c]{@{}l@{}}Average \\ (ms)\end{tabular} & \begin{tabular}[c]{@{}l@{}}Standard deviation \\ (ms)\end{tabular} \\")
    latex_lines.append(r"\midrule")
    
    # Row Processing
    for index, row in df.iterrows():
        # Clean values: format numbers to 2 decimal places
        n_val = str(row.iloc[0])
        desc_val = str(row.iloc[1])
        avg_val = f"{row['Average']:.2f}"
        std_val = f"{row['Standard_Deviation']:.2f}"
        
        # Add a row to the table
        latex_lines.append(f" {n_val} & {desc_val} & ${avg_val}$ & ${std_val}$ \\\\")
    
    # Footer
    latex_lines.append(r"\bottomrule")
    latex_lines.append(r"\end{tabular}")
    latex_lines.append(r"\end{table}")
    
    return "\n".join(latex_lines)

if os.path.exists(sys.argv[1]):
    latex_code = generate_latex_table(sys.argv[1])
    print(latex_code)
    