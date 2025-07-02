# Header Files
from flask import Flask, request
import pandas as pd
from datetime import datetime
import os

app = Flask(__name__)

# Use .csv file instead of .xlsx
FILE_NAME = "D:/Personal/SCM_Internship/Data_privatisation/led_test/LED_Log.csv"

@app.route("/update", methods=['POST'])
def update():
    try:
        data = request.json
        led_state = data.get('led_state')
        new_entry = {
            'Timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
            'LED State': led_state
        }

        if os.path.exists(FILE_NAME) and os.path.getsize(FILE_NAME) > 0:
            df = pd.read_csv(FILE_NAME)
            df = pd.concat([df, pd.DataFrame([new_entry])], ignore_index=True)
        else:
            df = pd.DataFrame([new_entry])

        df.to_csv(FILE_NAME, index=False)
        return {'status': 'success'}, 200
    except Exception as e:
        return {'error message': str(e)}, 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
