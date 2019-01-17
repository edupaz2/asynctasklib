#ifndef _PRIORITY_SCHEDULER_H_
#define _PRIORITY_SCHEDULER_H_

// Based on the Boost Fiber Library example
// https://www.boost.org/doc/libs/1_69_0/libs/fiber/examples/priority.cpp

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <boost/fiber/all.hpp>

class PriorityProps : public boost::fibers::fiber_properties {
public:
    PriorityProps(boost::fibers::context * ctx);

    int get_priority() const { return m_priority; }
    void set_priority(int p);

    bool is_set_to_stop() const { return m_stop; }
    void set_to_stop();

    std::string name;

private:
    int m_priority = 0;
    bool m_stop = false;
};

class PriorityScheduler :
    public boost::fibers::algo::algorithm_with_properties< PriorityProps > {
private:
    typedef boost::fibers::scheduler::ready_queue_type  rqueue_t;

    rqueue_t                    rqueue_;// Ready fibers
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
};

#endif