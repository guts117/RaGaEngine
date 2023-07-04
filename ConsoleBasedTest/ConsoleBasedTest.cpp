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

//Rule: 
//Entities are general purpose object that holds anything (To be implemented)
//Components are private data with setters and getters
//Behaviours are logic handlers that work on the components that can be parallalized
//Systems are Behaviour handlers that can be parallelized

//There can be multiple components
//Components can't hold other components
//There can be multiple behaviours of a certain system
//Behaviours can hold other behaviours
//Ideally there is only one instanc of any system

//In conclusion: 
//System holds various types of related behaviours
//Behaviour holds various types of components
//Components hold private data with public setters and getter
//Entity is just a unique id to point towards the right logic and data


struct alignas(alignof(SimpleString<32>)) NameLogComponent : POD
{
private:
    SimpleString<32> name;
    SimpleString<32> motherName;
    SimpleString<32> fatherName;
public:
    NameLogComponent(string _name, string _motherName, string _fatherName)
        : name{ std::move(_name) }
        , motherName{ std::move(_motherName) }
        , fatherName{ std::move(_fatherName) }
    {
    }

    void ChangeNameLvalue(SimpleString<32>& _name, SimpleString<32>& _motherName, SimpleString<32>& _fatherName)
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

struct alignas(alignof(int)) AgeLogComponent : POD
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
    int id;

