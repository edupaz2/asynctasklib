#ifndef _PRIORITY_SCHEDULER_H_
#define _PRIORITY_SCHEDULER_H_

// Based on the Boost Fiber Library example
// https://www.boost.org/doc/libs/1_69_0/libs/fiber/examples/priority.cpp

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <boost/fiber/all.hpp>
#include <map>

class PriorityProps : public boost::fibers::fiber_properties {
public:
    PriorityProps(boost::fibers::context * ctx);

    int get_priority() const;

    void set_priority(int p);

    std::string name;

private:
    int priority_;
};

class PriorityScheduler :
    public boost::fibers::algo::algorithm_with_properties< PriorityProps > {
private:
    typedef boost::fibers::scheduler::ready_queue_type  rqueue_t;
    typedef std::map<boost::fibers::context*, int>      pmap_t;

    rqueue_t                    rqueue_;// Ready fibers
    pmap_t                      pmap_;// Paused fibers map
    std::mutex                  mtx_{};
    std::condition_variable     cnd_{};
    bool                        flag_{ false };

public:
    PriorityScheduler();

    virtual void awakened(boost::fibers::context* ctx, PriorityProps & props) noexcept;
    virtual boost::fibers::context* pick_next() noexcept;
    virtual bool has_ready_fibers() const noexcept;
    virtual void property_change(boost::fibers::context* ctx, PriorityProps & props) noexcept;
    void suspend_until(std::chrono::steady_clock::time_point const& time_point) noexcept;
    void notify() noexcept;

    void describe_ready_queue();
    void describe_paused_map();
};

#endif