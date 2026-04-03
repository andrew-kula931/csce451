SEM_FILE = ./part1/sleepingStylistSem.c
SEM_EXEC = ./part1/sleepingStylistSem
MON_FILE = ./part2/sleepingStylistMon.c
MON_EXEC = ./part2/sleepingStylistMon

.PHONY: part1 part2

# Compiles all files
compile: $(SEM_FILE) $(MON_FILE)
	gcc -o $(SEM_EXEC) $(SEM_FILE)
	gcc -o $(MON_EXEC) $(MON_FILE)

# Compiles and runs part1: the semaphore run
part1: $(SEM_FILE)
	@gcc -o $(SEM_EXEC) $(SEM_FILE)
	@$(SEM_EXEC)

# Compiles and runs part2: the monitor run
part2: $(MON_FILE)
	@gcc -o $(MON_EXEC) $(MON_FILE)
	@$(MON_EXEC)