from planner_wrapper.domains.grid_model import GridWorld

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
    VISITED = BaseCellContentTuple(character="V")


class Block:

    def __init__(self, theid: int, position: Point):
        self.id = theid
        self.position = position

    def to_pddl_predicate(self) -> str:
        return "stone-{:02d}".format(self.id)


class SokobanWorld(GridWorld):
    """
    Represents the sokoban world. It is a decorated matrix. (0,0) is in the topleft corner of the matrix.
    DOWN is when the y tends to increase in module.
    """

    def __init__(self, rows, columns):
        super().__init__(rows, columns)
        self._docking_station = None
        self._robot = None
        self._blocks = []
        self._next_block_id = 0

    def add_content_in(self, row, col, content: BaseCellContent):
        super().add_content_in(row, col, content)
        if content == BaseCellContent.ROBOT:
            self._robot = Point(y=row, x=col)
        if content == BaseCellContent.DOCKING_STATION:
            self._docking_station = Point(y=row, x=col)
        if content == BaseCellContent.BLOCK:
            self._blocks.append(Block(theid=self._next_block_id, position=Point(y=row, x=col)))
            self._next_block_id += 1

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

    @property
    def robot(self):
        return self._robot

    @property
    def docking_station(self) -> Point:
        return self._docking_station

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
