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

#define TOBACO 1
#define PAPER 2
#define MATCH 3

hpx::lcos::local::mutex smoke, tobaco, paper, match, check;

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

// smoker1 has infinite tobaco
smoker smoker1 {1, 0, 0};

// smoker2 has infinite papers
smoker smoker2 {0, 1, 0};

// smoker3 has infinite matches
smoker smoker3 {0, 0, 1};

int prod1;
int prod2;
bool change = false;

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

void smoke_cigarrete(smoker s)
{
    std::string str = "";
    if(s.has_tobaco)    str += "smoker with tobaco";
    if(s.has_paper)    str += "smoker with paper";
    if(s.has_match)    str += "smoker with match";
    std::cout << str << " is now smoking.." << std::endl;
    hpx::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void create_cigarrete_tobacco()
{
    while(1)
    {
        {
            {
                std::lock_guard<hpx::lcos::local::mutex> lock(check);
                if((prod1 == MATCH || prod1 == PAPER) 
                    && 
                    ((prod2 == MATCH || prod2 == PAPER)))
                {
                    std::lock_guard<hpx::lcos::local::mutex> lock(smoke);
                    smoke_cigarrete(smoker1);
                }
            }
            change = true;
            hpx::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void create_cigarrete_paper()
{
    while(1)
    {
        {
            {
                std::lock_guard<hpx::lcos::local::mutex> lock(check);
                if((prod1 == MATCH || prod1 == TOBACO) 
                    && 
                    ((prod2 == MATCH || prod2 == TOBACO)))
                {
                    std::lock_guard<hpx::lcos::local::mutex> lock(smoke);
                    smoke_cigarrete(smoker2);
                }
            }    
            change = true;
            hpx::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        // change = true;
        // hpx::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void create_cigarrete_match()
{
    while(1)
    {
        {
            {
                std::lock_guard<hpx::lcos::local::mutex> lock(check);
                if((prod1 == PAPER || prod1 == TOBACO) 
                    && 
                    ((prod2 == PAPER || prod2 == TOBACO)))
                {
                    std::lock_guard<hpx::lcos::local::mutex> lock(smoke);
                    smoke_cigarrete(smoker3);
                }
            }
            change = true;
            hpx::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        // change = true;
        // hpx::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void simulate()
{
    std::pair<int, int> p = generate_two_products();

    prod1 = std::get<0>(p);
    prod2 = std::get<1>(p);

    hpx::thread t1(&create_cigarrete_tobacco);
    hpx::thread t2(&create_cigarrete_paper);
    hpx::thread t3(&create_cigarrete_match);

    while(1)
    {
        if(change)
        {
            std::lock_guard<hpx::lcos::local::mutex> lock(check);
            p = generate_two_products();

            prod1 = std::get<0>(p);
            prod2 = std::get<1>(p);
            // hpx::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    t1.join();
    t2.join();
    t3.join();
}

int main(int argc, char* argv[])
{
    simulate();

    return 0;
}