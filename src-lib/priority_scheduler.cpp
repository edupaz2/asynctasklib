#include "priority_scheduler.h"

#include <iostream>
#include <algorithm>                // std::find_if()

//#define _VERBOSE
//#define _VERBOSE_QUEUES

PriorityProps::PriorityProps(boost::fibers::context* ctx):
    fiber_properties(ctx)
{}

void PriorityProps::set_priority(int p)
{ /*<
    It's important to call `notify()` on any
    change in a property that can affect the
    scheduler's behavior. Therefore, such
    modifications should only be performed
    through an access method. >*/
    // Of course, it's only worth reshuffling the queue and all if we're
    // actually changing the priority.
    if ( p != m_priority )
     {
        m_priority = p;
        notify();
    }
}

void PriorityProps::set_to_stop()
{
    m_stop = true;
    notify();
}

// 
PriorityScheduler::PriorityScheduler() :
    rqueue_()
{}

void PriorityScheduler::awakened(boost::fibers::context* ctx, PriorityProps & props) noexcept
{
    int ctx_priority = props.get_priority(); /*< `props` is the instance of
                                               PriorityProps associated
                                               with the passed fiber `ctx`. >*/

    rqueue_t::iterator i( std::find_if( rqueue_.begin(), rqueue_.end(),
            [ctx_priority,this]( boost::fibers::context & c)
            { return properties( &c ).get_priority() >= ctx_priority; }));
    // Now, whether or not we found a fiber with lower priority,
    // insert this new fiber here.
    rqueue_.insert(i, *ctx);
#ifdef _VERBOSE
    std::cout << "awakened(" << props.name << "-" << ctx_priority << "-" << ctx->ready_is_linked() << "): ";
#endif
#ifdef _VERBOSE_QUEUES
    describe_ready_queue();
    std::cout << std::endl;
#endif
#ifdef _VERBOSE
    std::cout << std::endl;
#endif
}

boost::fibers::context* PriorityScheduler::pick_next() noexcept
{
#ifdef _VERBOSE
    std::cout << "pick_next() ";
#endif

    // if ready queue is empty, just tell caller
    if ( rqueue_.empty() ) {
        return nullptr;
    }

    // Traverse the priorities: the higher the number, the higher the priority
    // Avoid priorities below zero
    boost::fibers::context* ctx(&rqueue_.back());
    // Check below zero priorities
    PriorityProps & props(properties(ctx));
    if(props.get_priority() < 0)
        return nullptr;

    rqueue_.pop_back();
#ifdef _VERBOSE
    std::cout << " resuming " << properties(ctx).name << ": ";
#endif
#ifdef _VERBOSE_QUEUES
    describe_ready_queue();
    std::cout << std::endl;
#endif
#ifdef _VERBOSE
    std::cout << std::endl;
#endif
    return ctx;
}

bool PriorityScheduler::has_ready_fibers() const noexcept
{
    return ! rqueue_.empty();
}

void PriorityScheduler::property_change(boost::fibers::context * ctx, PriorityProps & props) noexcept
{
    // Although our PriorityProps class defines multiple properties, only
    // one of them (priority) actually calls notify() when changed. The
    // point of a property_change() override is to reshuffle the ready
    // queue according to the updated priority value.
#ifdef _VERBOSE
    std::cout << "property_change(" << props.name << '(' << props.get_priority()
              << ")): ";
#endif

    // 'ctx' might not be in our queue at all, if caller is changing the
    // priority of (say) the running fiber. If it's not there, no need to
    // move it: we'll handle it next time it hits awakened().
    if ( ! ctx->ready_is_linked()) { /*<
        Your `property_change()` override must be able to
        handle the case in which the passed `ctx` is not in
        your ready queue. It might be running, or it might be
        blocked. >*/

#ifdef _VERBOSE_QUEUES
        // hopefully user will distinguish this case by noticing that
        // the fiber with which we were called does not appear in the
        // ready queue at all
        describe_ready_queue();
        std::cout << std::endl;
#endif
        return;
    }

    // Found ctx: unlink it
    ctx->ready_unlink();

    if(props.is_set_to_stop())
    {
        //ctx->terminate(); // TODO This is not working here.
    }

    // Here we know that ctx was in our ready queue, but we've unlinked
    // it. We happen to have a method that will (re-)add a context* to the
    // right place in the ready queue.
    awakened(ctx, props);

    notify();
}


void PriorityScheduler::describe_ready_queue()
{
    std::cout << "{Queue: ";
    if ( rqueue_.empty() )
    {
        std::cout << "[empty]";
    }
    else
    {
        const char * delim = "";
        for ( boost::fibers::context & ctx : rqueue_) {
            PriorityProps & props( properties( & ctx) );
            std::cout << delim << props.name << '(' << props.get_priority() << ')';
            delim = ", ";
        }
    }
    std::cout << "}";
}

void PriorityScheduler::suspend_until(std::chrono::steady_clock::time_point const& time_point) noexcept
{
#ifdef _VERBOSE_QUEUES
    std::cout << "suspend_until BEGIN" << std::endl;
#endif
    if ( (std::chrono::steady_clock::time_point::max)() == time_point) {
        std::unique_lock< std::mutex > lk( mtx_);
        cnd_.wait( lk, [this](){ return flag_; });
        flag_ = false;
    } else {
        std::unique_lock< std::mutex > lk( mtx_);
        cnd_.wait_until( lk, time_point, [this](){ return flag_; });
        flag_ = false;
    }
#ifdef _VERBOSE
    std::cout << "suspend_until END" << std::endl;
#endif
}

void PriorityScheduler::notify() noexcept
{
    std::unique_lock< std::mutex > lk( mtx_);
    flag_ = true;
    lk.unlock();
    cnd_.notify_all();
}

