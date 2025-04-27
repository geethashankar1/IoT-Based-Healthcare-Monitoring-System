from flask import Flask, request, jsonify, render_template

app = Flask(__name__)

sensor_data = {"heart_rate": "--", "spo2": "--", "temperature": "--"}

@app.route('/')
def index():
    return render_template('index.html', data=sensor_data)

@app.route('/data', methods=['POST'])
def receive_data():
    global sensor_data
    data = request.get_json()
    if data:
        sensor_data = {
            "heart_rate": data.get("heart_rate", "--"),
            "spo2": data.get("spo2", "--"),
            "temperature": data.get("temperature", "--")
        }
        return jsonify({"message": "Data received"}), 200
    return jsonify({"error": "No data received"}), 400

@app.route('/get_data', methods=['GET'])
def send_data():
    return jsonify(sensor_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)

