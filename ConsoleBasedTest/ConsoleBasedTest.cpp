// ConsoleBasedTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <ClusteringMemoryPool.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

struct NameLog
{
    string name;
    string motherName;
    string fatherName;

    void ChangeName()
    {
    }

    void ChangeMotherName()
    {
    }

    void ChangeFatherName()
    {
    }
};

struct AgeLog
{
    int age;
    int motherAge;
    int fatherAge;

    void ChangeAge()
    {
    }

    void ChangeMotherAge()
    {
    }

    void ChangeFatherAge()
    {
    }
};

struct PersonHandler
{
    vector<rw_clustering_ptr<NameLog>> nameLogs;
    vector<rw_clustering_ptr<AgeLog>> ageLogs;

    void Update()
    {
    }
};

int main()
{
    ClusteringMemoryPool<NameLog> nameLogPool = ClusteringMemoryPool<NameLog>(5);
    ClusteringMemoryPool<AgeLog> ageLogPool = ClusteringMemoryPool<AgeLog>(5);
    vector<PersonHandler> personHandler = std::vector<PersonHandler>();

    for(int i = 0; i < 1000; ++i)
    {
        auto id = to_string(i);
        auto person = PersonHandler();
        person.nameLogs.push_back(nameLogPool.AddToPool(NameLog{ "rabin"+ id,  "rabin mom"+ id, "rabin dad"+ id }));
        person.ageLogs.push_back(ageLogPool.AddToPool(AgeLog{ 25 + i, 42 + i, 45 + i}));
        personHandler.push_back(std::move(person));
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(personHandler.begin(), personHandler.end(), g);

    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
