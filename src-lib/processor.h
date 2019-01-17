#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

// Based on the Boost Fiber Library
/* Tasks can be transferred via channels.
 * The worker thread runs a pool of fibers that dequeue and executed tasks from the channel.
 * The termination is signalled via closing the channel.
 */
// https://www.boost.org/doc/libs/1_69_0/libs/fiber/doc/html/fiber/worker.html#fiber.worker.processing_tasks

#include "asynctasklib_defs.h"

#include <boost/fiber/buffered_channel.hpp>

struct ChannelTask
{
	ChannelTask() {}
	ChannelTask(task_id tid, int p) : id(tid), priority(p) {}
	task_id id;
	int 	priority;
};

class Processor
{
public:
	virtual 		~Processor();

	void			init		();
	void			quit		();

    task_id 		startTask	();
	task_id 		startTask	(const task_type& type);

	task_id			pauseTask	(const task_id& id);
	task_id			resumeTask	(const task_id& id);
	task_id			stopTask	(const task_id& id);

	task_status_v	status 		();

private:
	task_id			m_taskCounter;

	typedef boost::fibers::buffered_channel<ChannelTask> Channel;

	// Worker thread accepting tasks through the buffered channel
	std::thread* 	m_workerThread;
	// Buffered channel to send task to the worker thread
	Channel* 	 	m_taskPool;
};

#endif