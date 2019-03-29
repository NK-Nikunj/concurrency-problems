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

class Dining_philosopher
{
public:
    Dining_philosopher(int n) :num_phil(n)
    {
        std::srand(std::time(0));
        
        for(auto i = 0; i < n; ++i)
        {
            chopsticks.push_back(true);

            std::unique_ptr<hpx::lcos::local::mutex> _mu = 
                            std::make_unique<hpx::lcos::local::mutex>();
            mu.emplace_back(std::move(_mu));
        }
    }

private:
    bool generate_random_number()
    {
        return (std::rand() % 2) ? false : true;
    }

    bool check_left(int i)
    {
        std::lock_guard<hpx::lcos::local::mutex> mu_left(*(mu.at(i)));
        bool val = chopsticks[i];
        if(val == true)
            chopsticks[i] == false;
        
        return val;
    }

    bool check_right(int i)
    {
        std::lock_guard<hpx::lcos::local::mutex> mu_right(*(mu.at(i)));
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

        while(!should_stop)
        {
            bool wishes_to_eat = generate_random_number();

            if(wishes_to_eat)
            {
                {
                    std::lock_guard<hpx::lcos::local::mutex> lock(print);
                    char const* fmt1 = "Philosopher {1} wishes to eat, it will now try\n";
                    hpx::util::format_to(std::cout, fmt1, index);
                }

                bool is_left_clear, is_right_clear;
                do {
                    is_left_clear = check_left(left);
                    is_right_clear = check_right(right);

                    if(is_left_clear && is_right_clear)
                        eat();

                } while(is_left_clear == false || is_right_clear == false);

                {
                    std::lock_guard<hpx::lcos::local::mutex> lock(print);
                    char const* fmt2 = "Philosopher {1} has eaten well, it will now think\n";
                    hpx::util::format_to(std::cout, fmt2, index);
                }

                chopsticks[left] = true;
                chopsticks[right] = true;
            }        
        }
    }

    void make_stop()
    {
        std::cin.get();
        should_stop = true;
    }

public:
    void start_simulation()
    {
        for(auto i = 0; i < num_phil; ++i)
        {
            threads.emplace_back(std::make_unique<hpx::thread>(&Dining_philosopher::simulate, this, i));
        }

        threads.emplace_back(std::make_unique<hpx::thread>(&Dining_philosopher::make_stop, this));

        for(auto i = 0; i < num_phil+1; ++i)
        {
            threads[i] -> join();
        }

    }

private:
    int num_phil;
    std::vector<bool> chopsticks;
    std::vector<std::unique_ptr<hpx::thread>> threads;
    std::vector<std::unique_ptr<hpx::lcos::local::mutex>> mu;
    hpx::lcos::local::mutex print;
    bool should_stop = false;
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