import logging
from logging.handlers import RotatingFileHandler

from flask import Flask, jsonify
from web.static import sokoban_route
from web.static.flask_exceptions import SolutionNotFoundException

# main application
app = Flask(__name__)
#log file
handler = RotatingFileHandler('flask.log', maxBytes=10000, backupCount=1)
handler.setLevel(logging.INFO)
app.logger.addHandler(handler)
#setup werkzeug log file (in order to have everything in the logfile
log = logging.getLogger('werkzeug')
log.setLevel(logging.INFO)
log.addHandler(handler)
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
