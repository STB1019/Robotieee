import typing
from abc import ABCMeta


class FlaskException(Exception, metaclass=ABCMeta):

    def __init__(self, message: str, status_code: str = 400, payload: typing.Dict[typing.AnyStr, typing.Any] = None):
        Exception.__init__(self)
        self.message = message
        self.status_code = status_code
        self.payload = payload or {}

    def to_dict(self):
        rv = dict(self.payload or ())
        rv['message'] = self.message
        rv['errorcode'] = int(self.status_code)
        return rv


class SolutionNotFoundException(FlaskException):

    def __init__(self, message, status_code=400, payload=None):
        FlaskException.__init__(self, message, status_code, payload)


class MalformedRequestException(FlaskException):

    def __init__(self, message, status_code=400, payload=None):
        FlaskException.__init__(self, message, status_code, payload)