#include <hpx/hpx_main.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/util.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#define N 5

std::vector<bool> chopsticks(N, 1);
hpx::lcos::local::mutex mu[5];
hpx::lcos::local::mutex print_mut;


void print_wish(int i)
{
    std::lock_guard<hpx::lcos::local::mutex> print(print_mut);
    char const* fmt = "Philosopher {1} wishes to eat\n";
    hpx::util::format_to(std::cout, fmt, i);
}

void print_eat(int i)
{
    std::lock_guard<hpx::lcos::local::mutex> print(print_mut);
    char const* fmt = "Philosopher {1} has eaten\n";
    hpx::util::format_to(std::cout, fmt, i);
}

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
    // std::cout << i << std::endl;
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

void dt(int i)
{
    int left = (i + N - 1) % N;
    int right = (i + 1) % N;

    int t = 100;

    while(t--)
    {
        bool wishes_to_eat = generate_random_number();

        if(wishes_to_eat)
        {
            // char const* fmt1 = "Philosopher {1} wishes to eat, it will now try";
            // hpx::util::format_to(std::cout, fmt1, index);
            
            print_wish(i);

            bool is_left_clear, is_right_clear;
            do {
                is_left_clear = check_left(left);
                is_right_clear = check_right(right);

                if(is_left_clear && is_right_clear)
                    eat();

            } while(is_left_clear == false || is_right_clear == false);

            // char const* fmt2 = "Philosopher {1} has eaten well, it will now think";
            // hpx::util::format_to(std::cout, fmt2, index);

            print_eat(i);

            chopsticks[left] = true;
            chopsticks[right] = true;
        }
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::unique_ptr<hpx::thread>> threads;
    for(auto i = 0; i < N; ++i)
    {
        std::unique_ptr<hpx::thread> pt = std::make_unique<hpx::thread>(dt, i);
        threads.emplace_back(std::move(pt));
    }

    for(auto i = 0; i < N; ++i)
    {
        threads[i] -> join();
    }

    return 0;
}