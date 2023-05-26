#include <ClusteringMemoryPool.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

struct NameLog
{
    string name;
    string motherName;
    string fatherName;

    void ChangeName(string _name, string _motherName, string _fatherName)
    {
        name = std::move(_name);
        motherName = std::move(_motherName);
        fatherName = std::move(_fatherName);
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

    void Shuffle(std::mt19937 g)
    {
        std::shuffle(personLogs.begin(), personLogs.end(), g);
    }

    void Update()
    {
        std::random_device rd;
        std::mt19937 g(rd());

        for (auto& pL : personLogs)
        {
            auto id = g();
            auto idStr = to_string(id);
            pL.nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
            pL.nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
            pL.nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
            pL.nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
            pL.nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
            pL.nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
        }
    }
};

struct PersonLogNormal
{
    shared_ptr<NameLog> nameLog;
    shared_ptr<AgeLog> ageLog;
};

struct PersonHandlerNormal
{
    vector<PersonLogNormal> personLogs;

    void Update()
    {
        std::random_device rd;
        std::mt19937 g(rd());

        for (auto& pL : personLogs)
        {
            auto id = g();
            auto idStr = to_string(id);
            pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
            pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
            pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
            pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
            pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
            pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
            pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
        }
    }
};

void TestClustering()
{
    ClusteringMemoryPool<NameLog> nameLogPool = ClusteringMemoryPool<NameLog>(100);
    ClusteringMemoryPool<AgeLog> ageLogPool = ClusteringMemoryPool<AgeLog>(100);
    ClusteringMemoryPool<PersonHandler> personHandlerPool = ClusteringMemoryPool<PersonHandler>(100);
    vector<rw_clustering_ptr<PersonHandler>> personHandlers = vector<rw_clustering_ptr<PersonHandler>>();

    for (int a = 0; a < 1000; ++a)
    {
        auto personHandlr = PersonHandler();

        for (int i = 0; i < 10000; ++i)
        {
            auto id = to_string(i);
            auto personLog = PersonLog{ nameLogPool.AddToPool(NameLog{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id })
                                , ageLogPool.AddToPool(AgeLog{ 0 + i, 50 + i, 100 + i }) };
            personHandlr.personLogs.push_back(std::move(personLog));
        }

        personHandlers.push_back(std::move(personHandlerPool.AddToPool(std::move(personHandlr))));
    }

    std::random_device rd;
    std::mt19937 g(rd());

    auto start = chrono::high_resolution_clock::now();

    std::shuffle(personHandlers.begin(), personHandlers.end(), g);

    for (auto& pH : personHandlers)
    {
        pH.write(&PersonHandler::Shuffle, std::move(g));
        pH.write(&PersonHandler::Update);
    }

    personHandlerPool.ExecuteClusteredTasks();
    nameLogPool.ExecuteClusteredTasks();
    ageLogPool.ExecuteClusteredTasks();

    auto end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;

    cout << "Time taken by clustering memory pool is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;
}

void TestNormal()
{
    vector<shared_ptr<NameLog>> nameLogPool = vector<shared_ptr<NameLog>>();
    vector<shared_ptr<AgeLog>> ageLogPool = vector<shared_ptr<AgeLog>>();
    vector<shared_ptr<PersonHandlerNormal>> personHandlers = std::vector<shared_ptr<PersonHandlerNormal>>();

    for (int a = 0; a < 1000; ++a)
    {
        auto personHandlr = std::make_shared<PersonHandlerNormal>(PersonHandlerNormal());

        for (int i = 0; i < 10000; ++i)
        {
            auto id = to_string(i);
            nameLogPool.push_back(std::make_shared<NameLog>(NameLog{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id }));
            ageLogPool.push_back(std::make_shared<AgeLog>(AgeLog{ 0 + i, 50 + i, 100 + i }));
            auto personLog = PersonLogNormal{ nameLogPool.back(), ageLogPool.back()};
            personHandlr->personLogs.push_back(std::move(personLog));
        }

        personHandlers.push_back(std::move(personHandlr));
    }

    std::random_device rd;
    std::mt19937 g(rd());

    auto start = chrono::high_resolution_clock::now();

    std::shuffle(personHandlers.begin(), personHandlers.end(), g);

    for (auto& pH : personHandlers)
    {
        std::shuffle(pH->personLogs.begin(), pH->personLogs.end(), g);
        pH->Update();
    }

    auto end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;

    cout << "Time taken by vector of pointers is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;
}

void TestClusteringPoolWriteValidity()
{
    ClusteringMemoryPool<NameLog> nameLogPool = ClusteringMemoryPool<NameLog>(8);

    auto rw_ptr = nameLogPool.AddToPool(NameLog{ "rabin",  "rabin mom", "rabin dad" });

    cout << "Before write: " << rw_ptr.get()->name << endl;
    rw_ptr.write(&NameLog::ChangeName, std::move("valid write"), std::move("valid write mom"), std::move("valid write dad"));
    nameLogPool.ExecuteClusteredTasks();
    cout << "After write: " << rw_ptr.get()->name << endl;
}

int main()
{
    TestNormal();
    TestClustering();
    TestClusteringPoolWriteValidity();
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
