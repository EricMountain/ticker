#include <iostream>
#include <cerrno>
#include <cstring>
#include <map>
#include <time.h>

clockid_t clocks[] = { CLOCK_REALTIME, CLOCK_MONOTONIC };

std::map<clockid_t, struct timespec > clock_states;

const std::string clock2string(clockid_t clock) {
    switch (clock) {
    case 0:
        return "REALTIME";
    case 1:
        return "MONOTONIC";
    }
    return "UNKNOWN";
}

void resolutions()
{
    struct timespec spec;
    for (clockid_t clock : clocks) {
        if (clock_getres(clock, &spec)) {
            std::cerr << clock2string(clock) << "error getting clock resolution" << std::endl;
            exit(1);
        }
        std::cout << clock << ": " << spec.tv_sec << "\" " << spec.tv_nsec << "ns" << std::endl;
    }
}

void do_sleep(int interval_ms) {
    struct timespec sleep = { interval_ms / 1000, (interval_ms % 1000) * 1000000 };
    while (1) {
        int nano_res = nanosleep(&sleep, &sleep);
        if (nano_res == 0) {
            return;
        }
        if (errno == EINVAL) {
            std::cerr << "nanosleep EINVAL: " << std::strerror(errno) << std::endl;
            exit(1);
        }
        std::cerr << "My sleep was disturbed, trying again" << std::endl;
    }
}

long int timedelta(const struct timespec start, const struct timespec end) {
    long int lstart = start.tv_sec * 1000000000 + start.tv_nsec;
    long int lend = end.tv_sec * 1000000000 + end.tv_nsec;
    return lend - lstart;
}

void tick(int interval_ms)
{
    for (clockid_t clock : clocks) {
        struct timespec* current = new struct timespec;
        int res = clock_gettime(clock, current);
        if (res != 0) {
            std::cerr << "clock_gettime error: " << std::strerror(errno) << std::endl;
            exit(1);
        }
        if (clock_states.count(clock) != 0) {
            long int delta_ns = timedelta(clock_states[clock], *current);
            long int interval_ns = interval_ms * 1000000;
            long int tick_delta_ns = delta_ns - interval_ns;
            bool warn = (tick_delta_ns > interval_ns);
            std::cout << clock2string(clock) << ": " << current->tv_sec << "\" " << current->tv_nsec
                      << "ns, delta = " << delta_ns
                      << ", tick_delta = " << tick_delta_ns << "ns "
                      << (warn ? "WARNING" : "OK")
                      << std::endl;
            clock_states.erase(clock);
        }
        clock_states[clock] = *current;
        delete current;
    }
    do_sleep(interval_ms);
}

void ticker(int interval_ms)
{
    while (1) {
        tick(interval_ms);
    }
}


int main()
{
    int interval_ms = 10;
    resolutions();
    ticker(interval_ms);
}
