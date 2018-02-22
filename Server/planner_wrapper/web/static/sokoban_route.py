from flask import Blueprint, request

simple_page = Blueprint('index', __name__)


@simple_page.route("/sokoban_problem", method=['POST'])
def sokoban_problem():
    content = request.get_json(silent=True)
    return content