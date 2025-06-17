#Header Files
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats 

# Name of CSV file
csv_file = "D:\Personal\SCM_Internship\Mixer_Data\mixer_data_1.csv"

# Names of the four fields that are being analyzed
data_fields = ["gyroX_dps", "gyroY_dps", "gyroZ_dps", "euclidean_sum_accel"]

# Loading Data
try:
    df = pd.read_csv(csv_file)
    print("CSV loaded successfully. First 5 rows:")
    print(df.head())
    print("\nColumn names:")
    print(df.columns.tolist())
except FileNotFoundError:
    print(f"Error: The file '{csv_file}' was not found. Please ensure it's in the same directory or provide the full path.")
    exit()
except Exception as e:
    print(f"An error occurred while reading the CSV: {e}")
    exit()

# Calculating Descriptive Statistics
print("\n--- Descriptive Statistics ---")
for field in data_fields:
    if field in df.columns:
        if pd.api.types.is_numeric_dtype(df[field]):
            # Basic statistics using pandas
            mean_val = df[field].mean()
            median_val = df[field].median()
            std_val = df[field].std()
            min_val = df[field].min()
            max_val = df[field].max()
            q1 = df[field].quantile(0.25)
            q3 = df[field].quantile(0.75)
            iqr_val = q3 - q1
            mode_result = stats.mode(df[field].dropna())
            mode_val = mode_result.mode
            mode_count = mode_result.count
            skew_val = df[field].skew()
            kurt_val = df[field].kurt()
            print(f"\nStatistics for: {field}")
            print(f"  Mean: {mean_val:.4f}")
            print(f"  Median: {median_val:.4f}")
            print(f"  Mode: {mode_val:.4f} (Count: {mode_count})")
            print(f"  Standard Deviation: {std_val:.4f}")
            print(f"  Min: {min_val:.4f}")
            print(f"  Max: {max_val:.4f}")
            print(f"  IQR (Interquartile Range): {iqr_val:.4f}")
            print(f"  Skewness: {skew_val:.4f}")
            print(f"  Kurtosis: {kurt_val:.4f}")
            print(f"  Number of Observations: {df[field].count()}")
        else:
            print(f"Warning: Field '{field}' is not numeric. Skipping statistics.")
    else:
        print(f"Warning: Field '{field}' not found in CSV. Skipping statistics.")

# Histogram Plotting
print("\n--- Generating Histograms ---")
plt.style.use('seaborn-v0_8-darkgrid')
plt.figure(figsize=(12, 8))
for i, field in enumerate(data_fields):
    if field in df.columns:
        if pd.api.types.is_numeric_dtype(df[field]):
            plt.subplot(2, 2, i + 1)
            sns.histplot(df[field].dropna(), kde=True, bins=30, color='skyblue')
            plt.title(f'Histogram of {field}', fontsize=12)
            plt.xlabel(field, fontsize=10)
            plt.ylabel('Frequency', fontsize=10)
        else:
            print(f"Warning: Cannot plot histogram for '{field}'. Field is not numeric.")
    else:
        print(f"Warning: Cannot plot histogram for '{field}'. Field not found.")
plt.tight_layout() 
plt.show() 
print("\nAnalysis complete. Check the console for statistics and the plot window for histograms.\n")
