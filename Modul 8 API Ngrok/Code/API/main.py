from flask import *
import json, time


app = Flask(__name__)

@app.route('/', methods=['GET'])
def home():
    page_request = {'Page': 'HOME', 'Message': 'Load HOME PAGE', 'Timestamp': time.time()} #json
    json_dump = json.dumps(page_request)

    return json_dump

@app.route('/user/', methods=['GET'])
def request_user():
    user_query = str(request.args.get('user')) #/user/?user=Username

    page_request = {'Page': 'REQUEST', 'Message': f'Load REQUEST from {user_query}', 'Timestamp': time.time()} #json
    json_dump = json.dumps(page_request)

    return json_dump

if __name__ == '__main__':
    app.run(port=8000)

