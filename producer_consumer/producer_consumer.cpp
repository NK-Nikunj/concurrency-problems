///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Nikunj Gupta
///////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx_init.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/util.hpp>

#include <cstdint>
#include <iostream>
#include <queue>
#include <atomic>


class Item
{
public:
    Item(std::string s)
    {
        content = s;
    }

    std::string get_content()
    {
        return content;
    }

private:
    std::string content;
};

// To define a semaphore
std::atomic<int> full = 0;
std::atomic<int> empty = 0;
std::atomic<int> mutex = 1;

std::queue<Item> buffer;

// To end loops
bool should_end = false;

namespace semaphore
{
    class mutex 
    { 
        std::atomic<bool> lock_stream = false; 
    public:
        mutex(){} 
        void lock() 
        { 
            do{
                while(lock_stream) continue;
            } while(lock_stream.exchange(true));
        } 

        void unlock() 
        { 
            lock_stream = false;
        } 
    };

    mutex mu;

    void wait(std::atomic<int>& S)
    {
        while (1) {
            while (S <= 0) {}
            mu.lock();
            if (S <= 0) {
                mu.unlock();
                continue;
            }
            S -= 1;
            mu.unlock();
            break;
        }
    }

    void signal(std::atomic<int>& S)
    {
        mu.lock();
        S += 1;
        mu.unlock();
    }
}

std::string get_item()
{
    std::vector<std::string> s {"Random string 1",
                                "Random string 2",
                                "Random string 3",
                                "Random string 4",
                                "Random string 5",
                                "Random string 6",
                                "Random string 7",
                                "Random string 8",
                                "Random string 9",
                                "Random string 10"};

    return s[std::rand() % 10];
}

void producer(int i)
{
    while(!should_end)
    {
        Item item(get_item());

        semaphore::wait(empty);
        semaphore::wait(mutex);

        std::cout << empty << " " << mutex << " " << full << std::endl;

        std::cout << "Producer " << i << " produced item with string: "
                  << item.get_content() << std::endl;

        buffer.push(item);

        semaphore::signal(mutex);
        semaphore::signal(full);
    }
}

void consumer(int i)
{
    while(!should_end)
    {
        semaphore::wait(full);
        semaphore::wait(mutex);

        std::cout << empty << " " << mutex << " " << full << std::endl;

        Item item = buffer.front();
        std::cout << "Consumer " << i << " consumed item with string: "
                  << item.get_content() << std::endl;

        semaphore::signal(mutex);
        semaphore::signal(empty);
    }
}

void stop()
{
    std::cin.get();
    should_end = true;
}

void simulate(int p, int c)
{
    std::vector<std::unique_ptr<hpx::thread>> thread_spawn;
    for(auto i = 0; i < p; ++i)
    {
        std::unique_ptr<hpx::thread> pt = 
                    std::make_unique<hpx::thread>(&producer, i);
        
        thread_spawn.emplace_back(std::move(pt));
    }

    for(auto i = 0; i < c; ++i)
    {
        std::unique_ptr<hpx::thread> pt = 
                    std::make_unique<hpx::thread>(&consumer, i);
        
        thread_spawn.emplace_back(std::move(pt));
    }

    std::unique_ptr<hpx::thread> pt = 
                std::make_unique<hpx::thread>(&stop);
    
    thread_spawn.emplace_back(std::move(pt));

    for(auto i = 0; i < p + c + 1; ++i)
        thread_spawn[i] -> join();
}

int hpx_main(boost::program_options::variables_map& vm)
{
    std::srand(std::time(0));

    int n = vm["n-value"].as<int>();
    int p = vm["prod-value"].as<int>();
    int c = vm["cons-value"].as<int>();
    empty = n;

    {
        simulate(p, c);
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
          "n value for the Buffer length")
        ;

    desc_commandline.add_options()
    ( "prod-value",
        boost::program_options::value<int>()->default_value(5),
        "Number of producers")
    ;

    desc_commandline.add_options()
        ( "cons-value",
          boost::program_options::value<int>()->default_value(5),
          "Number of consumers")
        ;



    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}