    void Update()
    {
        auto idStr = to_string(id);
        SimpleString<32> name = "valid rabin" + idStr;
        SimpleString<32> momname = "valid rabin mom" + idStr;
        SimpleString<32> dadname = "valid rabin dad" + idStr;

        int age = 0 + id;
        int momage = 50 + id;
        int dadage = 100 + id;

        for (int i = 0; i < 1; ++i)
        {
            //auto addstr = to_string(i);
            //SimpleString<32> name = "rabin" + idStr + addstr;
            //SimpleString<32> momname = "rabin mom" + idStr + addstr;
            //SimpleString<32> dadname = "rabin dad" + idStr + addstr;

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
    vector<shared_ptr<PersonLogNormal>> personLogs;

    void Shuffle(std::mt19937& g)
    {
        std::shuffle(personLogs.begin(), personLogs.end(), g);
    }

    void Update()
    {
        for (auto& pL : personLogs)
        {
            pL->Update();
        }
    }
};

struct PersonLog
{
    rw_clustering_ptr<NameLogComponent> nameLog;
    rw_clustering_ptr<AgeLogComponent> ageLog;
    int id;
};

struct PersonBehaviour : Behaviour
{
private:
    std::vector<PersonLog> personLogs;
public:

    void AddPersonLog(PersonLog&& _personLog)
    {
        personLogs.emplace_back(std::move(_personLog));
    }

    void AddPersonLog(rw_clustering_ptr<NameLogComponent>&& _nameLog, rw_clustering_ptr<AgeLogComponent>&& _ageLog, int& _id)
    {
        personLogs.emplace_back(PersonLog{std::move(_nameLog), std::move(_ageLog), _id});
    }

    void Update()
    {
        for(auto personLogIndex = 0; personLogIndex < personLogs.size(); ++personLogIndex)
        {
            auto& personLog = personLogs[personLogIndex];

            auto idStr = to_string(personLog.id);
            SimpleString<32> name = "valid rabin" + idStr;
            SimpleString<32> momname = "valid rabin mom" + idStr;
            SimpleString<32> dadname = "valid rabin dad" + idStr;

            int age = 0 + personLog.id;
            int momage = 50 + personLog.id;
            int dadage = 100 + personLog.id;

            for (int i = 0; i < 1; ++i)
            {
                //auto addstr = to_string(i);
                //SimpleString<32> name = "rabin" + idStr + addstr;
                //SimpleString<32> momname = "rabin mom" + idStr + addstr;
                //SimpleString<32> dadname = "rabin dad" + idStr + addstr;

                personLog.nameLog.invoke(&NameLogComponent::ChangeNameLvalue, name, momname, dadname);

                //int age = 0 + id + i;
                //int momage = 50 + id + i;
                //int dadage = 100 + id + i;

                personLog.ageLog.invoke(&AgeLogComponent::ChangeAge, age, momage, dadage);

                //R&D Hint: invoke here is much faster because when oneTimeWrite writes on the buffer that is in nameLog/ageLog 
                //i.e. the nameLog/ageLog itself gets pulled in the cache
                //so when nameLogPool.ExecuteClusteredTasksSerial() comes into play it has to pull all again causing worse performance
                //ToDo: Figure out a way so that systems when they write don't pull the whole object in the cache, maybe seperate out the buffer to it's own contigious memory
            }
        }
    }
};

lock_free_thread_pool pool;

struct PersonSystem : public System
{
    vector<rw_clustering_ptr<PersonBehaviour>> personLogs;

    void OnInit(Scene* scene)
    {
        auto factor = std::thread::hardware_concurrency() / 10.0f;

        std::vector<PersonLog> allPersonLogs;

        for (int i = 0; i < 10000000; ++i)
        {
            auto id = to_string(i);

            auto entity = scene->NewEntity();
            auto nameLogPtr = scene->AssignComponent(entity, NameLogComponent("rabin" + id, "rabin mom" + id, "rabin dad" + id), 10000);
            auto ageLogPtr = scene->AssignComponent(entity, AgeLogComponent(0 + i, 50 + i, 100 + i), 10000);
            allPersonLogs.emplace_back(std::move(PersonLog(std::move(nameLogPtr), std::move(ageLogPtr), i)));
        }

        scene->ExecuteClusteredTasksParallel<Scene::Entity>(pool, true);

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(allPersonLogs.begin(), allPersonLogs.end(), g);

        for (int i = 0; i < 1000; ++i)
        {
            auto personBehaviour = PersonBehaviour();
            for (int j = 0; j < 10000; ++j)
            {
                personBehaviour.AddPersonLog(std::move(allPersonLogs[i * 10000 + j]));
            }
            auto entity = scene->NewEntity();
            auto personBehaviourPtr = scene->AssignComponent(entity, std::move(personBehaviour), 100);
            personLogs.emplace_back(std::move(personBehaviourPtr));
        }

        scene->ExecuteClusteredTasksParallel<Scene::Entity>(pool, true);
    }

    void Shuffle()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(personLogs.begin(), personLogs.end(), g);
    }

    void UpdateSerial()
    {
        for (auto& pL : personLogs)
        {
            pL.invoke(&PersonBehaviour::Update);
        }
    }

    void UpdateParallel(Scene* scene)
    {
        for (auto& pL : personLogs)
        {
            pL.stackingWrite(&PersonBehaviour::Update);
        }

        scene->ExecuteClusteredTasksParallel<PersonBehaviour>(pool, true);
        scene->ExecuteClusteredTasksParallel<NameLogComponent>(pool, false);
        scene->ExecuteClusteredTasksParallel<AgeLogComponent>(pool, true);
    }
};

void TestNormal()
{
    vector<shared_ptr<NameLogComponent>> nameLogPool = vector<shared_ptr<NameLogComponent>>();
    vector<shared_ptr<AgeLogComponent>> ageLogPool = vector<shared_ptr<AgeLogComponent>>();
    vector<shared_ptr<PersonHandlerNormal>> personSystemPool = vector<shared_ptr<PersonHandlerNormal>>();

    vector<shared_ptr<PersonLogNormal>> perLogs = vector<shared_ptr<PersonLogNormal>>();

    for (int i = 0; i < 10000000; ++i)
    {
        auto id = to_string(i);
        nameLogPool.push_back(std::make_shared<NameLogComponent>(NameLogComponent{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id }));
        ageLogPool.push_back(std::make_shared<AgeLogComponent>(AgeLogComponent{ 0 + i, 50 + i, 100 + i }));
        auto personLog = std::make_shared<PersonLogNormal>(nameLogPool.back(), ageLogPool.back(), i);
        perLogs.push_back(std::move(personLog));
    }

    std::random_device rd;
    std::mt19937 g(rd());

    for (auto& sys : personSystemPool)
    {
        sys->Shuffle(g);
    }

    auto start = chrono::high_resolution_clock::now();

    for (auto& sys : personSystemPool)
    {
        sys->Update();
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
    //ClusteringMemoryPool<NameLogComponent> nameLogPool = ClusteringMemoryPool<NameLogComponent>(10000000);
    //ClusteringMemoryPool<AgeLogComponent> ageLogPool = ClusteringMemoryPool<AgeLogComponent>(10000000);
    //ClusteringMemoryPool<PersonLogBehaviour> perLogPool = ClusteringMemoryPool<PersonLogBehaviour>(10000000);
    //ClusteringMemoryPool<PersonSubSystem> personSystemPool = ClusteringMemoryPool<PersonSubSystem>(1000);

    //vector<rw_clustering_ptr<PersonLogBehaviour>> perLogs = vector<rw_clustering_ptr<PersonLogBehaviour>>();
    //vector<rw_clustering_ptr<PersonSubSystem>> perSystems = vector<rw_clustering_ptr<PersonSubSystem>>();

    //for (int i = 0; i < 10000000; ++i)
    //{
    //    auto id = to_string(i);
    //    auto personLog = PersonLogBehaviour{ nameLogPool.AddToPool(NameLogComponent{ "rabin" + id,  "rabin mom" + id, "rabin dad" + id }), ageLogPool.AddToPool(AgeLogComponent(0 + i, 50 + i, 100 + i)) ,  i };

    //    perLogs.emplace_back(perLogPool.AddToPool(std::move(personLog)));
    //}

    //std::random_device rd;
    //std::mt19937 g(rd());

    //std::shuffle(perLogs.begin(), perLogs.end(), g);

    //for (int i = 0; i < 1000; ++i)
    //{
    //    auto tempPerVec = vector<rw_clustering_ptr<PersonLogBehaviour>>();
    //    for (int j = 0; j < 10000; ++j)
    //    {
    //        tempPerVec.emplace_back(std::move(perLogs[i * 10000 + j]));
    //    }
    //    perSystems.emplace_back(personSystemPool.AddToPool(std::move(PersonSystem(std::move(tempPerVec)))));
    //}

    //perLogs.clear();
    //perLogs.shrink_to_fit();

    //for (auto& sys : perSystems)
    //{
    //    sys.invoke(&PersonSystem::Shuffle, g);
    //}

    //auto start = chrono::high_resolution_clock::now();

    //for (auto& sys : perSystems)
    //{
    //    sys.invoke(&PersonSystem::UpdateSerial);
    //}

    //perSystems.clear();
    //perSystems.shrink_to_fit();

    //auto end = chrono::high_resolution_clock::now();

    //// Calculating total time taken by the program.
    //double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    //time_taken *= 1e-9;

    //cout << "Time taken by (parallel) clustering memory pool is : " << fixed << time_taken << setprecision(9);
    //cout << " sec" << endl;
}

//ToDo: Fix pool getting destroyed before the scope ends causing threads throwing exception bug.
void TestParallelClusterExecution()
{
    //auto factor = std::thread::hardware_concurrency() / 10.0f;

    Scene scene = Scene(100);
    Stage personStage = Stage();
    PersonSystem personSystem = PersonSystem();
    //personStage.systemPool.push_back(&personSystem);
    
    personSystem.OnInit(&scene);
    personSystem.Shuffle();

    auto start = chrono::high_resolution_clock::now();

    personSystem.UpdateParallel(&scene);

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
        SimpleString<32> name = "valid write";
        SimpleString<32> momname = "valid write mom";
        SimpleString<32> dadname = "valid write dad";
        rw_ptr1.invoke(&NameLogComponent::ChangeNameLvalue, name, momname, dadname);

        int age = 0;
        int momage = 50;
        int dadage = 100;
        rw_ptr2.invoke(&AgeLogComponent::ChangeAge, age, momage, dadage);
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
