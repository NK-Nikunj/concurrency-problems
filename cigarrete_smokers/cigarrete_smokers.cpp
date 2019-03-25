///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Nikunj Gupta
///////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx_main.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/util.hpp>

#include <cstdint>
#include <iostream>
#include <utility>
#include <ctime>
#include <string>
#include <memory>

///////////////////////////////////////////////////////////////////////////////
#define TOBACO 1
#define PAPER 2
#define MATCH 3

hpx::lcos::local::mutex smoke, check;

bool should_stop = false;

struct smoker
{
    bool has_tobaco;
    bool has_paper;
    bool has_match;

    smoker(int t, int p, int m):
        has_tobaco(t),
        has_paper(p),
        has_match(m) {}
};

///////////////////////////////////////////////////////////////////////////////
std::pair<int, int> generate_two_products()
{
    std::srand(std::time(0));

    int num = std::rand() % 3 + 1;

    switch (num)
    {
        case TOBACO:
            return std::make_pair(PAPER, MATCH);
        
        case PAPER:
            return std::make_pair(TOBACO, MATCH);

        case MATCH:
            return std::make_pair(PAPER, TOBACO);
    }
}

///////////////////////////////////////////////////////////////////////////////
void smoke_cigarrete(std::shared_ptr<smoker> s)
{
    std::string str = "";
    if(s -> has_tobaco)    str += "smoker with tobaco";
    if(s -> has_paper)    str += "smoker with paper";
    if(s -> has_match)    str += "smoker with match";
    std::cout << str << " is now smoking.." << std::endl;
    hpx::this_thread::sleep_for(std::chrono::milliseconds(100));
}

///////////////////////////////////////////////////////////////////////////////
void create_cigarrete_tobacco(
    std::shared_ptr<smoker> s,
    std::shared_ptr<int> prod1,
    std::shared_ptr<int> prod2,
    std::shared_ptr<bool> change
)
{
    while(!should_stop)
    {
        {
            {
                std::lock_guard<hpx::lcos::local::mutex> lock(check);
                if((*prod1 == MATCH || *prod1 == PAPER) 
                    && 
                    ((*prod2 == MATCH || *prod2 == PAPER)))
                {
                    std::lock_guard<hpx::lcos::local::mutex> lock(smoke);
                    smoke_cigarrete(s);
                }
            }
            *change = true;
            hpx::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void create_cigarrete_paper(
    std::shared_ptr<smoker> s,
    std::shared_ptr<int> prod1,
    std::shared_ptr<int> prod2,
    std::shared_ptr<bool> change
)
{
    while(!should_stop)
    {
        {
            {
                std::lock_guard<hpx::lcos::local::mutex> lock(check);
                if((*prod1 == MATCH || *prod1 == TOBACO) 
                    && 
                    ((*prod2 == MATCH || *prod2 == TOBACO)))
                {
                    std::lock_guard<hpx::lcos::local::mutex> lock(smoke);
                    smoke_cigarrete(s);
                }
            }    
            *change = true;
            hpx::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        // change = true;
        // hpx::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void create_cigarrete_match(
    std::shared_ptr<smoker> s,
    std::shared_ptr<int> prod1,
    std::shared_ptr<int> prod2,
    std::shared_ptr<bool> change
)
{
    while(!should_stop)
    {
        {
            {
                std::lock_guard<hpx::lcos::local::mutex> lock(check);
                if((*prod1 == PAPER || *prod1 == TOBACO) 
                    && 
                    ((*prod2 == PAPER || *prod2 == TOBACO)))
                {
                    std::lock_guard<hpx::lcos::local::mutex> lock(smoke);
                    smoke_cigarrete(s);
                }
            }
            *change = true;
            hpx::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        // change = true;
        // hpx::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

///////////////////////////////////////////////////////////////////////////////
void stop_threads()
{
    std::cin.get();
    should_stop = true; 
}

///////////////////////////////////////////////////////////////////////////////
void simulate(std::shared_ptr<smoker> s1,
              std::shared_ptr<smoker> s2,
              std::shared_ptr<smoker> s3)
{
    std::pair<int, int> p = generate_two_products();

    std::shared_ptr<int> prod1 = std::make_shared<int>();
    std::shared_ptr<int> prod2 = std::make_shared<int>();
    std::shared_ptr<bool> change = std::make_shared<bool>();

    *prod1 = std::get<0>(p);
    *prod2 = std::get<1>(p);

    hpx::thread t1(&create_cigarrete_tobacco, s1, prod1, prod2, change);
    hpx::thread t2(&create_cigarrete_paper, s2, prod1, prod2, change);
    hpx::thread t3(&create_cigarrete_match, s3, prod1, prod2, change);

    hpx::thread stop(&stop_threads);

    while(!should_stop)
    {
        if(*change)
        {
            std::lock_guard<hpx::lcos::local::mutex> lock(check);
            p = generate_two_products();

            *prod1 = std::get<0>(p);
            *prod2 = std::get<1>(p);
            // hpx::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    t1.join();
    t2.join();
    t3.join();
    stop.join();
}

int main(int argc, char* argv[])
{
    std::shared_ptr<smoker> smoker1 = std::make_shared<smoker>(1, 0, 0);
    std::shared_ptr<smoker> smoker2 = std::make_shared<smoker>(0, 1, 0);
    std::shared_ptr<smoker> smoker3 = std::make_shared<smoker>(0, 0, 1);

    simulate(smoker1, smoker2, smoker3);

    return 0;
}