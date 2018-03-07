import os
from flask import Blueprint, request
from flask import current_app
from werkzeug.local import LocalProxy

from planner_wrapper.lpg_planner import LPGPlanner
from planner_wrapper import sokoban_problem_generator, solution_converter
from planner_wrapper.sokoban_world import SokobanWorldJsonParserVersion1
from web.static.flask_exceptions import SolutionNotFoundException, MalformedRequestException

simple_page = Blueprint('index', __name__)

# logger
logger = LocalProxy(lambda: current_app.logger)

@simple_page.route("/sokoban_problem", methods=['POST'])
def sokoban_problem():
    """
    Example:

    curl -i http://localhost:5000/sokoban_problem -X POST -d '{"version": "1.0","world": {"rows": 3,"columns": 3,"cells": [{ "y": 0, "x": 0, "entities": "RG"},{ "y": 0, "x": 1, "entities": ""},{ "y": 0, "x": 2, "entities": "U"},{ "y": 1, "x": 0, "entities": ""},{ "y": 1, "x": 1, "entities": "B"},{ "y": 1, "x": 2, "entities": ""},{ "y": 2, "x": 0, "entities": ""},{ "y": 2, "x": 1, "entities": ""},{ "y": 2, "x": 2, "entities": ""}]}}' -H "Content-Type: application/json"


    :return:
    """
    content = request.get_json(silent=True)
    if content is None:
        raise MalformedRequestException("Couldn't fetch world json in this request. You should mark this request as a json one and put in the payload a compliant json!")

    domain_version1_filename = os.path.abspath("../Problems/Sokoban/domain.pddl")

    logger.info('generating sokoban world from received json...')
    json_to_world_parser = SokobanWorldJsonParserVersion1()
    sokoban_world = json_to_world_parser.parse(content)

    logger.info('generating pddl problem file from sokoban world...')
    problem_generator = sokoban_problem_generator.PddlSokobanConverterVersion1()
    problem_filename = problem_generator.generate_problem(
        problem_filename="problem_instance",
        domain_name="sokobanSequential",
        problem_name="sokobanSequential-01",
        world=sokoban_world,
    )

    logger.info('generating planner instance manager...')
    planner = LPGPlanner(lpg_location="/home/koldar/Documents/LPGFrancesco/LPG/lpg")

    planner.use_best_first = True
    planner.use_lpg = False
    planner.solutions_to_find = 1
    planner.output_filename = os.path.join(".", "computed.plan")

    logger.info('invoking planner (this may take quite time!)...')
    ret = planner.invoke(
        domain_filename=domain_version1_filename,
        problem_filename=problem_filename,
        working_directory="."
    )
    if ret is False:
        logger.info("planner didn't find any solutions!")
        # the planner didn't generate anything
        raise SolutionNotFoundException(f"Couldn't find solution for problem!")

    logger.info('Solution found! computing the json of it!')

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
    logger.info('Returning json')
    return json_string
