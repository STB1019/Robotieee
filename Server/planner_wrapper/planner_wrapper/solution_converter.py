import json
import typing
import collections
import re

# ; Version LPG-td-1.0
# ; Seed 89065539
# ; Command line: /home/koldar/Documents/LPGFrancesco/LPG/lpg -n 1 -f /home/koldar/git/Robotieee/Server/planner_wrapper/Problems/Sokoban/instance-1 -o /home/koldar/git/Robotieee/Server/planner_wrapper/Problems/Sokoban/domain.pddl -out /home/koldar/git/Robotieee/Server/planner_wrapper/tests/computed.plan -onlybestfirst 
# ; Problem /home/koldar/git/Robotieee/Server/planner_wrapper/Problems/Sokoban/instance-1
# ; Actions having STRIPS duration
# ; Time 4.28
# ; Search time 4.27
# ; Parsing time 0.01
# ; Mutex time 0.00
# ; NrActions 211

action_tags = {
    'move': ('action','player','start-pos','end-pos','direction'),
    'push-to-goal': ('action','player','stone','player-start-pos','start-pos','end-pos','direction'),
    'push-to-nongoal': ('action','player','stone','player-start-pos','start-pos','end-pos','direction')
}

def convert_instruction(tags, invoker_label):
    instructions = collections.OrderedDict(zip(action_tags[invoker_label],tags))

    #change position tags to a more readable format
    if invoker_label == 'move':
        positions = tags[2:4]
        pos_tags = action_tags[invoker_label][2:4]
    else:
        positions = tags[3:6]
        pos_tags = action_tags[invoker_label][3:6]
    
    coords = convert_positions_tag(positions)
    p = dict(list(zip(pos_tags, coords))) #{'start-pos', {('x', '5'), ('y', '3')}), ('end-pos', {('x', '6'), ('y', '3')})}
    instructions.update(p)
    return instructions

def convert_positions_tag(positions):
    res = []
    coords = ('x', 'y')
    for pos in positions:
        values = re.sub(r'^pos|-0', ' ', pos).lstrip().split()
        res.append(list(zip(coords, values)))
    return res

def plan_to_dict(file_in):
    plan = []

    #parse the entire file
    with open(file_in, 'r') as f:
        lines = [line.rstrip('\n') for line in f]

    for l in lines:
        #l is an action
        if len(l) > 0:
            if l[0].isdigit():
                l = re.sub(r'^\d*:\s*\(|\)\s*\[\d\]', '',l).lower().split()
                plan.append(convert_instruction(l, str(l[0])))
    
    return plan

def dict_to_json(d, file_out):
    with open(file_out, 'w') as f:
        json.dump(d,f,indent=2)
            
p = plan_to_dict(r"C:\Users\Nicola\Desktop\Ubuntu Backup\Robotieee-master\Server\planner_wrapper\tests\test-instance.sol")
dict_to_json(p, r"C:\Users\Nicola\Desktop\Ubuntu Backup\Robotieee-master\Server\planner_wrapper\tests\plan.json")

# ISSUES:
#     1) I'm expecting a method that gets as input param 'computed.plan_x.SOL' and parses it. Where is it?/ Where should I put it?
#     2) Silly question: I understand (not completely) the planner.py module, but is it really necessary? I know it is not as elegant
#     as the current solution, but I would use a HUGE switch-case construct while iterating throu the file, line by line, rather than using such complex architecture.
#     3) I still can't figure out how the entire program should be launched and how the modules are executed.
