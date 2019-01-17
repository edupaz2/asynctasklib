#include "processor.h"
#include <iostream>

#include "priority_scheduler.h"

std::map<task_id, STaskStatus::status>  taskStatusMap;
std::map<task_id, PriorityProps*>       taskPropsMap;

template< typename Fn >
boost::fibers::fiber launch(Fn && func, std::string const& name, ChannelTask tsk) {
    boost::fibers::fiber fiber( func);
    PriorityProps & props(fiber.properties< PriorityProps >() );
    props.name = name;
    props.set_priority(tsk.priority);// Running

    taskStatusMap[tsk.id] =  STaskStatus::status::running;
    taskPropsMap[tsk.id] = &props;

    return fiber;
}

void yield_fn() {
    std::string name( boost::this_fiber::properties< PriorityProps >().name);
    while(true) {
        //std::cout << "fiber " << name << " yielding" << std::endl;
        boost::this_fiber::yield();
    }
}

Processor::~Processor()
{
	std::cout << "~Processor" << std::endl;
}

void Processor::init()
{
    m_taskCounter = FIRST_TASK_ID;
    m_taskPool = new Channel(1024);
    m_workerThread = new std::thread(
        [this] {
            // Define the scheduling
            // make sure we use our priority_scheduler rather than default round_robin
            boost::fibers::use_scheduling_algorithm< PriorityScheduler >();
            // dequeue and process tasks coming through the buffered channel
            ChannelTask tsk;
            while (boost::fibers::channel_op_status::closed != this->m_taskPool->pop(tsk))
            {
                std::string taskname = std::to_string(tsk.id);
                std::cout << "Thread poping task " << taskname << std::endl;
                boost::fibers::fiber {
                    launch(yield_fn, taskname, tsk)
                }.detach();
            }
        });
}

void Processor::quit()
{
    // TODO: Wait for running fibers with join

    // signal termination
    m_taskPool->close();
    m_workerThread->join();

    delete m_taskPool;
    delete m_workerThread;
}

task_id Processor::startTask()
{
    const ChannelTask ct(m_taskCounter, 5);
    m_taskPool->push(ct);
	return m_taskCounter++;
}

task_id Processor::startTask(const task_type& type)
{
    const ChannelTask ct(m_taskCounter, 5);
    m_taskPool->push(ct);
	return m_taskCounter++;
}

task_id Processor::pauseTask(const task_id& id)
{
    auto search = taskPropsMap.find(id);
    if(search != taskPropsMap.end())
    {
        if(taskStatusMap[id] == STaskStatus::status::running)
        {
            std::cout << "Pause [" << search->first << "]" << std::endl;
            search->second->set_priority(-1);
            taskStatusMap[id] =  STaskStatus::status::paused;

            return id;
        }
    }
    return 0;
}

task_id Processor::resumeTask(const task_id& id)
{
    auto search = taskPropsMap.find(id);
    if(search != taskPropsMap.end())
    {
        if(taskStatusMap[id] == STaskStatus::status::paused)
        {
            std::cout << "Resume [" << search->first << "]" << std::endl;
            search->second->set_priority(10);
            taskStatusMap[id] = STaskStatus::status::running;

            return id;
        }
    }
    return 0;
}

task_id Processor::stopTask(const task_id& id)
{
	 auto search = taskPropsMap.find(id);
    if(search != taskPropsMap.end())
    {
        if(taskStatusMap[id] == STaskStatus::status::paused || taskStatusMap[id] ==  STaskStatus::status::running)
        {
            std::cout << "Resume [" << search->first << "]" << std::endl;
            search->second->set_priority(10);
            taskStatusMap[id] = STaskStatus::status::running;

            return id;
        }
    }
    return 0;
}

task_status Processor::status()
{
	return task_status();
}
