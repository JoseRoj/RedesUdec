#include "rtt.h"

#include <stdlib.h>
#include <sys/time.h>

#define RTT_RTOCALC(rtt) ((rtt)->srtt + (4.0 * (rtt)->rttvar))

static float rtt_minmax(float rto) {
    if (rto < RTT_RXTMIN) {
        rto = RTT_RXTMIN;
    } else if (rto > RTT_RXTMAX) {
        rto = RTT_RXTMAX;
    }

    return rto;
}

void rtt_init(struct rtt_t* rtt) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    rtt->base = tv.tv_sec;

    rtt->rtt = 0.0;
    rtt->srtt = 0.0;
    rtt->rttvar = 0.75;
    rtt->rto = rtt_minmax(RTT_RTOCALC(rtt));
}

uint32_t rtt_ts(struct rtt_t* rtt) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return ((tv.tv_sec - rtt->base) * 1000) + (tv.tv_usec / 1000);
}

void rtt_newpack(struct rtt_t* rtt) {
    rtt->nrexmt = 0;
}

int rtt_start(struct rtt_t* rtt) {
    return (int)(rtt->rto + 0.5);
}

void rtt_stop(struct rtt_t* rtt, uint32_t ms) {
    rtt->rtt = ms / 1000.0;

    double delta = rtt->rtt - rtt->srtt;
    rtt->srtt += delta / 8.0;

    if (delta < 0.0) {
        delta = -delta;
    }

    rtt->rttvar += (delta - rtt->rttvar) / 4;
    rtt->rto = rtt_minmax(RTT_RTOCALC(rtt));
}

int rtt_timeout(struct rtt_t* rtt) {
    rtt->rto *= 2;

    if (++rtt->nrexmt > RTT_MAXNREXMT) {
        return -1;
    }

    return 0;
}
