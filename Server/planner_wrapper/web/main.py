from flask import Flask, jsonify
from web.static import sokoban_route
from web.static.flask_exceptions import SolutionNotFoundException

# main application
app = Flask(__name__)
# blueprint registration
app.register_blueprint(sokoban_route.simple_page)

# error handler


@app.errorhandler(SolutionNotFoundException)
def handle_invalid_usage(error):
    response = jsonify(error.to_dict())
    response.status_code = error.status_code
    return response


if __name__ == "__main__":
    app.run()