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

    void ChangeNameLvalue(string& _name, string& _motherName, string& _fatherName)
    {
        name = std::move(_name);
        motherName = std::move(_motherName);
        fatherName = std::move(_fatherName);
    }

    void ChangeNameRvalue(string&& _name, string&& _motherName, string&& _fatherName)
    {
        name = std::move(_name);
        motherName = std::move(_motherName);
        fatherName = std::move(_fatherName);
    }

    alignas(alignof(std::string)) std::byte buffer[alignof(std::string) * 4];
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

    alignas(alignof(int)) std::byte buffer[alignof(int) * 5];
};

struct PersonLog
{
    rw_clustering_ptr<NameLog> nameLog;
    rw_clustering_ptr<AgeLog> ageLog;

    void Update(unsigned int& id, string& idStr)
    {
        //string name = "rabin" + idStr;
        //string momname = "rabin mom" + idStr;
        //string dadname = "rabin dad" + idStr;

        //int age = 0 + id;
        //int momage = 50 + id;
        //int dadage = 100 + id;

        for(int i = 0; i< 1; ++i)
        {
            auto addstr = to_string(i);
            string name = "rabin" + idStr + addstr;
            string momname = "rabin mom" + idStr + addstr;
            string dadname = "rabin dad" + idStr + addstr;

            nameLog.stackingWrite(&NameLog::ChangeNameLvalue, name, momname, dadname);
            //nameLog.oneTimeWrite(&NameLog::ChangeNameRvalue, name, momname, dadname);
            //nameLog.oneTimeWrite(&NameLog::ChangeNameRvalue, name, momname, dadname);
            //nameLog.oneTimeWrite(&NameLog::ChangeNameRvalue, name, momname, dadname);
            //nameLog.oneTimeWrite(&NameLog::ChangeNameRvalue, name, momname, dadname);
            //nameLog.oneTimeWrite(&NameLog::ChangeNameRvalue, name, momname, dadname);

            //nameLog.write(&NameLog::ChangeNameRValue, name, momname, dadname);
            //nameLog.write(&NameLog::ChangeNameRvalue, name, momname, dadname);

            //int age = 0 + id + i;
            //int momage = 50 + id + i;
            //int dadage = 100 + id + i;

            //ageLog.write(&AgeLog::ChangeAge, age, momage, dadage);
        }
        //nameLog.write(&NameLog::ChangeName, name, momname, dadname);
        //nameLog.write(&NameLog::ChangeName, name, momname, dadname);
        //nameLog.write(&NameLog::ChangeName, name, momname, dadname);
        //nameLog.write(&NameLog::ChangeName, name, momname, dadname);
        //nameLog.write(&NameLog::ChangeName, name, momname, dadname);
        //nameLog.write(&NameLog::ChangeName, name, momname, dadname);


        //ageLog.write(&AgeLog::ChangeAge, age, momage, dadage);
        //ageLog.write(&AgeLog::ChangeAge, age, momage, dadage);
        //ageLog.write(&AgeLog::ChangeAge, age, momage, dadage);
        //ageLog.write(&AgeLog::ChangeAge, age, momage, dadage);
        //ageLog.write(&AgeLog::ChangeAge, age, momage, dadage);
        //ageLog.write(&AgeLog::ChangeAge, age, momage, dadage);
        
        //nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
        //nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
        //nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
        //nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
        //nameLog.write(&NameLog::ChangeName, "rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //ageLog.write(&AgeLog::ChangeAge, 0 + id, 50 + id, 100 + id);
    }
};

struct PersonHandler
{
    vector<PersonLog> personLogs;

    void Shuffle(std::mt19937& g)
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
            pL.Update(id, idStr);
        }
    }
};

struct PersonLogNormal
{
    shared_ptr<NameLog> nameLog;
    shared_ptr<AgeLog> ageLog;

