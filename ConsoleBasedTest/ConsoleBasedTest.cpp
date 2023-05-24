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
#include <chrono>

using namespace std;

struct NameLog
{
    string name;
    string motherName;
    string fatherName;

    void ChangeName(string _name, string _motherName, string _fatherName)
    {
        name = _name;
        motherName = _motherName;
        fatherName = _fatherName;
    }
};

struct AgeLog
{
    int age;
    int motherAge;
    int fatherAge;

    void ChangeAge(int _age, int _motherAge, int _fatherAge)
    {
        age = _age;
        motherAge = _motherAge;
        fatherAge = _fatherAge;
    }
};

struct PersonLog
{
    rw_clustering_ptr<NameLog> nameLog;
    rw_clustering_ptr<AgeLog> ageLog;
};

struct PersonHandler
{
    vector<PersonLog> personLogs;

    void Update()
    {
        std::random_device rd;
        std::mt19937 g(rd());

        for (auto& pL : personLogs)
        {
            auto id = g();
            pL.nameLog.write(&NameLog::ChangeName, "rabin" + to_string(id), "rabin mom" + to_string(id), "rabin dad" + to_string(id));
            pL.ageLog.write(&AgeLog::ChangeAge, 25 + id, 42 + id, 45 + id);
        }
    }
};

int main()
{
    ClusteringMemoryPool<NameLog> nameLogPool = ClusteringMemoryPool<NameLog>(5);
    ClusteringMemoryPool<AgeLog> ageLogPool = ClusteringMemoryPool<AgeLog>(5);
    vector<PersonHandler> personHandlers = std::vector<PersonHandler>();

    for(int a = 0; a < 10; ++a)
    {
        auto personHandlr = PersonHandler();

        for (int i = 0; i < 1000; ++i)
        {
            auto id = to_string(i);
            auto personLog = PersonLog{ nameLogPool.AddToPool(NameLog{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id })
                                , ageLogPool.AddToPool(AgeLog{ 25 + i, 42 + i, 45 + i }) };
            personHandlr.personLogs.push_back(std::move(personLog));
        }

        personHandlers.push_back(std::move(personHandlr));
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(personHandlers.begin(), personHandlers.end(), g);
    for(int i = 0; i < personHandlers.size(); ++i)
    {
        std::shuffle(personHandlers[i].personLogs.begin(), personHandlers[i].personLogs.end(), g);
    }

    auto start = chrono::high_resolution_clock::now();

    for (auto& pH : personHandlers) 
    {
        pH.Update();
    }

    nameLogPool.ExecuteClusteredTasks();
    ageLogPool.ExecuteClusteredTasks();

    auto end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;

    cout << "Time taken by program is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;
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
