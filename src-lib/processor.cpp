#include "processor.h"
#include <iostream>

Processor::~Processor()
{
	std::cout << "~Processor" << std::endl;
}

void Processor::init()
{
}

task_id Processor::startTask()
{
	return FIRST_TASK_ID;
}

task_id Processor::startTask(const task_type& type)
{
	return FIRST_TASK_ID;
}

task_id Processor::pauseTask(const task_id& id)
{
    return FIRST_TASK_ID;
}

task_id Processor::resumeTask(const task_id& id)
{
    return FIRST_TASK_ID;
}

task_id Processor::stopTask(const task_id& id)
{
	return FIRST_TASK_ID;
}

task_status Processor::status()
{
	return task_status();
}

void Processor::quit()
{}
