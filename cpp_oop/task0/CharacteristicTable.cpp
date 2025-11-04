#include <string>
#include <random>
#include <iostream> 
#include <functional>  
#include <limits>

#include "CharacteristicTable.hpp"

namespace ch {
    CharacteristicTable::CharacteristicTable()
    {
    };
 
    CharacteristicTable::CharacteristicTable(std::vector<std::string> & names)
    {
        auto current_name = names.begin();

        while (current_name != names.end()) {
            this->CharcVector.push_back(Characteristic(*current_name));
            this->CharcNameSet.insert(*current_name);

            current_name++;
        }
    };
 
    CharacteristicTable::CharacteristicTable(Characteristic &)
    {
    }; 



    Characteristic & CharacteristicTable::operator[](std::string target_charc_name)
    {
        /*
         * Если в множестве CharcNameSet нет нужного имени - вернуть итератор,
         * указывающий на следующий за последним элементом CharcVector. 
         * По крайней мере с контейнерами делают так.
         */
        if (this->CharcNameSet.find(target_charc_name) == this->CharcNameSet.end())
            return *this->CharcVector.end();

        for (auto charc_i = CharcVector.begin();
                charc_i != this->CharcVector.end(); charc_i++)
        {
            if ((*charc_i).getName() == target_charc_name)
                return *charc_i;
        }

        return *this->CharcVector.end();
    };
}
