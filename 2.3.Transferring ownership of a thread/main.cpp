#include <iostream>
#include <thread>

void some_function()
{
    std::cout<<"some_function"<<std::endl;
}
void some_other_function()
{
    std::cout<<"some_other_function"<<std::endl;
}

void some_other_function_int(int i)
{
    std::cout<<"some_other_function"<<i<<std::endl;
}

std::thread f()
{
  return std::thread(some_function);
}

std::thread g()
{
  std::thread t(some_other_function_int,42);
  return t;
}


void ff(std::thread t)
{
    //����߳����й����з����쳣��֮����õ�join�ᱻ���ԣ�
    //Ϊ����Ҫ�����쳣���ڴ����쳣ʱ����join
    try
    {

    }
    catch(...)
    {
        t.join();
    }
    std::cout<<"ff"<<std::endl;
//    std::cout<<t.get_id();
    t.join();
}
void gg()
{
    //����ʹ��move()�����������ֵ������ֵ������ֵ��thread�ھ�Ϊ = delete
//    ff(std::thread(some_function));
    std::thread a(some_function);
    ff(std::move(a));
    std::cout<<"gg thread id:"<<std::this_thread::get_id()<<std::endl;
}

/*
std::thread֧���ƶ����Դ���thread_guard���ʵ��(������嵥2.3)��
����ӵ���߳�����Ȩ��������thread_guard���������е��߳�ʱ���ƶ������Ϳ���
����ܶ಻��Ҫ���鷳����ĳ������ת�����̵߳�����Ȩ���Ͳ��ܶ��߳̽��л����
���롣Ϊ��ȷ���߳��ڳ����˳�ǰ��ɣ�������scoped_thread�ࡣ���ڣ���������һ��������ͣ�
*/
class scoped_thread
{
  std::thread t;
public:
  explicit scoped_thread(std::thread t_): // 1
    t(std::move(t_))
  {
    if(!t.joinable())  // 2
      throw std::logic_error("No thread");
  }

  ~scoped_thread()
  {
    std::cout<<"scope join"<<std::endl;
    t.join(); // 3
  }
  scoped_thread(scoped_thread const&)=delete;
  scoped_thread& operator=(scoped_thread const&)=delete;
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

void scope()
{
  int some_local_state;
  scoped_thread t(std::thread(func(some_local_state)));    // 4
  std::cout<<"scope"<<std::endl;
} // 5

int main()
{
    std::thread t1(some_function);            // 1
    std::thread t2=std::move(t1);            // 2
    t1=std::thread(some_other_function);    // 3
    t1.join();
    std::thread t3;                            // 4
    t3=std::move(t2);                        // 5
    t3.join();
//    t1=std::move(t3);                        // 6 ��ֵ������ʹ�������
//    t1.join();
/*
���ȣ����߳���t1������١�����ʽʹ��std::move()����t2��ڣ�t1������Ȩ��ת�Ƹ���t2��
֮��t1��ִ���߳��Ѿ�û�й����ˣ�ִ��some_function�ĺ����߳���t2������

Ȼ����ʱstd::thread������ص��߳������ˢۡ�Ϊʲô����ʽ����std::move()ת��
����Ȩ�أ���Ϊ����������һ����ʱ���󡪡��ƶ�����������ʽ�ĵ��á�

t3ʹ��Ĭ�Ϲ��췽ʽ�����ܣ�û�����κ��߳̽��й���������std::move()��t2�����߳�
������Ȩת�Ƶ�t3�Тݡ���Ϊt2��һ������������Ҫ��ʽ�ĵ���std::move()��
�ƶ���������ɺ�t1��ִ��some_other_function���߳��������t2���κ��̶߳��޹�����
t3��ִ��some_function���߳��������

���һ���ƶ���������some_function�̵߳�����Ȩת�Ƣ޸�t1��������t1�Ѿ�����һ��
�������߳�(ִ��some_other_function���߳�)����������ϵͳֱ�ӵ���std::terminate()
��ֹ����������С�������(���׳��쳣��std::terminate()��noexcept����)��Ϊ�˱�֤
��std::thread��������������Ϊһ�¡�2.1.1���У���Ҫ���̶߳�������ǰ����ʽ�ĵȴ�
�߳���ɣ����߷����������и�ֵʱҲ��Ҫ������Щ����(˵��������ͨ������ֵ��
std::thread����ķ�ʽ��"����"һ���߳�)��
*/


    //std::thread֧���ƶ����̵߳�����Ȩ�����ں��������ת��
    t2 = f();
    t2.join();
    std::thread t4 = std::move(g());
    std::cout<<"t4 id:"<<t4.get_id()<<std::endl;//�߳�һ��join��������ӵ��ID
    t4.join();


    //������Ȩ�����ں����ڲ����ݣ�������std::threadʵ����Ϊ�������д���
    gg();

    /*
    ���̻߳�ֱ�Ӵ��ݵ�scoped_thread�Тܣ����Ǵ���һ�����������������̵߳���f()ĩβʱ�ݣ�
    scoped_thread����ͻ����٣�Ȼ���ڹ��캯������ɻ���ۡ�
    ����2.3�е�thread_guard�࣬��Ҫ�������м���߳��Ƿ񡰿ɻ��롱��
    ����Ѽ������˹��캯���Тڣ����ҵ��̲߳��ɻ���ʱ�׳��쳣��
    */
    scope();
    return 0;
}
