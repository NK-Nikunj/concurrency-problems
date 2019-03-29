# Building

Run the following code:

```
mkdir build && cd build
cmake ..
make
```

If the program built succesfully, You should see a binary named 
`producer_consumer`. To run:

```
./producer_consumer
```

This would run a simulation of producer consumer problem with buffer 
length of 10 and 5 consumer and producer threads. 
If you want to simulate with a different number of philosophers run:
```
./producer_consumer --n-value your_value --prod-value your_value 
--cons-value your_value
```

Replace `your_value` with the desired value. `n-value` is the buffer length, 
`prod-value` is the number of producer threads and `cons-value` is the 
number of consumer threads.

### Note

HPX is a task based execution system which expects work to yield via a HPX 
synchronization primitive or complete. Thus the HPX solution CAN sometimes 
result in a deadlock due to internal thread scheduling policies.

If you experience something similar try running the standard C++14 concurrency 
port of the HPX code. Run:

```
producer_consumer_standard_cpp
```