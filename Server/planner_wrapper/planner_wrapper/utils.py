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