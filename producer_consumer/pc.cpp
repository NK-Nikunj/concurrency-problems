///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Nikunj Gupta
///////////////////////////////////////////////////////////////////////////////

#include <cstdint>
#include <iostream>
#include <queue>
#include <atomic>

#include <thread>

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
std::atomic<int> full {0};
std::atomic<int> empty {0};
std::atomic<int> mutex {1};

std::queue<Item> buffer;

// To end loops
bool should_end = false;

namespace semaphore
{
    class mutex 
    { 
        std::atomic<bool> lock_stream; 
    public:
        mutex() :lock_stream(false){} 
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
    std::vector<std::unique_ptr<std::thread>> thread_spawn;
    for(auto i = 0; i < p; ++i)
    {
        std::unique_ptr<std::thread> pt = 
                    std::make_unique<std::thread>(&producer, i);
        
        thread_spawn.emplace_back(std::move(pt));
    }

    for(auto i = 0; i < c; ++i)
    {
        std::unique_ptr<std::thread> pt = 
                    std::make_unique<std::thread>(&consumer, i);
        
        thread_spawn.emplace_back(std::move(pt));
    }

    std::unique_ptr<std::thread> pt = 
                std::make_unique<std::thread>(&stop);
    
    thread_spawn.emplace_back(std::move(pt));

    for(auto i = 0; i < p + c + 1; ++i)
        thread_spawn[i] -> join();
}

int main()
{
    std::srand(std::time(0));

    int n = 10;
    int p = 5;
    int c = 5;
    empty = n;

    {
        simulate(p, c);
    }

    return 0;
}