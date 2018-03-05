import os
from flask import Blueprint, request

from planner_wrapper.planner import LPGPlanner
from planner_wrapper import sokoban_problem_generator, solution_converter
from planner_wrapper.sokoban_world import SokobanWorldJsonParserVersion1
from web.static.flask_exceptions import SolutionNotFoundException

simple_page = Blueprint('index', __name__)


@simple_page.route("/sokoban_problem")
def sokoban_problem():
    content = request.get_json(silent=True)

    domain_version1_filename = os.path.abspath("../Problems/Sokoban/domain.pddl")

    simple_page.logger.info('generating sokoban world from received json...')
    json_to_world_parser = SokobanWorldJsonParserVersion1()
    sokoban_world = json_to_world_parser.parse(content)

    simple_page.logger.info('generating pddl problem file from sokoban world...')
    problem_generator = sokoban_problem_generator.PddlSokobanConverterVersion1()
    problem_filename = problem_generator.generate_problem(
        problem_filename="problem_instance",
        domain_name="../Problems/Sokoban/domain.pddl",
        problem_name="sokoban-sequential",
        world=sokoban_world,
    )

    simple_page.logger.info('generating planner instance manager...')
    planner = LPGPlanner(lpg_location="/home/koldar/Documents/LPGFrancesco/LPG/lpg")

    planner.use_best_first = True
    planner.use_lpg = False
    planner.solutions_to_find = 1
    planner.output_filename = os.path.join(".", "computed.plan")

    simple_page.logger.info('invoking planner (this may take quite time!)...')
    ret = planner.invoke(
        domain_filename=domain_version1_filename,
        problem_filename=problem_filename,
        working_directory="."
    )
    if ret is False:
        simple_page.logger.info("planner didn't find any solutions!")
        # the planner didn't generate anything
        raise SolutionNotFoundException(f"Couldn't find solution for problem!")

    simple_page.logger.info('Solution found! computing the json of it!')

    ##############################
    #NICOLA implementation (slick)
    ##############################

    p = solution_converter.plan_to_dict(planner.output_filename)
    json_outputfilename = "output.json"
    solution_converter.dict_to_json(p, json_outputfilename)
    with open(json_outputfilename, "r") as f:
        json_string = f.read()

    ##############################
    # MAX implementation (too big)
    ##############################

    # json_string = planner.convert_plan_to_json(planner.output_filename)
    simple_page.logger.info('Returning json')
    return json_string
