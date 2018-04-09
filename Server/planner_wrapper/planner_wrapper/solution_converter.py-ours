import json
import re

import collections

action_tags = {
	'move': ('action', 'player', 'start-pos', 'end-pos', 'direction'),
	'push-to-goal': ('action', 'player', 'stone', 'player-start-pos', 'start-pos', 'end-pos', 'direction'),
	'push-to-nongoal': ('action', 'player', 'stone', 'player-start-pos', 'start-pos', 'end-pos', 'direction')
}


def convert_instruction(tags, invoker_label):
	instructions = collections.OrderedDict(zip(action_tags[invoker_label], tags))

	# change position tags to a more readable format
	if invoker_label == 'move':
		positions = tags[2:4]
		pos_tags = action_tags[invoker_label][2:4]
	else:
		positions = tags[3:6]
		pos_tags = action_tags[invoker_label][3:6]

	coords = convert_positions_tag(positions)
	p = dict(
		list(zip(pos_tags, coords)))  # {'start-pos', {('x', '5'), ('y', '3')}), ('end-pos', {('x', '6'), ('y', '3')})}
	instructions.update(p)
	return instructions


def convert_positions_tag(positions):
	res = []
	coords = ('x', 'y')
	for pos in positions:
		values = re.sub(r'^pos|-0|-', ' ', pos).lstrip().split()
		res.append(dict(zip(coords, values)))
	return res


def plan_to_dict(file_in):
	plan = {}
	plan['version'] = "1.0"
	plan['actions'] = []

	# parse the entire file
	with open(file_in, 'r') as f:
		lines = [line.rstrip('\n') for line in f]

	for l in lines:
		# l is an action
		if len(l) > 0:
			if l[0].isdigit():
				l = re.sub(r'^\d*:\s*\(|\)\s*\[\d\]', '', l).lower().split()
				plan['actions'].append(convert_instruction(l, str(l[0])))

	return plan


def dict_to_json(d, file_out):
	with open(file_out, 'w') as f:
		json.dump(d, f, indent=4)

# p = plan_to_dict(r"C:\Users\Nicola\Desktop\ZUMOrobot\Robotieee\Server\planner_wrapper\tests\test-instance.sol")
# dict_to_json(p, r"C:\Users\Nicola\Desktop\ZUMOrobot\Robotieee\Server\planner_wrapper\tests\plan.json")
