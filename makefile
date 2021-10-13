CC = gcc
t1 = 4-1
t2 = 4-2
all:
	$(CC) $(t1).c -o $(t1)
	$(CC) $(t2).c -o $(t2)
clean:
	rm -f $(t1)
	rm -f $(t2)

