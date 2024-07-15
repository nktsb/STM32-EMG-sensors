#ifndef __CLOCK_H__
#define __CLOCK_H__

typedef int clock_time_t;
extern  clock_time_t  current_time;

#define clock_time() current_time
#define clock_tick_cb() current_time++

void clock_init(void);

#endif /* __CLOCK_H__ */
