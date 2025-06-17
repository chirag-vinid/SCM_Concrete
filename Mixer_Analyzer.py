#Headers
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import numpy as np

# --- Configuration ---
# Name of your CSV file
csv_file = "D:\Personal\SCM_Internship\Mixer_Data\mixer_data_1.csv"
data_fields = [
    "gyroX_dps", "gyroY_dps", "gyroZ_dps",
    "euclidean_sum_accel_g", # Renamed for clarity, will convert this
    "combined_angular_velocity_dps",
    "euclidean_sum_accel_ms2" # New field for acceleration in m/s^2
]
calculated_fields = [
    "combined_angular_velocity_dps",
    "euclidean_sum_accel_ms2"
]

# --- Conversion factor ---
GRAVITY_MS2 = 9.80665 # Standard acceleration due to gravity in m/s^2

# --- Load Data ---
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

# --- Calculate Derived Fields ---
if 'euclidean_sum_accel' in df.columns and 'euclidean_sum_accel_g' not in df.columns:
    df.rename(columns={'euclidean_sum_accel': 'euclidean_sum_accel_g'}, inplace=True)
    print("\nRenamed 'euclidean_sum_accel' to 'euclidean_sum_accel_g'.")
    if "euclidean_sum_accel" in data_fields:
        data_fields.remove("euclidean_sum_accel")
    if "euclidean_sum_accel_g" not in data_fields:
        data_fields.insert(data_fields.index("gyroZ_dps") + 1, "euclidean_sum_accel_g") # Keep original order where possible


# --- Calculating Combiner Angular Velocity ---
required_gyro_cols = ["gyroX_dps", "gyroY_dps", "gyroZ_dps"]
if all(col in df.columns for col in required_gyro_cols):
    df['combined_angular_velocity_dps'] = np.sqrt(
        df['gyroX_dps']**2 + df['gyroY_dps']**2 + df['gyroZ_dps']**2
    )
    print("\nCalculated 'combined_angular_velocity_dps' column.")
else:
    print(f"\nWarning: Could not calculate 'combined_angular_velocity_dps'. Missing one or more of {required_gyro_cols} columns.")
    if 'combined_angular_velocity_dps' in data_fields:
        data_fields.remove('combined_angular_velocity_dps')


# --- Calculating acceleration in m/s^2 ---
if 'euclidean_sum_accel_g' in df.columns:
    df['euclidean_sum_accel_ms2'] = df['euclidean_sum_accel_g'] * GRAVITY_MS2
    # Round the acceleration values to 3 decimal places
    df['euclidean_sum_accel_ms2'] = df['euclidean_sum_accel_ms2'].round(3)
    print(f"Calculated and rounded 'euclidean_sum_accel_ms2' column (using g={GRAVITY_MS2} m/s^2) to 3 decimal places.")
else:
    print(f"\nWarning: Could not calculate 'euclidean_sum_accel_ms2'. Missing 'euclidean_sum_accel_g' column.")
    if 'euclidean_sum_accel_ms2' in data_fields:
        data_fields.remove('euclidean_sum_accel_ms2')


# --- Calculate Descriptive Statistics ---
print("\n--- Descriptive Statistics ---")
for field in data_fields:
    if field in df.columns:
        if pd.api.types.is_numeric_dtype(df[field]):
            mean_val = df[field].mean()
            median_val = df[field].median()
            std_val = df[field].std()
            min_val = df[field].min()
            max_val = df[field].max()
            q1 = df[field].quantile(0.25)
            q3 = df[field].quantile(0.75)
            iqr_val = q3 - q1
            mode_result = stats.mode(df[field].dropna())
            mode_val = mode_result.mode[0] if isinstance(mode_result.mode, np.ndarray) else mode_result.mode
            mode_count = mode_result.count[0] if isinstance(mode_result.count, np.ndarray) else mode_result.count
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

# --- Plot Histograms ---
print("\n--- Generating Histograms for Calculated Fields Only ---")
fields_to_plot = [field for field in data_fields if field in calculated_fields]
num_plots = len(fields_to_plot)
if num_plots > 0:
    rows = (num_plots + 1) // 2
    plt.figure(figsize=(6 * 2, 4 * rows))
    for i, field in enumerate(fields_to_plot):
        if field in df.columns:
            if pd.api.types.is_numeric_dtype(df[field]):
                plt.subplot(rows, 2, i + 1)
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
else:
    print("No calculated fields to plot histograms for.")
print("\nAnalysis complete. Check the console for statistics and the plot window for histograms (if any).\n")
