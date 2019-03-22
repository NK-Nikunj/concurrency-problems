////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Nikunj Gupta
////////////////////////////////////////////////////////////////////////////////


#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/util.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

///////////////////////////////////////////////////////////////////////////////
// Main function taking care of dining philosopher problem
// struct Philosopher;

// void dining_philospher(std::shared_ptr<Philosopher>,
//                        int,
//                        int,
//                        std::unique_ptr<hpx::lcos::local::mutex*>&);

// Protocols to check left and right
// bool check_left(int, std::unique_ptr<hpx::lcos::local::mutex*>&);
// bool check_right(int, std::unique_ptr<hpx::lcos::local::mutex*>&);

// Generate random bool to check if the philosopher wishes to eat
// bool generate_random_bool();
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// The Philosopher struct
// Stores whether a Philosopher is eating or thinking
// struct Philosopher{

//     // Is the philosopher eating
//     bool is_eating;

//     // Is the philosopher thinking
//     bool is_thinking;

//     Philosopher() :is_eating(false), is_thinking(true) {}

// };

class Dining_philosopher
{
public:
    Dining_philosopher(int n) :num_phil(n)
    {
        for(auto i = 0; i < n; ++i)
        {
            // philosophers.push_back(
            //     std::make_shared<Philosopher>(new Philosopher())
            // );
            // philosophers.push_back(i);

            chopsticks.push_back(true);
        }

        std::unique_ptr<hpx::lcos::local::mutex[]> cp(new hpx::lcos::local::mutex[n]);

        mu = std::move(cp);
    }

private:
    bool generate_random_number()
    {
        std::srand(std::time(0));

        return (std::rand() % 2) ? false : true;
    }

    bool check_left(int i)
    {
        std::lock_guard<hpx::lcos::local::mutex> mu_left(mu[i]);
        bool val = chopsticks[i];
        if(val == true)
            chopsticks[i] == false;
        
        return val;
    }

    bool check_right(int i)
    {
        std::lock_guard<hpx::lcos::local::mutex> mu_right(mu[i]);
        // auto mu_right(mu[i]);
        bool val = chopsticks[i];
        if(val == true)
            chopsticks[i] == false;
        
        return val;
    }

    void eat()
    {
        // Let the philosopher eat for 1s
        hpx::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void simulate(int index)
    {
        int left = (index + num_phil - 1) % num_phil;
        int right = (index + 1) % num_phil;

        while(1)
        {
            bool wishes_to_eat = generate_random_number();

            if(wishes_to_eat)
            {
                char const* fmt1 = "Philosopher {1} wishes to eat, it will now try";
                hpx::util::format_to(std::cout, fmt1, index);

                bool is_left_clear, is_right_clear;
                do {
                    is_left_clear = check_left(left);
                    is_right_clear = check_right(right);

                    if(is_left_clear && is_right_clear)
                        eat();
                }while(is_left_clear == false || is_right_clear == false);

                char const* fmt2 = "Philosopher {1} has eaten well, it will now think";
                hpx::util::format_to(std::cout, fmt2, index);


                chopsticks[left] = true;
                chopsticks[right] = true;
            }        
        }
    }

public:
    void start_simulation()
    {
        for(auto i = 0; i < num_phil; ++i)
            {
                hpx::thread t([i, this] {simulate(i);});
                threads.push_back(std::move(t));
            } 
    }

private:
    int num_phil;
    // std::vector<int> philosophers;
    std::vector<bool> chopsticks;
    std::vector<hpx::thread> threads;
    std::unique_ptr<hpx::lcos::local::mutex[]> mu;
    // hpx::lcos::local::mutex mu*;
};


///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    // Extract the n-value i.e. Number of philosophers
    int n = vm["n-value"].as<int>();

    {
        Dining_philosopher simulate_problem(n);

        simulate_problem.start_simulation();
    }

    // Shutdown HPX
    return hpx::finalize();
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Configure application-specific options
    boost::program_options::options_description
        desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");
    
    desc_commandline.add_options()
        (
            "n-value",
            boost::program_options::value<int>()->default_value(5),
            "Number of Philosophers around the Dining Table"
        );
    
    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}