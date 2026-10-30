import matplotlib.pyplot as plt
import pandas as pd
import os

# Loop through all CSV files in the current directory
for file in os.listdir('.'):
    if file.endswith('.csv') and not file.startswith('processed_'):
        # Load the file
        df = pd.read_csv(file)

        # Select the 10 numeric columns (indices 2 to 11)
        # Assuming first two columns are at index 0 and 1
        numeric_data = df.iloc[:, 2:12]

        # Compute calculations
        df['Average'] = numeric_data.mean(axis=1)
        df['Standard_Deviation'] = numeric_data.std(axis=1)

        # Save the result to a new file
        output_filename = f'processed_{file}'
        df.to_csv(output_filename, index=False)
        print(f"Processed: {file} -> {output_filename}")

        # Load the processed data
        df = pd.read_csv(output_filename)

        # Use the first column as the identifier for the X-axis
        x_labels = df.iloc[:, 0].astype(str)
        # Select the 10 numeric columns
        numeric_cols = df.columns[2:12]

        plt.figure(figsize=(12, 6))

        # 1. Scatter plot: For each row identifier, plot all 10 numeric values
        for col in numeric_cols:
            plt.scatter(x_labels, df[col], color='blue', alpha=0.3, label='_nolegend_')

        # Add a single dummy scatter for the legend
        plt.scatter([], [], color='blue', alpha=0.3, label='Individual Values (10 per row)')

        # 2. Line plot: Connect the row-wise averages
        plt.plot(x_labels, df['Average'], color='red', marker='s', 
                linewidth=2, label='Row Average')

        # Formatting the plot
        plt.title('Numeric Distribution per Row and Overall Average Trend')
        plt.xlabel('Row Identifiers (First Column)')
        plt.ylabel('Numeric Values')
        plt.xticks(rotation=45)
        plt.legend()
        plt.grid(True, linestyle='--', alpha=0.5)
        plt.tight_layout()

        plt.savefig(f'{output_filename}.png')