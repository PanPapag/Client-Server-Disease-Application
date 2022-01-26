# Client-Server-Disease-Application

The aim of this project is to demonstrate the utility of multithreaded programming and internetworking with sockets. We implement a distributed system that will provide the same functionality as the [Disease-Aggregator-Application](https://github.com/PanPapag/Disease-Aggregator-Application). In particular, we implement the following three programs: 

1. A ```master``` program that creates a series of ```Worker``` processes.
2. A multithreaded server program ```whoServer``` which collects network summary statistics from ```Worker``` processes and queries from ```clients```
3. A multithreaded client program ```whoClient``` which creates many threads, where each thread plays the role of a client sending queries to ```whoServer```.

![](system_design.png)

## Master 

The program will be called ```master``` and will be used as follows:

```./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir```

where:

* The ```numWorkers``` parameter is the number of Worker processes that the application will generate.
* The ```bufferSize``` parameter is the size of the buffer for reading over the pipes.
* The ```serverIP``` parameter is the IP address of the whoServer to which the Worker processes will connect to
send him the summary statistics.
* The ```serverPort``` parameter is the port number where the whoServer listens.
* The ```input_dir``` parameter is a directory that contains subdirectories with the files that Workers process. Similar to [Disease-Aggregator-Application](https://github.com/PanPapag/Disease-Aggregator-Application), each subdirectory has a country name and contains files with names that are all DD-MM-YYYY format.

In the begining, the ```master``` program starts ```numWorkers``` Workers child processes and distribute the subdirectories evenly with the countries inside ```input_dir``` to Workers. Specifically, it starts the Workers and informs each one of them via named pipe about the subdirectories that the they will undertake. The parent processes also sends via named pipe the ```IP address``` and the ```port number``` of ```whoServer```. When the creation of the Worker processes is finished, the parent process remains alive to fork a new Worker process in case an existing one suddenly terminates.

Each Worker process, for each directory assigned to it, reads all its files in chronological order based on the file names and creates a series of data structures that it will use to answer questions that will be forwarded to it by whoServer. Then, it connects to whoServer and sends the following information:
1. A port number where the Worker process listens the questions that will will be forwarded to it by whoServer.
2. The summary statistics (same as [Disease-Aggregator-Application](https://github.com/PanPapag/Disease-Aggregator-Application)).

When the Worker process finishes transmitting information to whoServer, it will listen to the port number it has selected and wait for connections from whoServer for requests to the countries it manages. The signal handling remains the same as [Disease-Aggregator-Application](https://github.com/PanPapag/Disease-Aggregator-Application).
