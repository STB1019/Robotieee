import json

# ASCII symbol to sokoban entity
# I've assumed that the sokoban robot starts from his docking station
conversion_dict = {
	'#': 'U',
	'@': 'RD',
	'$': 'B',
	'.': 'G',
	' ': ' '
}


def convert_line(line, line_counter):
	"""Converts an ASCIIart sokoban instance to a JSON request that can be passed to the server thru curl"""
	cells = []
	# cell = collections.OrderedDict({})
	cell = {}

	for x in range(len(line)):
		cell = {'y': line_counter, 'x': x, 'entities': conversion_dict[line[x]]}
		cells.append(cell)
	return cells


def parse_ASCII(file_in):
	"""Parses a file to extract the ASCII art"""
	sokoban_instance = {'version': 1.0}
	cell_list = []

	with open(file_in, 'r') as f:
		lines = [line.rstrip('\n') for line in f]

	rows = len(lines)
	columns = len(lines[0])

	line_counter = 0

	# check walls presence at the beginning & at the end of the file
	if ''.join(lines[0]) != '#' * columns or ''.join(lines[-1]) != '#' * columns:
		raise ValueError("Unexpected file format @ line {}".format(line_counter))

	for l in lines:
		if line_counter == 0 or line_counter == len(lines) - 1:
			pass
		else:
			if len(l) > 0:
				if l[0] != '#' or l[-1] != '#':  # expecting a starting (and ending) wall for each line
					raise ValueError("Unexpected file format @ line {}".format(line_counter))
				else:
					formatted_line = l[1:len(l) - 1]  # remove walls in the line
					line2cells = convert_line(line=formatted_line, line_counter=line_counter)
					cell_list.append(cell for cell in line2cells)
		line_counter += 1

	world_instance = {'rows': rows, 'columns': columns, 'cells': cell_list}
	sokoban_instance['world'] = world_instance

	return sokoban_instance


def dict_2_json(d, file_out):
	"""Write a dictionary to a file"""
	with open(file_out, 'w') as f:
		json.dump(d, f, indent=4)


# main
if __name__ == '__main__':
	instance = parse_ASCII(file_in=r'C:\Users\Nicola\Desktop\sokoban_example2.txt')
# dict_2_json(d=instance, file_out=r'C:\Users\Nicola\Desktop\sokoban_out.json')
