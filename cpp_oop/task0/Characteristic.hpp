#ifndef CHARACTERISTIC_HPP
#define CHARACTERISTIC_HPP

#include <string>
#include <cmath>
#include <stdexcept>
#include <functional>
 
namespace ch {
 
    class Characteristic {
        protected:
            std::string name;
            int value;
 
            void checkValue(int value) {
 
                if (value < 1 || value > 20) {
                    throw std::out_of_range("The characteristic value must be from 1 to 20");
                }
            }
 
        public:
            explicit Characteristic();
            explicit Characteristic(std::string name, int value);
            explicit Characteristic(std::string name);
 
            std::string getName() const {return name; };
            int getValue() const {return value; };
 
            Characteristic &setName (std::string newName){
                name = newName;
                return *this;
            };
 
            Characteristic &setValue(int newValue){
                checkValue(newValue);
                value = newValue;
                return *this;
            };
 
 
            // выполнение проверки с заданной сложностью;
            bool abilityCheck(int hardness);
            // выполнение проверки с преимуществом с заданной сложностью;
            bool advantageCheck(int hardness);
            // выполнение проверки с помехой с заданной сложностью;
            bool disadvantageCheck(int hardness);
            // (+=) увеличение характеристики на заданное число.
            bool valueChange(int delta);
 
            Characteristic &operator+=(int delta);
 
            friend std::ostream & operator << (std::ostream &, const Characteristic &);
            friend std::istream & operator >> (std::istream &, Characteristic &);
 
    };
 
    std::ostream & operator << (std::ostream &, const Characteristic &);
    std::istream & operator >> (std::istream &, const Characteristic &);
    int rollD20();
 
}
 
#endif // CHARACTERISTIC_HPP