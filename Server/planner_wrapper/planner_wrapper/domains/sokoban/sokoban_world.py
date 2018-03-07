__doc__="""
Represents a module containing all the class representing the model of the sokoban world
"""

import enum
import json
import collections
import typing
import itertools

from planner_wrapper.utils import ParsableEnum
from planner_wrapper.utils import Point


BaseCellContentTuple = collections.namedtuple('BaseCellContentTuple', ' '.join(['character', ]))


class BaseCellContent(ParsableEnum):
    ROBOT = BaseCellContentTuple(character="R")
    DOCKING_STATION = BaseCellContentTuple(character="D")
    BLOCK = BaseCellContentTuple(character="B")
    GOAL = BaseCellContentTuple(character="G")
    UNTRAVERSABLE = BaseCellContentTuple(character="U")

class Block:

    def __init__(self, theid: int, position: Point):
        self.id = theid
        self.position = position

    def to_pddl_predicate(self) -> str:
        return "stone-{:02d}".format(self.id)


class SokobanWorld:
    """
    Represents the sokoban world. It is a decorated matrix. (0,0) is in the topleft corner of the matrix.
    DOWN is when the y tends to increase in module.
    """

    def __init__(self, rows, columns):
        self._m = [set() for _ in range(rows*columns)]
        self._rows = rows
        self._colums = columns
        self._docking_station = None
        self._robot = None
        self._blocks = []
        self._next_block_id = 0

    def _2d21d(self, row, col):
        return row * self.cols + col

    def _1d22d(self, index) -> Point:
        return Point(y=divmod(index, self._colums)[0], x=divmod(index, self._colums)[1])

    def add_content_in(self, row, col, content: BaseCellContent):
        self._m[self._2d21d(row, col)].add(content)
        if content == BaseCellContent.ROBOT:
            self._robot = Point(y=row, x=col)
        if content == BaseCellContent.DOCKING_STATION:
            self._docking_station = Point(y=row, x=col)
        if content == BaseCellContent.BLOCK:
            self._blocks.append(Block(theid=self._next_block_id, position=Point(y=row, x=col)))
            self._next_block_id += 1

    def remove_content_in(self, row, col, content: BaseCellContent):
        self._m[self._2d21d(row, col)].remove(content)

    def contains_content(self, row, col, content: BaseCellContent):
        return content in self._m[self._2d21d(row, col)]

    def __getitem__(self, item: Point) -> typing.List[BaseCellContent]:
        return self._m[self._2d21d(row=item.y, col=item.x)]

    def __setitem__(self, key: Point, value: typing.List[BaseCellContent]):
        self._m[self._2d21d(row=key.y, col=key.x)] = value

    def is_traversable(self, row, col):
        return BaseCellContent.UNTRAVERSABLE not in self[Point(y=row, x=col)]

    def is_empty(self, row, col):
        """

        :param row: the row of the cell involved
        :param col: the column of the cell involved
        :return: False if in the cell there is a ROBOT, UNTRAVERSABLE or a BLOCK; true otherwise or if there is nothing
        """

        if BaseCellContent.UNTRAVERSABLE in self[Point(y=row, x=col)]:
            return False
        if BaseCellContent.BLOCK in self[Point(y=row, x=col)]:
            return False
        if BaseCellContent.ROBOT in self[Point(y=row, x=col)]:
            return False
        if BaseCellContent.DOCKING_STATION in self[Point(y=row, x=col)]:
            return True # goal doesn't count for clear cell
        if BaseCellContent.GOAL in self[Point(y=row, x=col)]:
            return True # goal doesn't count for clear cell
        return len(self[Point(y=row, x=col)]) == 0

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
    def cells_number(self):
        return self.rows * self.cols

    @property
    def robot(self):
        return self._robot

    @property
    def docking_station(self):
        return self._docking_station

    @property
    def cells(self) -> typing.Iterable[Point]:
        return (Point(x=t[1], y=t[0]) for t in itertools.product(range(0, self.rows), range(0, self.cols)))

    @property
    def blocks(self) -> typing.Iterable[Block]:
        return self._blocks

    @property
    def goals(self) -> typing.Iterable[Point]:
        ret_val = []
        for index in range(self.cells_number):
            if BaseCellContent.GOAL in self[self._1d22d(index)]:
                ret_val.append(self._1d22d(index))
        return ret_val
