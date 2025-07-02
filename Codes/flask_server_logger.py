from flask import Flask, request
import pandas as pd
from datetime import datetime
import os

app = Flask(__name__)

# Define different file names for clarity, or combine into one if structure allows
MPU_FILE_NAME = "D:/Personal/SCM_Internship/Data_privatisation/MPU6050/MPU6050_Log.csv"
SCT_FILE_NAME = "D:/Personal/SCM_Internship/Data_privatisation/SCT013/SCT013_Log.csv" # New file for current sensor

# Ensure the directory exists for SCT013
os.makedirs(os.path.dirname(SCT_FILE_NAME), exist_ok=True)


@app.route("/update", methods=['POST'])
def update():
    try:
        data = request.json
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        
        # Check if the data contains MPU6050 fields
        if 'gyroX_dps' in data or 'euclidean_sum_accel' in data:
            new_entry = {
                'Timestamp': timestamp,
                'GyroX_dps': data.get('gyroX_dps'),
                'GyroY_dps': data.get('gyroY_dps'),
                'GyroZ_dps': data.get('gyroZ_dps'),
                'Accel_Euclidean': data.get('euclidean_sum_accel')
            }
            file_to_write = MPU_FILE_NAME
            columns = ['Timestamp', 'GyroX_dps', 'GyroY_dps', 'GyroZ_dps', 'Accel_Euclidean']

        # Check if the data contains SCT-013 field
        elif 'field5' in data:
            new_entry = {
                'Timestamp': timestamp,
                'Current_Irms': data.get('field5')
            }
            file_to_write = SCT_FILE_NAME
            columns = ['Timestamp', 'Current_Irms']

        else:
            return {'error message': 'Unknown data format received'}, 400

        # --- Logic for appending to CSV ---
        if os.path.exists(file_to_write) and os.path.getsize(file_to_write) > 0:
            df = pd.read_csv(file_to_write)
            df = pd.concat([df, pd.DataFrame([new_entry])], ignore_index=True)
            # Ensure all columns are present, fill missing with NaN if necessary
            df = df.reindex(columns=columns, fill_value=pd.NA)
        else:
            df = pd.DataFrame([new_entry], columns=columns) # Ensure columns are set for new file

        df.to_csv(file_to_write, index=False)
        return {'status': 'success'}, 200

    except Exception as e:
        print(f"Error processing request: {e}") # Print error to console for debugging
        return {'error message': str(e)}, 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
