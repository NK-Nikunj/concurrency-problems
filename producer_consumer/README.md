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
./dining_philosopher --n-value your_value --prod-value your_value 
--cons-value your_value
```

Replace `your_value` with the desired value. `n-value` is the buffer length, 
`prod-value` is the number of producer threads and `cons-value` is the 
number of consumer threads.