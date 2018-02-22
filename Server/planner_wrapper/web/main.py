from flask import Flask
from web.static import sokoban_route

app = Flask(__name__)
app.register_blueprint(sokoban_route.simple_page)

if __name__ == "__main__":
    app.run()