# Client-Server-Disease-Application

The aim of this project is to demonstrate the utility of multithreaded programming and internetworking with sockets. We implement a distributed system that will provide the same functionality as the [Disease-Aggregator-Application](https://github.com/PanPapag/Disease-Aggregator-Application). In particular, we implement the following three programs: 

1. A ```master``` program that creates a series of ```Worker``` processes.
2. A multithreaded server program ```whoServer``` which collects network summary statistics from ```Worker``` processes and queries from ```clients```
3. A multithreaded client program ```whoClient``` which creates many threads, where each thread plays the role of a client sending queries to ```whoServer```.


