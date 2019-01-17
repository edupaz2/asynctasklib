#ifndef _ASYNCTASKLIB_DEFS_H_
#define _ASYNCTASKLIB_DEFS_H_

#include <cstdint>
#include <vector>
#include <string>
#include <sstream>

using task_id = std::uint32_t;
const task_id FIRST_TASK_ID = 10; // [0,9] Error codes. [10, MAXINT] Task ids
using task_type = std::uint32_t;
struct TaskStatus
{
	task_id 	id;
	task_type 	type;
	enum task_status { paused, running, stopped, completed };
	task_status status;
	std::string toStr() const
	{
		std::ostringstream oss;
  		oss << "[" << id << "] = ";
  		switch(status)
  		{
  			case paused:
  				oss << "PAUSED";
  				break;
  			case running:
  				oss << "RUNNING";
  				break;
  			case stopped:
  				oss << "STOPPED";
  				break;
  			case completed:
  				oss << "COMPLETED";
  				break;
  		}
		return oss.str();
	}
};
using task_status_v = std::vector<TaskStatus>;
#endif
