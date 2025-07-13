from flask import Flask

app = Flask(__name__)

@app.route('/')
def index():
    return '<h1>🚀 proxy-test.fastping.it.com is working!</h1>'

if __name__ == '__main__':
    app.run(host='141.22.198.147', port=5060)
