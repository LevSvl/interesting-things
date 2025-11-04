#ifndef CHARACTERISTICTABLE_HPP
#define CHARACTERISTICTABLE_HPP

#include <string>
#include <stdexcept>
#include <vector>
#include <unordered_set>

#include "Characteristic.hpp"
 
namespace ch {
 
    class CharacteristicTable {
        private:
            std::vector<Characteristic> CharcVector;
            //список имён характеристик для быстрого поиска
            std::unordered_set<std::string> CharcNameSet; 
 
        public:
 
        enum checkType {
            NORMAL,
            ADVANTAGE,
            DISADVANTAGE
        };
 
        //конструктор по умолчанию (явный или неявный);
        CharacteristicTable();
        //методы ввода и вывода состояния класса в входной/выходной поток;
        //создание экземпляра класса с инициализацией массивом названий характеристик;
        CharacteristicTable(std::vector<std::string> &);
        //создание экземпляра класса с инициализацией одной характеристикой (названием и значением);
        CharacteristicTable(Characteristic &);
        //([]) получение характеристики по её названию (возврат по ссылке);
        Characteristic &operator [] (std::string);
        //( () ) прохождение проверки1 по заданной характеристике с заданной сложностью в одном из режимов: обычная, с преимуществом, с помехой;
        bool &operator () (Characteristic &, int, checkType);
        //(+=) добавление новой характеристики;
        CharacteristicTable &operator += (const Characteristic &);
        //“=” для копирования экземпляра сложного класса;
        CharacteristicTable &operator = (const CharacteristicTable &);
        //упорядочивание характеристик по названию;
        void sort();
        //поиск характеристики с максимальным значением среди заданных (передаётся массив строк)
        const Characteristic &findMax(std::vector<std::string> &);
    };
 
    using chTbl = CharacteristicTable;
 
}
 
 
#endif // CHARACTERISTICTABLE_HPP
