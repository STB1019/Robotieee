import os
import typing
import subprocess
from planner_wrapper import planner


class CallProgram:
    def __init__(self, exit_status: int, stdout: str, stderr: str):
        self.exit_status = exit_status
        self.stdout = stdout
        self.stderr = stderr


def call_program(program: typing.List[str], working_directory: str = os.path.abspath(".")):
    working_directory = os.path.abspath(working_directory)
    print('CWD is "{}"'.format(working_directory))
    print('Executing "{}"'.format(' '.join(program)))
    with subprocess.Popen(program, cwd=working_directory, stdout=subprocess.PIPE) as proc:
        return CallProgram(
            exit_status=proc.returncode,
            stdout=str(proc.stdout.read(), 'utf8') if proc.stdout is not None else "",
            stderr=str(proc.stderr.read(), 'utf8') if proc.stderr is not None else ""
        )


