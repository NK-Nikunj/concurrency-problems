# Building

Run the following code:

```
mkdir build && cd build
cmake ..
make
```

If the program built succesfully, You should see a binary named 
`sleeping_barber`. To run:

```
./sleeping_barber
```

This would run a simulation of sleeping barber problem with 10 customers 
arriving at the shop, 50ms of maximum time between entry of any two 
customers, 50ms of time to cut hair and maximum of 3 customers on waiting 
chairs. 
If you want to simulate with a different number of philosophers run:
```
./dining_philosopher --n-value your_value --wait-time your_value 
--cut-time your_value --queue-length
```

Replace `your_value` with the desired value. `n-value` is the customer count, 
`wait-time` is the maximum time between 2 customers, `cut-time` is the 
time taken by barber to cut hair and `queue-length` is the number of 
customers allowed to wait in the waiting area.