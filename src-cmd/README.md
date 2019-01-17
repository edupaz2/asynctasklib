# asynctasklib-cmd
Command line application for using Asynchtasklib


The program accepts the following arguments:
- ```./program --help``` prints help message and instructions
- ```./program``` starts and waits for instructions

Once the program is running, it should read instructions from the standard input. These instructions should have the following format:
- ```start``` starts a task and prints its ID
- ```start <task_type_id>``` (optional, nice to have) If more than one task type is supported, starts a
task of a given type and and prints its ID
- ```pause <task_id>``` pause the task with the given id and print a confirmation message
- ```resume <task_id>``` resume task with the given id (if paused) and print a confirmation message
- ```stop <task_id>``` stop the task with the given id (if not stopped) and print a confirmation message
- status prints the id, the status (paused, running, stopped, completed) and an optional
indicator of progress for each task. If the application supports multiple task types, prints the
task type ID.
- status <task_id> As above, but for a single task.
- quit gracefully shut down