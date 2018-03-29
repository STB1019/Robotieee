import argparse
import logging
from logging.handlers import RotatingFileHandler

from flask import Flask, jsonify
from web.static import sokoban_route, exploration_route
from web.static.flask_exceptions import FlaskException
from planner_wrapper import config

# main application
app = Flask(__name__)
# log file
handler = RotatingFileHandler('flask.log', maxBytes=10000, backupCount=1)
handler.setLevel(logging.INFO)
app.logger.addHandler(handler)
# setup werkzeug log file (in order to have everything in the logfile
log = logging.getLogger('werkzeug')
log.setLevel(logging.INFO)
log.addHandler(handler)
# blueprint registration
app.register_blueprint(sokoban_route.simple_page)
app.register_blueprint(exploration_route.simple_page)

# error handler
@app.errorhandler(FlaskException)
def handle_invalid_usage(error):
    response = jsonify(error.to_dict())
    response.status_code = error.status_code
    return response


def parse_arguments():
    """
    Parse the option arguments
    :return: an object storing the parsed arguments
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", "--planner_location",
            type=str,
            help="""
            If we're using a planner in the system, 
            the string represents the location where the planner executable is"""
    )
    parser.add_argument("-p", "--use_planner",
        metavar=str,
        required=True,
        help="""the planner to use. Accepted values are "lpg" or "fake"."""
    )
    parser.add_argument("-I", "--ip",
        metavar=str,
        default="0.0.0.0",
        help="""The ips where the server will accepts connections from"""
    )
    parser.add_argument("-P", "--port",
        metavar=int,
        default=5000,
        help="""The port where the server will accept connections from"""
    )

    o = parser.parse_args()
    return o


if __name__ == "__main__":
    options = parse_arguments()
    config.PLANNER_LOCATION = options.planner_location
    config.USE_PLANNER = options.use_planner

    app.run(host=options.ip, port=options.port)
