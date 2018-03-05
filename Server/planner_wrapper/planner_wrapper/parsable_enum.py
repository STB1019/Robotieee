import enum


class ParsableEnum(enum.Enum):

    @property
    def parsable_str(self):
        return self.value[0]

    @classmethod
    def parse(cls, string: str):
        for s in (cls[x] for x in list(cls.__members__.keys())):
            print("s is {} aand string is {}".format(s.parsable_str, string))
            if s.parsable_str == string:
                return s
        else:
            raise ValueError("""Can't parse string "{}" into enumeration {}!""".format(string, cls.__name__))