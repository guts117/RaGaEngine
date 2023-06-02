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

struct alignas(alignof(std::string)) NameLog
{
private:
    string name;
    string motherName;
    string fatherName;
public:
    //ToDo: find a way to give compiler error when buffer size is not enough
    alignas(alignof(std::string)) std::byte buffer[sizeof(std::string) * 3];

    NameLog(std::string _name, std::string _motherName, std::string _fatherName)
        : name{ _name }
        , motherName{ _motherName }
        , fatherName{ _fatherName }
    {
        //ToDo: find a way to force people to do this each time a construct is called ? derive from a base class??
        std::memset(buffer, 0, sizeof(buffer));
    }

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

    std::string GetName() const { return name; }
};

struct alignas(alignof(int)) AgeLog
{
private:
    int age;
    int motherAge;
    int fatherAge;
public:
    //ToDo: find a way to give compiler error when buffer size is not enough
    alignas(alignof(int)) std::byte buffer[sizeof(int) * 3];

    AgeLog(int _age, int _motherAge, int _fatherAge)
        : age { _age}
        , motherAge { _motherAge}
        , fatherAge { _fatherAge}
    {
        //ToDo: find a way to force people to do this each time a construct is called ? derive from a base class??
        std::memset(buffer, 0, sizeof(buffer));
    }

    void ChangeAge(int& _age, int& _motherAge, int& _fatherAge)
    {
        age = _age;
        motherAge = _motherAge;
        fatherAge = _fatherAge;
    }

    int GetAge() const { return age; }
};


struct PersonLogNormal
{
    shared_ptr<NameLog> nameLog;
    shared_ptr<AgeLog> ageLog;

    void Update(unsigned int& id, string& idStr)
    {
        auto name = "rabin" + idStr;
        auto momname = "rabin mom" + idStr;
        auto dadname = "rabin dad" + idStr;

        int age = 0 + id;
        int momage = 50 + id;
        int dadage = 100 + id;

        for (int i = 0; i < 1; ++i)
        {
            //auto addstr = to_string(i);
            //string name = "rabin" + idStr + addstr;
            //string momname = "rabin mom" + idStr + addstr;
            //string dadname = "rabin dad" + idStr + addstr;

            nameLog->ChangeNameLvalue(name, momname, dadname);

            //int age = 0 + id + i;
            //int momage = 50 + id + i;
            //int dadage = 100 + id + i;

            ageLog->ChangeAge(age, momage, dadage);
        }
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
            unsigned int id = g();
            auto idStr = to_string(id);
            pL.Update(id, idStr);
        }
    }
};

struct PersonLog
{
    rw_clustering_ptr<NameLog> nameLog;
    rw_clustering_ptr<AgeLog> ageLog;

    void Update(unsigned int& id, string& idStr)
    {
        string name = "rabin" + idStr;
        string momname = "rabin mom" + idStr;
        string dadname = "rabin dad" + idStr;

        int age = 0 + id;
        int momage = 50 + id;
        int dadage = 100 + id;

        for(int i = 0; i< 1; ++i)
        {
            //auto addstr = to_string(i);
            //string name = "rabin" + idStr + addstr;
            //string momname = "rabin mom" + idStr + addstr;
            //string dadname = "rabin dad" + idStr + addstr;

            nameLog.oneTimeWrite(&NameLog::ChangeNameLvalue, name, momname, dadname);

            //int age = 0 + id + i;
            //int momage = 50 + id + i;
            //int dadage = 100 + id + i;

            ageLog.oneTimeWrite(&AgeLog::ChangeAge, age, momage, dadage);
        }
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
            unsigned int id = g();
            string idStr = to_string(id);
            pL.Update(id, idStr);
        }
    }
};

void ExecutePersonHandlerTasks(ClusteringMemoryPool<PersonHandler> & personPool)
{
    personPool.ExecuteClusteredTasks();
}

