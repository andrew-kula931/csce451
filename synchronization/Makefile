SEM_FILE  = ./part1/sleepingStylistSem.c
SEM_EXEC  = ./part1/sleepingStylistSem
MON_SRC   = ./part2/sleepingStylistMon.c
MON_C     = ./part2/monitor.c
MON_OBJ   = ./part2/monitor.o
MON_EXEC  = ./part2/sleepingStylistMon

.PHONY: compile part1 part2 clean

# Compiles both parts
compile: $(SEM_EXEC) $(MON_EXEC)

$(SEM_EXEC): $(SEM_FILE)
	gcc -pthread -o $(SEM_EXEC) $(SEM_FILE)

$(MON_OBJ): $(MON_C) ./part2/monitor.h
	gcc -c -o $(MON_OBJ) $(MON_C)

$(MON_EXEC): $(MON_SRC) $(MON_OBJ) ./part2/monitor.h
	gcc -pthread -o $(MON_EXEC) $(MON_SRC) $(MON_OBJ)

# Compiles and runs part1: the semaphore solution
part1: $(SEM_EXEC)
	@$(SEM_EXEC)

# Compiles and runs part2: the monitor solution
part2: $(MON_EXEC)
	@$(MON_EXEC)

clean:
	rm -f $(SEM_EXEC) $(MON_EXEC) $(MON_OBJ)
