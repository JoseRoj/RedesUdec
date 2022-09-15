#ifndef RTT_H
#define RTT_H

#include <stdint.h>

struct rtt_t {
    double rtt;
    double srtt;
    double rttvar;
    double rto;
    int nrexmt;
    uint32_t base;
};

#define RTT_RXTMIN 0.5
#define RTT_RXTMAX 60
#define RTT_MAXNREXMT 5

void rtt_init(struct rtt_t* rtt);
void rtt_newpack(struct rtt_t* rtt);
int rtt_start(struct rtt_t* rtt);
void rtt_stop(struct rtt_t* rtt, uint32_t ms);
int rtt_timeout(struct rtt_t* rtt);
uint32_t rtt_ts(struct rtt_t* rtt);

#endif  // RTT_H
