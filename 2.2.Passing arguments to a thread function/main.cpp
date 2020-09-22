#include <iostream>
#include <thread>

/*
��ɵ��ö���������ݲ����ܼ򵥣�ֻ��Ҫ����Щ������Ϊ
std::thread���캯���ĸ��Ӳ������ɡ���Ҫע����ǣ���Щ����
�´�������̵߳��ڴ�ռ���(ͬ��ʱ����һ��)����ʹ�����еĲ��������õ���ʽ��
��������Ҳ��ִ��
*/
void f(int i, std::string const& s)
{
    std::cout<<"f"<<i<<s<<std::endl;
}
void ff(int& i)
{
    std::cout<<"ff"<<i<<std::endl;
}

void oops(int some_param)
{
  char buffer[1024]; // 1
  sprintf(buffer, "%i",some_param);
  std::thread t(f,3,buffer); // 2
  t.detach();
}

/*
buffer����һ��ָ�������ָ��ֲ�������Ȼ��˾ֲ�����ͨ��buffer���ݵ����߳��Тڡ�
��ʱ������oops���ܻ���bufferת����std::string֮ǰ�������Ӷ�����δ�������Ϊ��
��Ϊ���޷���֤��ʽת���Ĳ�����std::thread���캯���Ŀ���������˳��
�п���std::thread�Ĺ��캯����������ת��ǰ�ı���(bufferָ��)���������������
���ݵ�std::thread���캯��֮ǰ���ͽ�����ֵת��Ϊstd::string
*/
void not_oops(int some_param)
{
  char buffer[1024];
  sprintf(buffer,"%i",some_param);
  std::thread t(f,3,std::string(buffer));  // ʹ��std::string����������ָ��
  t.detach();
}

class X
{
public:
  void do_lengthy_work()
  {
      std::cout<<"do_lengthy_work"<<std::endl;
  }
  void do_lengthy_work2(int i)
  {
      std::cout<<"do_lengthy_work"<<i<<std::endl;
  }
};

int main()
{
    std::thread t1(f, 3, "hello");
    t1.join();
//    oops(112);
    not_oops(112);

    X my_x;
    std::thread t2(&X::do_lengthy_work, &my_x);
    t2.join();

    /*
    ff()�ڴ�����һ�����ã���std::thread�Ĺ��캯���ڲ���֪����
    ���캯�����Ӻ����������ͣ�äĿ�ؿ������ṩ�ı������������ڲ�����Ὣ����
    �Ĳ�������ֵ�ķ�ʽ���д��ݣ�����Ϊ����Щֻ֧���ƶ������ͣ�����᳢����
    ��ֵΪʵ�ε���update_data_for_widget������Ϊ������������һ���ǳ�������
    ��Ϊ����(������ֵ)�����Ի��ڱ���ʱ����
    */
    int i = 1;
    int& num = i;
    std::thread t3(ff, std::ref(num));
    t3.join();

    /*
    ʹ�á��ƶ���ת�ƶ�������Ȩ�󣬾ͻ�����һ����ָ�롣ʹ���ƶ��������Խ�����
    ת���ɺ����ɽ��ܵ�ʵ�����ͣ������㺯������ֵ����Ҫ�󡣵�ԭ��������ʱ����ʱ��
    ���Զ������ƶ�����������ԭ������һ������������ת�Ƶ�ʱ�����Ҫʹ��std::move()
    ������ʾ�ƶ���
    */
    int num2(0);
    std::thread t4(&X::do_lengthy_work2, &my_x, num2);
//    std::thread t4(&X::do_lengthy_work2, &my_x, std::move(num2));
    t4.join();
    return 0;
}
