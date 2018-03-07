class Point:
    """
    A point within sokoban
    """

    def __init__(self, x, y):
        self._p = [x, y]

    def __iter__(self):
        return iter(self._p)

    def __getitem__(self, item):
        return self._p[item]

    def __setitem__(self, key, value):
        self._p[key] = value

    def __eq__(self, other):
        if other is None:
            return False
        try:
            return self.x == other.x and self.y == other.y
        except AttributeError:
            return False

    def __str__(self):
        return f"<x={self.x}, y={self.y}>"

    @property
    def x(self):
        return self[0]

    @property
    def y(self):
        return self[1]

    def to_json(self):
        return {"x": self.x, "y": self.y}
