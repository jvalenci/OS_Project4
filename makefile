unix_metrics: UnixFSMetrics.c
	cc -o unix_metrics UnixFSMetrics.c

clean: 
	rm unix_metrics *.o 