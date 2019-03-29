# Building

Run the following code:

```
mkdir build && cd build
cmake ..
make
```

If the program built succesfully, You should see a binary named 
`readers_writers`. To run:

```
./readers_writers
```

This would run a simulation of readers writers problem with buffer 
length of 10 and 5 consumer and producer threads. 
If you want to simulate with a different number of philosophers run:
```
./readers_writers --n-value your_value --write-value your_value 
--read-value your_value
```

Replace `your_value` with the desired value. `n-value` is the buffer length, 
`write-value` is the number of writer threads and `read-value` is the 
number of reader threads.

### Note

HPX is a task based execution system which expects work to yield via a HPX 
synchronization primitive or complete. Thus the HPX solution CAN sometimes 
result in a deadlock due to internal thread scheduling policies.

If you experience something similar try running the standard C++14 concurrency 
port of the HPX code. Run:

```
readers_writers_standard_cpp
```