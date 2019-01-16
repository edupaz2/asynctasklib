#include "processor.h"
#include <iostream>

#include "priority_scheduler.h"

template< typename Fn >
boost::fibers::fiber launch( Fn && func, std::string const& name, int priority) {
    boost::fibers::fiber fiber( func);
    PriorityProps & props( fiber.properties< PriorityProps >() );
    props.name = name;
    props.set_priority( priority);
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
            task_id tsk;
            while (boost::fibers::channel_op_status::closed != this->m_taskPool->pop(tsk))
            {
                std::string taskname = std::to_string(tsk);
                std::cout << "Thread poping task " << taskname << std::endl;
                boost::fibers::fiber {
                    launch(yield_fn, taskname, 1)
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
    m_taskPool->push(m_taskCounter);
	return m_taskCounter++;
}

task_id Processor::startTask(const task_type& type)
{
	return m_taskCounter++;
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
