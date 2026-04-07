!!IMPORTANT: When the final zip file is made, name it [GroupName]\_classic where group name is our group name

# Classical Synchronization Problem

Group Members: Andrew Kula, Kurt Kuhlman, Boston Bailey

## Requirements

- make
- gcc

### Makefile Instructions

To run the program, a few helper commands have been implemented:

`make compile`: Compiles both part1 and part2.

`make part1`: Compiles and runs part1; the semaphore stylist problem.

`make part2`: Compiles and runs part2; the monitor stylist problem.

`make clean`: Removes the executable files generated from part1 and part2.

### Part 1

To run part 1, either enter `make part1` into the terminal, or enter `make compile` then `./part1/sleepingStylistSem`.
Once run, the program will run and the stylist will begin taking on customers.

The signal-and-continue behavior can be verified by watching when haircuts are finished and when new customers are
occupying the available seating. The max seating is set to 6, so therefore, any case when a customer attempts to take
a seat beyond that threshold will result in the output dialogue "No available seating. Going shopping.". All other cases
will increase the number of seated customers by one (and exclusively one).

The output should look something like this:

> Customer 5 takes a seat: Currently waiting: 1  
> Haircut finished  
> Customer 6 takes a seat: Currently waiting: 1  
> Customer 7 takes a seat: Currently waiting: 1  
> Haircut finished  
> Customer 8 takes a seat: Currently waiting: 1  
> Customer 9 takes a seat: Currently waiting: 2  
> Customer 10 takes a seat: Currently waiting: 3  
> Haircut finished  
> Haircut finished  
> Haircut finished  
> Customer 11 takes a seat: Currently waiting: 1  
> Haircut finished  
> Customer 12 takes a seat: Currently waiting: 1  
> Customer 13 takes a seat: Currently waiting: 2

**Time spent:** 12 hours

**Challenge level:** 3/5

### Part 2

To run part 2, either enter `make part2` into the terminal, or enter `make compile` then `./part2/sleepingStylistMon`.

The output will show the state of the waiting chairs before each action, followed by messages indicating when the stylist sleeps and wakes up, and when customers wait for the stylist.

The output should look something like this:

> Salon empty = 1 times
> [Customer] Salon full, going shopping (salon_full=1054)
> |1|1|1|1|1|1| => 6
> Given haircuts = 15
> Salon full = 1054 times
> Salon empty = 1 times
> [Customer] Salon full, going shopping (salon_full=1055)
> |1|1|1|1|1|1| => 6
> Given haircuts = 15
> Salon full = 1055 times
> Salon empty = 1 times
> [Customer] Salon full, going shopping (salon_full=1056)
> [Customer] Salon full, going shopping (salon_full=1057)

#### Observing the signal-and-continue discipline

The key lines to look for are:

1. `[Stylist] Woke up, resuming inside monitor (signal-and-continue confirmed)` — after a customer signals the stylist, the stylist re-enters the monitor and continues from where it left off. If this were signal-and-wait, the customer would run first instead.

2. `[Customer] Stylist ready, resuming inside monitor (signal-and-continue confirmed)` — after the stylist signals a customer, the customer competes to re-enter the monitor via the entry queue. The stylist continues running first (signal-and-continue), and only after the stylist exits does the customer resume.

In both cases, the **signaler continues** inside the monitor and the **woken thread waits at the entry queue** — confirming signal-and-continue behavior.

**Time spent:** 15 hours

**Challenge level:** 4/5
