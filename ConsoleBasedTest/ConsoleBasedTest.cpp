#include <ClusteringMemoryPool.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <chrono>
#include <string>
#include <SimpleString.h>

using namespace std;

//Temporary Rule: 
//Entities are general purpose object that holds anything (To be implemented)
//Components are private data with setters and getters
//Systems are component + logic holders that can be parallelized by the Handlers
//Handlers are purely there for holding similar systems together and concurrent execution

struct alignas(alignof(SimpleString<16>)) NameLogComponent : ClusterableWithBuffer<sizeof(SimpleString<16>) * 3, alignof(SimpleString<16>)>
{
private:
    SimpleString<16> name;
    SimpleString<16> motherName;
    SimpleString<16> fatherName;
public:
    NameLogComponent(string _name, string _motherName, string _fatherName)
        : name{ std::move(_name) }
        , motherName{ std::move(_motherName) }
        , fatherName{ std::move(_fatherName) }
    {
    }

    void ChangeNameLvalue(SimpleString<16>& _name, SimpleString<16>& _motherName, SimpleString<16>& _fatherName)
    {
        name = std::move(_name);
        motherName = std::move(_motherName);
        fatherName = std::move(_fatherName);
        //name = std::move(_name);
        //motherName = std::move(_motherName);
        //fatherName = std::move(_fatherName);
    }

    void ChangeNameRvalue(string& _name, string&& _motherName, string& _fatherName)
    {
        //name = _name.toString();
        //motherName = _motherName.toString();
        //fatherName = _fatherName.toString();
        name = std::move(_name);
        motherName = std::move(_motherName);
        fatherName = std::move(_fatherName);
    }

    const char* GetName() const { return name.getBuffer(); }
};

