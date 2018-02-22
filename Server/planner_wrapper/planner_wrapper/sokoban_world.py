import enum
import json
import collections
import typing
import itertools

BaseCellContentTuple = collections.namedtuple('BaseCellContentTuple', ' '.join(['character']))


class Point:

    def __init__(self, x, y):
        self._p = [x, y]

    def __iter__(self):
        return iter(self._p)

    def __getitem__(self, item):
        return self._p[item]

    def __setitem__(self, key, value):
        self._p[key] = value

    @property
    def x(self):
        return self[0]

    @property
    def y(self):
        return self[1]


class BaseCellContent(enum.Enum):
    ROBOT = BaseCellContentTuple(character="R")
    DOCKING_STATION = BaseCellContentTuple(character="D")
    BLOCK = BaseCellContentTuple(character="B")
    GOAL = BaseCellContentTuple(character="G")
    UNTRAVERSABLE = BaseCellContentTuple(character="U")

    @classmethod
    def parse(cls, string: str):
        if len(string) != 1:
            raise ValueError("can't parse to {} the non-character string {}".format(cls.__name__, string))
        for s in BaseCellContent:
            if s.value.character == string:
                return s
        else:
            raise ValueError("unsupport string {}", string)


class SokobanWorld:
    def __init__(self, rows, columns):
        self._m = [set() for _ in range(rows*columns)]
        self._rows = rows
        self._colums = columns
        self._docking_station = None
        self._robot = None

    def _2d21d(self, row, col):
        return row * self.cols + col

    def _1d22d(self, index) -> Point:
        return Point(y=divmod(index, self._colums)[0], x=divmod(index, self._colums)[1])

    def add_content_in(self, row, col, content: BaseCellContent):
        self._m[self._2d21d(row, col)].add(content)
        if content == BaseCellContent.ROBOT:
            self._robot = Point(row, col)
        if content == BaseCellContent.DOCKING_STATION:
            self._docking_station = Point(row, col)

    def remove_content_in(self, row, col, content: BaseCellContent):
        self._m[self._2d21d(row, col)].remove(content)

    def contains_content(self, row, col, content: BaseCellContent):
        return content in self._m[self._2d21d(row, col)]

    def __getitem__(self, item: typing.Tuple[int, int]) -> typing.List[BaseCellContent]:
        row, col = item
        return self._m[self._2d21d(row, col)]

    def __setitem__(self, key: typing.Tuple[int, int], value: typing.List[BaseCellContent]):
        row, col = key
        self._m[self._2d21d(row, col)] = value

    def is_traversable(self, row, col):
        return BaseCellContent.UNTRAVERSABLE not in self[row, col]

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
    def blocks(self) -> typing.Iterable[Point]:
        ret_val = []
        for index in range(self.cells_number):
            if BaseCellContent.BLOCK in self[self._1d22d(index)]:
                ret_val.append(self._1d22d(index))
        return ret_val

    @property
    def goals(self) -> typing.Iterable[Point]:
        ret_val = []
        for index in range(self.cells_number):
            if BaseCellContent.GOAL in self[self._1d22d(index)]:
                ret_val.append(self._1d22d(index))
        return ret_val



    @classmethod
    def parse(cls, json_str: str):
        js = json.loads(json_str)
        version = str(js['version'])
        if version == "1.0":
            return SokobanWorldVersion1().parse(js)
        else:
            raise ValueError("unsupported version {}".format(version))


class SokobanWorldVersion1:

    def parse(self, js) -> SokobanWorld:
        world = js['world']
        rows = int(world['rows'])
        cols = int(world['columns'])
        retval = SokobanWorld(rows, cols)

        for cell in world['cells']:
            for ch in cell['entities']:
                retval.add_content_in(
                    col=int(cell['x']),
                    row=int(cell['y']),
                    content=BaseCellContent.parse(str(ch))
                )

        return retval