import collections
import typing

import itertools

from planner_wrapper.utils import ParsableEnum, Point


class GridWorld:
    """
    Represents a world which is a grid map. It is a decorated matrix. (0,0) is in the topleft corner of the matrix.
    DOWN is when the y tends to increase in module.
    """

    def __init__(self, rows, columns):
        self._m = [set() for _ in range(rows*columns)]
        self._rows = rows
        self._colums = columns

    def _2d21d(self, row, col):
        return row * self.cols + col

    def _1d22d(self, index) -> Point:
        return Point(y=divmod(index, self._colums)[0], x=divmod(index, self._colums)[1])

    def add_content_in(self, row, col, content):
        self._m[self._2d21d(row, col)].add(content)

    def remove_content_in(self, row, col, content):
        self._m[self._2d21d(row, col)].remove(content)

    def contains_content(self, row, col, content):
        return content in self._m[self._2d21d(row, col)]

    def __getitem__(self, item: Point) -> typing.List[typing.Any]:
        return self._m[self._2d21d(row=item.y, col=item.x)]

    def __setitem__(self, key: Point, value: typing.List[typing.Any]):
        self._m[self._2d21d(row=key.y, col=key.x)] = value

    def get_up(self, row, col):
        if row == 0:
            raise ValueError("can't go up!")
        return Point(x=col, y=row - 1)

    def get_down(self, row, col):
        if row == (self._rows - 1):
            raise ValueError("can't go down!")
        return Point(x=col, y=row + 1)

    def get_left(self, row, col):
        if col == 0:
            raise ValueError("can't go left")
        return Point(x=col - 1, y=row)

    def get_right(self, row, col):
        if col == (self._colums - 1):
            raise ValueError("can't go right")
        return Point(x=col + 1, y=row)

    @property
    def rows(self):
        return self._rows

    @property
    def cols(self):
        return self._colums

    @property
    def cells(self) -> typing.Iterable[Point]:
        return (Point(x=t[1], y=t[0]) for t in itertools.product(range(0, self.rows), range(0, self.cols)))

    @property
    def cells_number(self):
        return self.rows * self.cols
