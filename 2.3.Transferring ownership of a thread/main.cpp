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
    try
    {
        t.join();
    }
    catch(...)
    {
        throw "ff error";
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

}
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
    t4.join();

    //������Ȩ�����ں����ڲ����ݣ�������std::threadʵ����Ϊ�������д���
    gg();

    return 0;
}
