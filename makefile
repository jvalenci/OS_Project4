unix_metrics: UnixFSMetrics.c
	cc -o unix_metrics UnixFSMetrics.c -lrt

clean: 
	rm unix_metrics 