#include <iostream>
#include <random>

#include "CharacteristicTable.hpp"


int ch_table_create_by_name()
{
    
    std::vector<std::string> names{"foo", "bar", "lol"};
    ch::CharacteristicTable ch_table(names);

    for (auto ch_name = names.begin(); ch_name != names.end(); ch_name++)
        std::cout << ch_table[*ch_name] << std::endl;

    if (ch_table["foo"].getValue() != 1)
        return 1;

    if (ch_table["bar"].getValue() != 1)
        return 1;

    if (ch_table["lol"].getValue() != 1)
        return 1;

    return 0;
}

int main()
{
    int retval = 0;

    std::cout << "Tests start..." << std::endl << std::endl;

    retval = ch_table_create_by_name();


    std::cout << std::endl << std::endl << "Tests done. Result ";
    std::cout << ((retval == 0) ? "OK" : "Error") << std::endl;

    return retval;
}
