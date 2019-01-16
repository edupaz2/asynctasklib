#ifndef _ASYNCTASKLIB_DEFS_H_
#define _ASYNCTASKLIB_DEFS_H_

#include <cstdint>
#include <vector>

using task_id = std::uint32_t;
const task_id FIRST_TASK_ID = 10; // [0,9] Error codes. [10, MAXINT] Task ids
using task_type = std::uint32_t;
struct STaskStatus
{
	task_id 	id;
	task_type 	type;
	enum status { paused, running, stopped, completed };

};
using task_status = std::vector<STaskStatus>;

#endif
