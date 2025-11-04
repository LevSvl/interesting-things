#include "Characteristic.hpp"
#include <string>
#include <random>
#include <iostream> 
#include <functional>  
#include <limits>      

namespace ch {
    Characteristic::Characteristic(): name(""), value(1) {};
 
    Characteristic::Characteristic(std::string name, int value){
        Characteristic::checkValue(value);
        this->name = name;
        this->value = value;
    };
 
    Characteristic::Characteristic(std::string name): value(1) {
        this->name = name;
    };
 
 
    int rollD20() {
        static std::random_device rd;  
        static std::mt19937 gen(rd()); 
        static std::uniform_int_distribution<int> dis(1, 20); 
 
        return dis(gen);
    };
 
    // выполнение проверки с заданной сложностью;
    // 1 - пройдена, 0 - не пройдена
    bool Characteristic::abilityCheck(int hardness){
        int check = rollD20();
 
        if (check == 1) return 0;
        if (check == 20) return 1;
 
        return this->value + check > hardness;
    };
 
    // выполнение проверки с преимуществом с заданной сложностью;
    bool Characteristic::advantageCheck(int hardness){
        bool check1 = abilityCheck(hardness);
        bool check2 = abilityCheck(hardness);
 
        return check1 + check2;
    };
 
    // выполнение проверки с помехой с заданной сложностью;
    bool Characteristic::disadvantageCheck(int hardness){
        bool check1 = abilityCheck(hardness);
        bool check2 = abilityCheck(hardness);
 
        return check1 * check2;
    };
 
    // (+=) увеличение характеристики на заданное число.
    Characteristic & Characteristic::operator += (int delta){
        this->value += delta;
        return *this;
    };
 
    std::ostream & operator << (std::ostream &s, const Characteristic &ch){
        s << "Name: " << ch.name << "\n";
        s << "Value: " << ch.value << "\n";
        s << std::endl;
        return s;
    };
 
   std::istream & operator >> (std::istream &s, Characteristic &ch) {
        std::string name;
        int value;
 
        while (true) {
 
            std::cout << "\nEnter name: ";
            while (!(s >> name)) {
                std::cout << "\nInvalid input for name. Please enter a valid string: ";
                s.clear();
                s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
 
 
            std::cout << "Enter value (1 - 20): ";
            while (!(s >> value)) {
                std::cout << "\nInvalid input for value. Please enter an integer: ";
                s.clear();
                s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
 
 
            try {
                ch.setName(name).setValue(value);
                std::cout << "Success! Characteristic '" << name << "' with value " << value << " has been set.\n";
                break; 
            } catch (const std::out_of_range& e) {
                std::cout << "\nError: " << e.what() << "\nPlease enter both name and value again.\n";
            }
 
 
            s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
 
        return s;
    }
 
}
