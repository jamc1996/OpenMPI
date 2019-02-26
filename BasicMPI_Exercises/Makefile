
CC = mpicc
CFLAGS = -Wall

EXECS = vec_ed vec_ed_coll decompose euc_norm euc_norm_fully_avail space_efficient dotcoll WriteBigVector

all: $(EXECS) 

vec_ed: 1-1vec_ed.c
	module load cports openmpi; \
	$(CC) $(CFLAGS) $< -o $@

vec_ed_coll: 1-2vec_ed_coll.c
	module load cports openmpi; \
	$(CC) $(CFLAGS) $< -o $@

decompose: 2decompose.c
	module load cports openmpi; \
	$(CC) $(CFLAGS) $< -o $@

euc_norm: 3euc_norm.c
	module load cports openmpi; \
	$(CC) $(CFLAGS) $< -o $@ -lm

euc_norm_fully_avail: 4euc_norm_fully_avail.c
	module load cports openmpi; \
	$(CC) $(CFLAGS) $< -o $@ -lm

space_efficient: 5space_efficient.c
	module load cports openmpi; \
	$(CC) $(CFLAGS) $< -o $@ -lm

dotcoll: 6dotcoll.c
	module load cports openmpi; \
	$(CC) $(CFLAGS) $< -o $@

WriteBigVector: WriteBigVector.c
	gcc $(CFLAGS) $< -o $@

.PHONY: clean
clean: 
	$(RM) *.o $(EXECS) a.out BigVector.txt output.txt collective_output.txt

test: $(EXECS)
	module load cports openmpi; \
	mpiexec -n 4 $(EXECS) 
