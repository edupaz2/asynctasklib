#include "priority_scheduler.h"

#include <iostream>
#include <algorithm>                // std::find_if()

//#define _VERBOSE
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

PriorityProps::PriorityProps(boost::fibers::context* ctx):
    fiber_properties(ctx),
    priority_(0)
{}

int PriorityProps::get_priority() const
{
    return priority_; /*< Provide read access methods at your own discretion. >*/
}

void PriorityProps::set_priority(int p)
{ /*<
    It's important to call `notify()` on any
    change in a property that can affect the
    scheduler's behavior. Therefore, such
    modifications should only be performed
    through an access method. >*/
    // Of course, it's only worth reshuffling the queue and all if we're
    // actually changing the priority.
    if ( p != priority_) {
        priority_ = p;
        notify();
    }
}

// 
PriorityScheduler::PriorityScheduler() :
    rqueue_()
{}

    // For a subclass of algorithm_with_properties<>, it's important to
    // override the correct awakened() overload.
    /*<< You must override the [member_link algorithm_with_properties..awakened]
         method. This is how your scheduler receives notification of a
         fiber that has become ready to run. >>*/
void PriorityScheduler::awakened(boost::fibers::context* ctx, PriorityProps & props) noexcept
{
    int ctx_priority = props.get_priority(); /*< `props` is the instance of
                                               PriorityProps associated
                                               with the passed fiber `ctx`. >*/
    // With this scheduler, fibers with higher priority values are
    // preferred over fibers with lower priority values. But fibers with
    // equal priority values are processed in round-robin fashion. So when
    // we're handed a new context*, put it at the end of the fibers
    // with that same priority. In other words: search for the first fiber
    // in the queue with LOWER priority, and insert before that one.
    rqueue_t::iterator i( std::find_if( rqueue_.begin(), rqueue_.end(),
        [ctx_priority,this]( boost::fibers::context & c)
        { return properties( &c ).get_priority() < ctx_priority; }));
    // Now, whether or not we found a fiber with lower priority,
    // insert this new fiber here.
    rqueue_.insert( i, * ctx);
#ifdef _VERBOSE
    std::cout << "awakened(" << props.name << "): ";
    describe_ready_queue();
#endif
}

    /*<< You must override the [member_link algorithm_with_properties..pick_next]
         method. This is how your scheduler actually advises the fiber manager
         of the next fiber to run. >>*/
boost::fibers::context* PriorityScheduler::pick_next() noexcept
{
    // if ready queue is empty, just tell caller
    if ( rqueue_.empty() ) {
        return nullptr;
    }
    boost::fibers::context * ctx( & rqueue_.front() );
    rqueue_.pop_front();
#ifdef _VERBOSE
    std::cout << "pick_next() resuming " << properties(ctx).name << ": ";
    describe_ready_queue();
#endif
    return ctx;
}

/*<< You must override [member_link algorithm_with_properties..has_ready_fibers]
to inform the fiber manager of the state of your ready queue. >>*/
bool PriorityScheduler::has_ready_fibers() const noexcept
{
    return ! rqueue_.empty();
}

/*<< Overriding [member_link algorithm_with_properties..property_change]
     is optional. This override handles the case in which the running
     fiber changes the priority of another ready fiber: a fiber already in
     our queue. In that case, move the updated fiber within the queue. >>*/
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
#ifdef _VERBOSE
        // hopefully user will distinguish this case by noticing that
        // the fiber with which we were called does not appear in the
        // ready queue at all
        describe_ready_queue();
#endif
        return;
    }

    // Found ctx: unlink it
    ctx->ready_unlink();

    // Here we know that ctx was in our ready queue, but we've unlinked
    // it. We happen to have a method that will (re-)add a context* to the
    // right place in the ready queue.
    awakened( ctx, props);
}


void PriorityScheduler::describe_ready_queue()
{
#ifdef _VERBOSE
    if ( rqueue_.empty() ) {
        std::cout << "[empty]";
    } else {
        const char * delim = "";
        for ( boost::fibers::context & ctx : rqueue_) {
            PriorityProps & props( properties( & ctx) );
            std::cout << delim << props.name << '(' << props.get_priority() << ')';
            delim = ", ";
        }
    }
    std::cout << std::endl;
#endif
}

void PriorityScheduler::suspend_until(std::chrono::steady_clock::time_point const& time_point) noexcept
{
    if ( (std::chrono::steady_clock::time_point::max)() == time_point) {
        std::unique_lock< std::mutex > lk( mtx_);
        cnd_.wait( lk, [this](){ return flag_; });
        flag_ = false;
    } else {
        std::unique_lock< std::mutex > lk( mtx_);
        cnd_.wait_until( lk, time_point, [this](){ return flag_; });
        flag_ = false;
    }
}

void PriorityScheduler::notify() noexcept
{
    std::unique_lock< std::mutex > lk( mtx_);
    flag_ = true;
    lk.unlock();
    cnd_.notify_all();
}

