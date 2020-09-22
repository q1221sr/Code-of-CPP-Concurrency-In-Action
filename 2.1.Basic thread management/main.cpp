#include <iostream>
#include<thread>
#include<assert.h>

void do_something()
{
    std::cout<<"do something"<<std::endl;
}

void do_something(int i)
{
    std::cout<<"do something"<<i<<std::endl;
}

void do_something_else()
{
    std::cout<<"do something else"<<std::endl;
}

class background_task
{
public:
  void operator()() const
  {
    do_something();
    do_something_else();
  }
};

struct func
{
  int& i;
  func(int& i_) : i(i_) {}
  void operator() ()
  {
    for (unsigned j=0 ; j<1000000 ; ++j)
    {
//      do_something(i);           //Ǳ�ڷ���������������
    }
  }
};

void oops()
{
  int some_local_state=0;
  func my_func(some_local_state);
  std::cout<<"oops"<<std::endl;
  std::thread my_thread(my_func);
  my_thread.detach();          //���뵱ǰ�̣߳����̲߳��ȴ���ǰ�߳̽�����������ᵼ�µ�ǰ�߳��ں�̨����ʱ�������߳��Ѿ����ٵľֲ�����
}

void f()
{
  int some_local_state=0;
  func my_func(some_local_state);
  std::thread t(my_func);
  //����Ӧ�ñ��׳����쳣����ֹ��ͨ���������쳣�������ʹ��join()ʱ����Ҫ���쳣��������е���join()���Ӷ������������ڵ����⡣
  try
  {
    std::cout<<"����:"<<std::endl;
  }
  catch(...)
  {
    t.join();  // 1
    throw  "f error";;
  }
  t.join();  // 2
}

class thread_guard
{
  std::thread& t;
public:
  explicit thread_guard(std::thread& t_):
    t(t_)
  {}
  ~thread_guard()
  {
    std::cout<<"��ʼ����"<<std::endl;
//    if(t.joinable()) // 1
//    {
    assert(t.joinable());
      t.join();      // 2
//    }
  }
  thread_guard(thread_guard const&)=delete;   // 3
  thread_guard& operator=(thread_guard const&)=delete;
};

void ff()
{
  int some_local_state=0;
  func my_func(some_local_state);
  std::thread t(my_func);
  std::cout<<"ff"<<std::endl;
  thread_guard g(t);
}    // 4

int main()
{
//    std::thread myThread(do_something);
//    myThread.join();
    background_task f;
//    std::thread my_thread(f);
//    std::thread my_thread((background_task()));//���һ������������������ȱ�����Ŵ��ݵ�����ʱ�������������Ὣ�����Ϊ��������
    std::thread my_thread{background_task()};//ͳһ�ĳ�ʼ���﷨
    my_thread.join();
//    oops();
//    f();
    ff();   //��Ҫ��ʾ����������������ݣ����func�ṹ�����do_something()����ע�͵�
    static_assert(true);        //��̬���ԣ������ڳ���
    return 0;
}
