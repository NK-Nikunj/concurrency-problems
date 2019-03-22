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
    void customer_leave();

    Barber(int ct, int ql) : cut_time(ct), queue_length(ql) {}

private:
    std::queue<int> customer_id;
    bool is_sleeping = false;
    int cut_time;
    int queue_length;

    hpx::lcos::local::mutex check, cut, push;
};

void Barber::cut_hair()
{
    std::cout << "Cutting hair" << std::endl;
    hpx::this_thread::sleep_for(std::chrono::milliseconds(cut_time));
}

void Barber::customer_leave()
{
    std::cout << "No waiting remaining. Leaving" << std::endl;
}

void Barber::enter_shop(int id)
{
    {
        std::lock_guard<hpx::lcos::local::mutex> lock(check);
        if(is_sleeping && customer_id.size() == 0)
        {
            is_sleeping == false;
            std::lock_guard<hpx::lcos::local::mutex> lock(cut);
            cut_hair();
        }
    }

    {
        {
            std::lock_guard<hpx::lcos::local::mutex> lock(push);
            if(customer_id.size() < queue_length)
                customer_id.push(id);
            else
                customer_leave();
        }

        while(customer_id.front() != id) {}

        std::lock_guard<hpx::lcos::local::mutex> lock(cut);
        cut_hair();
        customer_id.pop();

        {
            std::lock_guard<hpx::lcos::local::mutex> lock(check);
            if(customer_id.size() == 0)
                is_sleeping = true;
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
        
        std::srand(std::time(0));
        hpx::this_thread::sleep_for(
            std::chrono::milliseconds((std::rand()%wt) + 1));
    }

    for(auto i = 0; i < n; ++i)
        thread_spawn[i] -> join();
}

int hpx_main(boost::program_options::variables_map& vm)
{
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
          "number of customers that arrive")
        ;

    desc_commandline.add_options()
        ( "wait-time",
          boost::program_options::value<int>()->default_value(50),
          "maximum waiting time before two customers")
        ;

    desc_commandline.add_options()
        ( "cut-time",
          boost::program_options::value<int>()->default_value(500),
          "Time taken to cut hair by the barber")
        ;
    
    desc_commandline.add_options()
        ( "queue-length",
          boost::program_options::value<int>()->default_value(5),
          "Maximum waiting customer allowed")
        ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}