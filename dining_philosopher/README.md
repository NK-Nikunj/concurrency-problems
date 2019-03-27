# Building

Run the following code:

```
mkdir build && cd build
cmake ..
make
```

If the program built succesfully, You should see 2 binary named 
`dining_philosopher` and `dining_philospher_trivial`. To run:

```
./dining_philosopher
```

This would run a simulation of dining philospher problem with 5 philosophers. 
If you want to simulate with a different number of philosophers run:
```
./dining_philosopher --n-value your_value
```

Replace `your_value` with the desired value.