void ExecuteNameLogTasks(ClusteringMemoryPool<NameLog>& namePool)
{
    namePool.ExecuteClusteredTasks();
}

void ExecuteAgeLogTasks(ClusteringMemoryPool<AgeLog>& agePool)
{
    agePool.ExecuteClusteredTasks();
}

void TestClustering()
{
    ClusteringMemoryPool<NameLog> nameLogPool = ClusteringMemoryPool<NameLog>(10000);
    ClusteringMemoryPool<AgeLog> ageLogPool = ClusteringMemoryPool<AgeLog>(10000);
    ClusteringMemoryPool<PersonHandler> perHandlerPool = ClusteringMemoryPool<PersonHandler>(100);
    vector<rw_clustering_ptr<PersonHandler>> personHandlers = vector<rw_clustering_ptr<PersonHandler>>();

    for (int a = 0; a < 1000; ++a)
    {
        auto personHandlr = PersonHandler();

        for (int i = 0; i < 10000; ++i)
        {
            auto iid = i + a;
            auto id = to_string(iid);
            auto personLog = PersonLog{ nameLogPool.AddToPool(NameLog{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id }), ageLogPool.AddToPool(AgeLog(0 + iid, 50 + iid, 100 + iid)) };
            personHandlr.personLogs.push_back(std::move(personLog));
        }

        personHandlers.emplace_back(perHandlerPool.AddToPool(std::move(personHandlr)));
    }

    std::random_device rd;
    std::mt19937 g(rd());

    auto start = chrono::high_resolution_clock::now();

    //std::shuffle(personHandlers.begin(), personHandlers.end(), g);

    for (auto& pH : personHandlers)
    {
        //pH.Shuffle(g);
        pH.stackingWrite(&PersonHandler::Update);
    }

    ExecutePersonHandlerTasks(perHandlerPool);
    ExecuteNameLogTasks(nameLogPool);
    ExecuteAgeLogTasks(ageLogPool);

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
    vector<PersonHandlerNormal> personHandlers = std::vector<PersonHandlerNormal>();

    for (int a = 0; a < 1000; ++a)
    {
        auto personHandlr = PersonHandlerNormal();

        for (int i = 0; i < 10000; ++i)
        {
            auto id = to_string(i);
            nameLogPool.push_back(std::make_shared<NameLog>(NameLog{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id }));
            ageLogPool.push_back(std::make_shared<AgeLog>(AgeLog{ 0 + i, 50 + i, 100 + i }));
            auto personLog = PersonLogNormal{ nameLogPool.back(), ageLogPool.back()};
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
        //pH.Shuffle(g);
        pH.Update();
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
    ClusteringMemoryPool<NameLog> nameLogPool = ClusteringMemoryPool<NameLog>(1);
    ClusteringMemoryPool<AgeLog> ageLogPool = ClusteringMemoryPool<AgeLog>(1);

    auto rw_ptr1 = nameLogPool.AddToPool(NameLog{ "rabin",  "rabin mom", "rabin dad" });
    auto rw_ptr2 = ageLogPool.AddToPool(AgeLog{ 200,  200, 200 });

    cout << "Before write: " << rw_ptr1.get()->GetName() << endl;
    cout << "Before write: " << rw_ptr2.get()->GetAge() << endl;

    {
        string name = "valid write";
        string momname = "valid write mom";
        string dadname = "valid write dad";
        rw_ptr1.oneTimeWrite(&NameLog::ChangeNameLvalue, name, momname, dadname);

        int age = 0;
        int momage = 50;
        int dadage = 100;
        rw_ptr2.oneTimeWrite(&AgeLog::ChangeAge, age, momage, dadage);
    }
    
    {
        nameLogPool.ExecuteClusteredTasks();
        ageLogPool.ExecuteClusteredTasks();
        cout << "After write: " << rw_ptr1.get()->GetName() << endl;
        cout << "After write: " << rw_ptr2.get()->GetAge() << endl;
    }
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