struct alignas(alignof(int)) AgeLogComponent : ClusterableWithBuffer<sizeof(int) * 3, alignof(int)>
{
private:
    int age;
    int motherAge;
    int fatherAge;
public:
    AgeLogComponent(int _age, int _motherAge, int _fatherAge)
        : age { _age}
        , motherAge { _motherAge}
        , fatherAge { _fatherAge}
    {
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
    shared_ptr<NameLogComponent> nameLog;
    shared_ptr<AgeLogComponent> ageLog;

    void Update(unsigned int& id, string& idStr)
    {
        SimpleString<16> name = "valid rabin" + idStr;
        SimpleString<16> momname = "valid rabin mom" + idStr;
        SimpleString<16> dadname = "valid rabin dad" + idStr;

        int age = 0 + id;
        int momage = 50 + id;
        int dadage = 100 + id;

        for (int i = 0; i < 1; ++i)
        {
            //auto addstr = to_string(i);
            //SimpleString<16> name = "rabin" + idStr + addstr;
            //SimpleString<16> momname = "rabin mom" + idStr + addstr;
            //SimpleString<16> dadname = "rabin dad" + idStr + addstr;

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
        for (auto& pL : personLogs)
        {
            unsigned int id = 1;
            auto idStr = to_string(id);
            pL.Update(id, idStr);
        }
    }
};

struct PersonLogSystem
{
    rw_clustering_ptr<NameLogComponent> nameLog;
    rw_clustering_ptr<AgeLogComponent> ageLog;

    void Update(unsigned int& id, string& idStr)
    {
        SimpleString<16> name = "valid rabin" + idStr;
        SimpleString<16> momname = "valid rabin mom" + idStr;
        SimpleString<16> dadname = "valid rabin dad" + idStr;

        int age = 0 + id;
        int momage = 50 + id;
        int dadage = 100 + id;

        for (int i = 0; i < 1; ++i)
        {
            //auto addstr = to_string(i);
            //SimpleString<16> name = "rabin" + idStr + addstr;
            //SimpleString<16> momname = "rabin mom" + idStr + addstr;
            //SimpleString<16> dadname = "rabin dad" + idStr + addstr;

            nameLog.oneTimeWrite(0, &NameLogComponent::ChangeNameLvalue, name, momname, dadname);

            //int age = 0 + id + i;
            //int momage = 50 + id + i;
            //int dadage = 100 + id + i;

            ageLog.oneTimeWrite(0, &AgeLogComponent::ChangeAge, age, momage, dadage);
        }
    }
};

struct PersonHandler
{
    vector<PersonLogSystem> personLogs;

    void Shuffle(std::mt19937& g)
    {
        std::shuffle(personLogs.begin(), personLogs.end(), g);
    }

    void Update()
    {
        for (auto& pL : personLogs)
        {
            unsigned int id = 1;
            string idStr = to_string(id).c_str();
            pL.Update(id, idStr);
        }
    }
};

void TestNormal()
{
    vector<shared_ptr<NameLogComponent>> nameLogPool = vector<shared_ptr<NameLogComponent>>();
    vector<shared_ptr<AgeLogComponent>> ageLogPool = vector<shared_ptr<AgeLogComponent>>();
    vector<PersonHandlerNormal> personHandlers = std::vector<PersonHandlerNormal>();

    for (int a = 0; a < 1000; ++a)
    {
        auto personHandlr = PersonHandlerNormal();

        for (int i = 0; i < 10000; ++i)
        {
            auto id = to_string(i);
            nameLogPool.push_back(std::make_shared<NameLogComponent>(NameLogComponent{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id }));
            ageLogPool.push_back(std::make_shared<AgeLogComponent>(AgeLogComponent{ 0 + i, 50 + i, 100 + i }));
            auto personLog = PersonLogNormal{ nameLogPool.back(), ageLogPool.back() };
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

void TestSerialClusterExecution()
{
    ClusteringMemoryPool<NameLogComponent> nameLogPool = ClusteringMemoryPool<NameLogComponent>(10000);
    ClusteringMemoryPool<AgeLogComponent> ageLogPool = ClusteringMemoryPool<AgeLogComponent>(10000);
    ClusteringMemoryPool<PersonHandler> perHandlerPool = ClusteringMemoryPool<PersonHandler>(10);
    vector<rw_clustering_ptr<PersonHandler>> personHandlers = vector<rw_clustering_ptr<PersonHandler>>();

    for (int a = 0; a < 1000; ++a)
    {
        auto personHandlr = PersonHandler();

        for (int i = 0; i < 10000; ++i)
        {
            auto iid = i + a;
            auto id = to_string(iid);
            auto personLog = PersonLogSystem{ nameLogPool.AddToPool(NameLogComponent{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id }), ageLogPool.AddToPool(AgeLogComponent(0 + iid, 50 + iid, 100 + iid)) };
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

    perHandlerPool.ExecuteClusteredTasksSerial();
    nameLogPool.ExecuteClusteredTasksSerial();
    ageLogPool.ExecuteClusteredTasksSerial();

    auto end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;

    cout << "Time taken by (serial) clustering memory pool is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;
}

lock_free_thread_pool pool;

//ToDo: Fix pool getting destroyed before the scope ends causing threads throwing exception bug.
void TestParallelClusterExecution()
{
    auto factor = std::thread::hardware_concurrency() / 10.0f;

    ClusteringMemoryPool<NameLogComponent> nameLogPool = ClusteringMemoryPool<NameLogComponent>(10000 * factor);
    ClusteringMemoryPool<AgeLogComponent> ageLogPool = ClusteringMemoryPool<AgeLogComponent>(10000 * factor);
    ClusteringMemoryPool<PersonHandler> perHandlerPool = ClusteringMemoryPool<PersonHandler>(10 * factor);
    vector<rw_clustering_ptr<PersonHandler>> personHandlers = vector<rw_clustering_ptr<PersonHandler>>();

    for (int a = 0; a < 1000; ++a)
    {
        auto personHandlr = PersonHandler();

        for (int i = 0; i < 10000; ++i)
        {
            auto iid = i + a;
            auto id = to_string(iid);
            auto personLog = PersonLogSystem{ nameLogPool.AddToPool(NameLogComponent{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id }), ageLogPool.AddToPool(AgeLogComponent(0 + iid, 50 + iid, 100 + iid)) };
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

    perHandlerPool.ExecuteClusteredTasksParallel(pool, true);
    nameLogPool.ExecuteClusteredTasksParallel(pool, false);
    ageLogPool.ExecuteClusteredTasksParallel(pool, true);

    auto end = chrono::high_resolution_clock::now();

    // Calculating total time taken by the program.
    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;

    cout << "Time taken by (parallel) clustering memory pool is : " << fixed << time_taken << setprecision(9);
    cout << " sec" << endl;
}

void TestClusteringPoolWriteValidity()
{
    ClusteringMemoryPool<NameLogComponent> nameLogPool = ClusteringMemoryPool<NameLogComponent>(1);
    ClusteringMemoryPool<AgeLogComponent> ageLogPool = ClusteringMemoryPool<AgeLogComponent>(1);

    auto rw_ptr1 = nameLogPool.AddToPool(NameLogComponent{ "rabin",  "rabin mom", "rabin dad" });
    auto rw_ptr2 = ageLogPool.AddToPool(AgeLogComponent{ 200,  200, 200 });

    cout << "Before write: " << rw_ptr1.get()->GetName() << endl;
    cout << "Before write: " << rw_ptr2.get()->GetAge() << endl;

    {
        SimpleString<16> name = "valid write";
        SimpleString<16> momname = "valid write mom";
        SimpleString<16> dadname = "valid write dad";
        rw_ptr1.oneTimeWrite(0, &NameLogComponent::ChangeNameLvalue, name, momname, dadname);

        int age = 0;
        int momage = 50;
        int dadage = 100;
        rw_ptr2.oneTimeWrite(0, &AgeLogComponent::ChangeAge, age, momage, dadage);
    }
    
    {
        nameLogPool.ExecuteClusteredTasksParallel(pool, false);
        ageLogPool.ExecuteClusteredTasksParallel(pool, true);

        //while (!pool.isQueueEmpty())
        //{
        //    //std::this_thread::sleep_for(1ms);
        //    std::this_thread::yield();
        //}

        cout << "After write: " << rw_ptr1.get()->GetName() << endl;
        cout << "After write: " << rw_ptr2.get()->GetAge() << endl;
    }
}

int main()
{
    TestParallelClusterExecution();
    TestNormal();
    TestSerialClusterExecution();
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
