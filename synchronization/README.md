!!IMPORTANT: When the final zip file is made, name it [GroupName]_classic where group name is our group name

# Classical Synchronization Problem

Group Members: Andrew Kula, Kurt Kuhlman

## Requirements

- make
- gcc

### Makefile Instructions

To run the program, a few helper commands have been implemented:

`make compile`: Compiles both part1 and part2.

`make part1`: Compiles and runs part1; the semaphore stylist problem.

`make part2`: Compiles and runs part2; the monitor stylist problem.

### Part 1

To run part 1, either enter `make part1` into the terminal, or enter `make compile` then `./part1/sleepingStylistSem`. 
Once run, the program will run and the stylist will begin taking on customers.

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
