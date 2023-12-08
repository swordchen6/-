#include <cstdio>
#include"TestClass.cpp"//这个头文件可以不用管
#include"RealClass.h"
#include<chrono>
#include<vector>
using namespace std;
const int TEST_NUM = 100000;
void test1()
{
    auto start = chrono::steady_clock::now();
    vector<int*> arr;
    for (int i = 0; i < TEST_NUM; i++)
    {
        int* p = new int;
        arr.push_back(p);
    }
    for (int i = 0; i < TEST_NUM; i++)
    {
        delete arr[i];
    }
    auto end= chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "test 1 spend " << time.count() << " milliseconds" << endl;
}
void test2()
{
    TestClass<RealClass,TEST_NUM> p;
    auto start = chrono::steady_clock::now();
    vector<RealClass*> arr;
    for (int i = 0; i < TEST_NUM; i++)
    {
        RealClass* p = new RealClass;
        arr.push_back(p);
    }
    for (int i = 0; i < TEST_NUM; i++)
    {
        delete arr[i];
    }
    auto end = chrono::steady_clock::now();
    auto time = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "test 2 spend " << time.count() << " milliseconds" << endl;
}
int main()
{
    //test()1;
    test2();
}