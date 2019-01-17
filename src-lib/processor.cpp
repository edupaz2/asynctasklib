#include "processor.h"
#include <iostream>

#include "priority_scheduler.h"

#ifdef _VERBOSE
class Verbose {
public:
    Verbose( std::string const& d, std::string const& s="stop") :
        desc( d),
        stop( s) {
        std::cout << desc << " start" << std::endl;
    }

    ~Verbose() {
        std::cout << desc << ' ' << stop << std::endl;
    }

    Verbose( Verbose const&) = delete;
    Verbose & operator=( Verbose const&) = delete;

private:
    std::string     desc;
    std::string     stop;
};
#endif

std::map<task_id, TaskStatus::task_status>  taskStatusMap;
std::map<task_id, PriorityProps*>           taskPropsMap;

template< typename Fn >
boost::fibers::fiber launch(Fn && func, std::string const& name, ChannelTask tsk) {
    boost::fibers::fiber fiber(func);
    PriorityProps & props(fiber.properties< PriorityProps >() );
    props.name = name;
    props.set_priority(tsk.priority);// Running

    taskStatusMap[tsk.id] =  TaskStatus::task_status::running;
    taskPropsMap[tsk.id] = &props;

    return fiber;
}

void yield_fn()
{
    PriorityProps & props = boost::this_fiber::properties< PriorityProps >();
    std::string name(props.name);
#ifdef _VERBOSE
    // This Verbose obj will call the dtor when leaving the function
    // and it will leave a trace
    Verbose v("---> yield_fn I'm outta here", "stop\n");
#endif
    while(!props.is_set_to_stop()) {
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
                //std::cout << "Thread poping task " << taskname << std::endl;
                boost::fibers::fiber {
                    launch(yield_fn, taskname, tsk)
                }.detach();
            }
        });
}

void Processor::quit()
{
    // TODO: Wait for running fibers with join
    auto it = taskPropsMap.begin();
    const auto itEnd = taskPropsMap.end();
    while( it != itEnd )
    {
        it->second->set_priority(1);
        it->second->set_to_stop();
        ++it;
    }

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
        if(taskStatusMap[id] == TaskStatus::task_status::running)
        {
            std::cout << "Pause [" << search->first << "]" << std::endl;
            search->second->set_priority(-1);
            taskStatusMap[id] =  TaskStatus::task_status::paused;

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
        if(taskStatusMap[id] == TaskStatus::task_status::paused)
        {
            std::cout << "Resume [" << search->first << "]" << std::endl;
            search->second->set_priority(5);
            taskStatusMap[id] = TaskStatus::task_status::running;

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
        if(taskStatusMap[id] == TaskStatus::task_status::paused || taskStatusMap[id] ==  TaskStatus::task_status::running)
        {
            std::cout << "Stop [" << search->first << "]" << std::endl;
            search->second->set_to_stop();
            taskStatusMap[id] = TaskStatus::task_status::stopped;

            return id;
        }
    }
    return 0;
}

task_status_v Processor::status()
{
    task_status_v vc;
    auto it = taskStatusMap.begin();
    const auto itEnd = taskStatusMap.end();
    while( it != itEnd )
    {
        TaskStatus st;
        st.id = it->first;
        st.status = it->second;
        vc.push_back(st);
        ++it;
    }
	return vc;
}
