import enum


class Point:
    """
    A point
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


class ParsableEnum(enum.Enum):

    @property
    def parsable_str(self):
        return self.value[0]

    @classmethod
    def parse(cls, string: str):
        for s in (cls[x] for x in list(cls.__members__.keys())):
            if s.parsable_str == string:
                return s
        else:
            raise ValueError("""Can't parse string "{}" into enumeration {}!""".format(string, cls.__name__))


class TabFileWriter:
    """
    Represents a class allowing you to write automatically indented data in a file

    You can increase the indent by calling tab_file_writer_instance += 1 and decrease the indent by calling
    tab_file_writer_instance -= 1.

    write and writeln methods are main methods of this class
    """

    def __init__(self, f, start_indent: int = 0, element_per_indent: int = 1,tabs_instead_of_spaces: bool = True):
        self.f = f
        self.indent = start_indent
        self.element_per_indent = element_per_indent
        self.tabs_instead_of_spaces = tabs_instead_of_spaces
        self.new_line = True

    def __iadd__(self, other: int):
        """
        Increase the tab by other
        :param other: the number of indent to increase. Typically it's one
        :return: self
        """
        self.indent += 1
        return self

    def __isub__(self, other: int):
        """
        Decrease the tab by other
        :param other: the number of indent to decrease. Typically it's one
        :return: self
        """
        self.indent -= 1
        return self

    def write(self, string: str = ""):
        """
        Write the automatically indented string in the file. If the string contains '\n', they are automatically
        handled by the function and the newlines will be automatically indented as well
        :param string: the string to write
        """
        if '\n' not in string:
            if self.new_line:
                #generate indentation
                if self.tabs_instead_of_spaces:
                    self.f.write('\t' * (self.indent * self.element_per_indent))
                else:
                    self.f.write(' ' * (self.indent * self.element_per_indent))
            #write real line
            self.f.write(string)
            self.new_line = False
        else:
            lines = string.split('\n')
            for line in lines:
                self.writeln(line)

    def writeln(self, string: str = ""):
        """
        Like write but it will autoamtically add a carriage return
        :param string: the string to add (no \n is needed at the end
        """
        self.write(string)
        self.f.write('\n')
        self.new_line = True