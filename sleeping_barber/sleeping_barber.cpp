///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Nikunj Gupta
///////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/util.hpp>

#include <iostream>
#include <queue>
#include <memory>

class Barber
{
public:
    bool is_barber_cutting();
    void cut_hair();    
    void enter_shop(int);
    void simulate(int, int);

    Barber(int ct, int ql) : cut_time(ct), queue_length(ql) {}

private:
    std::queue<int> customer_id;
    volatile bool is_sleeping = true;
    int cut_time;
    int queue_length;

    hpx::lcos::local::mutex check, cut, print;
};

void Barber::cut_hair()
{
    hpx::this_thread::sleep_for(std::chrono::milliseconds(cut_time));
}

void Barber::enter_shop(int id)
{
    {
        std::lock_guard<hpx::lcos::local::mutex> lock(print);
        std::cout << "Customer id: " << id << " enters the salon\n";
    }
    
    {
        std::lock_guard<hpx::lcos::local::mutex> lock(check);
        if(is_sleeping && customer_id.size() == 0)
        {
            is_sleeping = false;
            std::cout << "Customer id: " << id << " is getting a hair cut\n";
            std::lock_guard<hpx::lcos::local::mutex> lock(cut);
            cut_hair();
            return;
        }
    }

    {
        {
            std::lock_guard<hpx::lcos::local::mutex> lock(check);
            if(customer_id.size() < queue_length)
            {
                std::cout << "Customer id: " << id 
                    << " is waiting in the queue. Current waiting number"
                    << customer_id.size() + 1 << std::endl;
                customer_id.push(id);
            }
            else
            {
                std::cout << "Customer id: " << id 
                    << " is leaving due to unavailability of waiting seats\n";
                return;
            }
        }

        while(customer_id.front() != id) {}

        {
            std::lock_guard<hpx::lcos::local::mutex> lock(cut);
            std::cout << "Customer id: " << id << " is getting a hair cut\n";
            cut_hair();
            customer_id.pop();
        }

        {
            std::lock_guard<hpx::lcos::local::mutex> lock(check);
            if(customer_id.size() == 0)
            {
                is_sleeping = true;
                std::cout << "No one to cut. Barber is now sleeping... zzz" << std::endl;
            }
        }
    }   
}

void Barber::simulate(int n, int wt)
{
    std::vector<std::unique_ptr<hpx::thread>> thread_spawn;
    for(auto i = 0; i < n; ++i)
    {
        std::unique_ptr<hpx::thread> pt = 
                    std::make_unique<hpx::thread>(&Barber::enter_shop, this, i);
        
        thread_spawn.emplace_back(std::move(pt));
        
        
        hpx::this_thread::sleep_for(
            std::chrono::milliseconds((std::rand()%wt) + 1));
    }

    for(auto i = 0; i < n; ++i)
        thread_spawn[i] -> join();
}

int hpx_main(boost::program_options::variables_map& vm)
{
    std::srand(std::time(0));
    
    int n = vm["n-value"].as<int>();
    int wt = vm["wait-time"].as<int>();
    int ct = vm["cut-time"].as<int>();
    int ql = vm["queue-length"].as<int>();

    {
        std::unique_ptr<Barber> barber = std::make_unique<Barber>(ct,ql);

        barber -> simulate(n, wt);
    }

    return hpx::finalize(); // Handles HPX shutdown
}


int main(int argc, char* argv[])
{
    // Configure application-specific options
    boost::program_options::options_description
       desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    desc_commandline.add_options()
        ( "n-value",
          boost::program_options::value<int>()->default_value(10),
          "Number of customers that arrive")
        ;

    desc_commandline.add_options()
        ( "wait-time",
          boost::program_options::value<int>()->default_value(50),
          "Maximum waiting time between two customers")
        ;

    desc_commandline.add_options()
        ( "cut-time",
          boost::program_options::value<int>()->default_value(50),
          "Time taken to cut hair by the barber")
        ;
    
    desc_commandline.add_options()
        ( "queue-length",
          boost::program_options::value<int>()->default_value(3),
          "Maximum waiting customer allowed")
        ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}