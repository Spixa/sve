#include "app.hpp"
#include <exception>

// std
#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main()
{
    sve::App app{};

    try {
        app.run();
    }
    catch (std::exception const& e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}