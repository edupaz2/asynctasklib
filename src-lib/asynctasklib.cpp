#include "asynctasklib.h"

#include "singleton.h"
#include "processor.h"

struct ProcessorSingleton : public Singleton<Processor, ProcessorSingleton>
{
	// Call the Singleton constructor with an int for it to invoke your static init() method.
	ProcessorSingleton() : Singleton(1) {}
	static Processor* init()
	{
		Processor* p = new Processor();
		p->init();
		return p;
	}
};

ProcessorSingleton processor;

task_id startTask()
{
	return processor->startTask();
}

task_id startTask(const task_type& type)
{
	return processor->startTask(type);
}

task_id	pauseTask(const task_id& id)
{
	return processor->pauseTask(id);
}

task_id	resumeTask(const task_id& id)
{
	return processor->resumeTask(id);
}

task_id	stopTask(const task_id& id)
{
	return processor->stopTask(id);
}

task_status	status()
{
	return processor->status();
}

void quit()
{
	processor->quit();
}
