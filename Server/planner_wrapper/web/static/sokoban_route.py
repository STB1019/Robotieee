from flask import Blueprint

simple_page = Blueprint('index', __name__)


@simple_page.route('/')
@simple_page.route('/index')
def index():
    return "Hello, World!"