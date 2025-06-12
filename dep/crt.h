#pragma once


namespace crt
{
    __forceinline std::chrono::high_resolution_clock::time_point now()
    {
        return std::chrono::high_resolution_clock::now();
    }

    __forceinline int ms_since( std::chrono::high_resolution_clock::time_point earlier ) 
    {
        return static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(now() - earlier).count()
            );
    }
}