    void Update(unsigned int& id, string& idStr)
    {
        //auto name = "rabin" + idStr;
        //auto momname = "rabin mom" + idStr;
        //auto dadname = "rabin dad" + idStr;

        //int age = 0 + id;
        //int momage = 50 + id;
        //int dadage = 100 + id;

        for (int i = 0; i < 1; ++i)
        {
            auto addstr = to_string(i);
            string name = "rabin" + idStr + addstr;
            string momname = "rabin mom" + idStr + addstr;
            string dadname = "rabin dad" + idStr + addstr;

            nameLog->ChangeNameLvalue(name, momname, dadname);

            //int age = 0 + id + i;
            //int momage = 50 + id + i;
            //int dadage = 100 + id + i;

            //ageLog->ChangeAge(age, momage, dadage);
        }

        //nameLog->ChangeName(name, momname, dadname);
        //nameLog->ChangeName(name, momname, dadname);
        //nameLog->ChangeName(name, momname, dadname);
        //nameLog->ChangeName(name, momname, dadname);
        //nameLog->ChangeName(name, momname, dadname);
        //nameLog->ChangeName(name, momname, dadname);
        
        //ageLog->ChangeAge(age, momage, dadage);
        //ageLog->ChangeAge(age, momage, dadage);
        //ageLog->ChangeAge(age, momage, dadage);
        //ageLog->ChangeAge(age, momage, dadage);
        //ageLog->ChangeAge(age, momage, dadage);
        //ageLog->ChangeAge(age, momage, dadage);
        // 
        //pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
        //pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
        //pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
        //pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
        //pL.nameLog->ChangeName("rabin" + idStr, "rabin mom" + idStr, "rabin dad" + idStr);
        //pL.ageLog->ChangeAge(0 + id, 50 + id, 100 + id);
    }
};

struct PersonHandlerNormal
{
    vector<PersonLogNormal> personLogs;

    void Shuffle(std::mt19937& g)
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

            pL.Update(id, idStr);
        }
    }
};

ClusteringMemoryPool<NameLog> nameLogPool = ClusteringMemoryPool<NameLog>(100);
ClusteringMemoryPool<AgeLog> ageLogPool = ClusteringMemoryPool<AgeLog>(100);
vector<PersonHandler> personHandlers = vector<PersonHandler>();

void TestClustering()
{
    for (int a = 0; a < 1000; ++a)
    {
        auto personHandlr = PersonHandler();

        for (int i = 0; i < 10000; ++i)
        {
            auto iid = i + a;
            auto id = to_string(iid);
            auto personLog = PersonLog{ nameLogPool.AddToPool(NameLog{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id })
                                , ageLogPool.AddToPool(AgeLog{ 0 + iid, 50 + iid, 100 + iid }) };
            personHandlr.personLogs.push_back(std::move(personLog));
        }

        personHandlers.push_back(std::move(personHandlr));
    }

    std::random_device rd;
    std::mt19937 g(rd());

    auto start = chrono::high_resolution_clock::now();

    //std::shuffle(personHandlers.begin(), personHandlers.end(), g);

    for (auto& pH : personHandlers)
    {
        //pH.Shuffle(g);        //ToDo: Figure out why this causes funcWrapper class to fuck up.
        pH.Update();
    }

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

    //std::shuffle(personHandlers.begin(), personHandlers.end(), g);

    for (auto& pH : personHandlers)
    {
        //pH->Shuffle(g);
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
    ClusteringMemoryPool<AgeLog> ageLogPool = ClusteringMemoryPool<AgeLog>(8);
    ClusteringMemoryPool<NameLog> nameLogPool = ClusteringMemoryPool<NameLog>(8);

    auto rw_ptr1 = nameLogPool.AddToPool(NameLog{ "rabin",  "rabin mom", "rabin dad" });
    auto rw_ptr2 = ageLogPool.AddToPool(AgeLog{ 200,  200, 200 });

    cout << "Before write: " << rw_ptr1.get()->name << endl;
    cout << "Before write: " << rw_ptr2.get()->age << endl;

    {
        string name = "valid write";
        string momname = "valid write mom";
        string dadname = "valid write dad";
        rw_ptr1.stackingWrite(&NameLog::ChangeNameLvalue, name, momname, dadname);

        int age = 0;
        int momage = 50;
        int dadage = 100;
        rw_ptr2.stackingWrite(&AgeLog::ChangeAge, age, momage, dadage);
    }
    
    {
        nameLogPool.ExecuteClusteredTasks();
        ageLogPool.ExecuteClusteredTasks();
        cout << "After write: " << rw_ptr1.get()->name << endl;
        cout << "After write: " << rw_ptr2.get()->age << endl;
    }
}

int main()
{
    TestNormal();
    TestClustering();
    //TestClusteringPoolWriteValidity();
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
