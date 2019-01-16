#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include "asynctasklib_defs.h"

class Processor
{
public:
	virtual 		~Processor();

	void			init		();

    task_id 		startTask	();
	task_id 		startTask	(const task_type& type);

	task_id			pauseTask	(const task_id& id);
	task_id			resumeTask	(const task_id& id);
	task_id			stopTask	(const task_id& id);

	task_status		status 		();

	void			quit		();
};

#endif