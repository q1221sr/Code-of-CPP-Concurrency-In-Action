#include <iostream>
#include<vector>
#include <algorithm>
#include <memory>
#include <thread>
#include <mutex>

template<int N, typename T>
T addValue(T x)
{
  return x + N;
}

template<typename T, typename... Types>
void print(T firstArg, Types... args)
{
  std::cout << sizeof...(Types) << '\n'; // print number of remaining types
  std::cout << sizeof...(args) << '\n'; // print number of remaining args
}

/*
��������һ��ͨ�õĻ��ƣ����䲢�Ǳ����������ݵ�Ψһ��ʽ���кܶ෽ʽ�������ض�����£�
�Թ��������ṩ���ʵı�����

һ���ر𼫶˵�������ǣ����������ڲ������ʺͳ�ʼ��ʱ(����Ҫ����)��
��Ҫ������ʽͬ�������������Ϊ������Ϊֻ����ʽ����������û��ͬ�����⣬
������Ϊ��Ҫ�ı�����Ϊ�����ݲ�����һ���֡��κ�����£����ݳ�ʼ������סһ����������
������Ϊ�˱������ʼ�����̣����һ�����ܴ�������Ҫ��Ӱ�졣

�������ϵ�ԭ��C++��׼�ṩ��һ�ִ��Ᵽ���������ݳ�ʼ�����̵Ļ��ơ�
*/

///3.3.1 �����������ݵĳ�ʼ������
/*
������һ������Դ���������ۺܰ��������ܻ��һ�����ݿ����ӻ������ܶ���ڴ档

�ӳٳ�ʼ��(Lazy initialization)�ڵ��̴߳���ܳ�������ÿһ����������Ҫ�ȶ�Դ���м�飬
Ϊ���˽������Ƿ񱻳�ʼ����Ȼ������ʹ��ǰ�����������Ƿ���Ҫ��ʼ����
*/
typedef int some_resource;
std::shared_ptr<int> resource_ptr;
void foo()
{
  if(!resource_ptr)
  {
    resource_ptr.reset(new some_resource);  // 1
  }
  std::cout<<resource_ptr<<std::endl;
}
/*
תΪ���̴߳���ʱ��ֻ�Тٴ���Ҫ�����������������ݶ��ڲ������ʾ��ǰ�ȫ�ġ�
�������������ת����ʹ���߳���Դ��������Ҫ�����л���
Ϊ��ȷ������Դ�Ѿ���ʼ����ÿ���̱߳���ȴ���������
*/
//����3.11 ʹ���ӳٳ�ʼ��(�̰߳�ȫ)�Ĺ���
std::mutex resource_mutex;

void foo(int i)
{
  std::unique_lock<std::mutex> lk(resource_mutex);  // �����߳��ڴ����л�
  if(!resource_ptr)
  {
    resource_ptr.reset(new some_resource);  // ֻ�г�ʼ��������Ҫ����
  }
  lk.unlock();
  std::cout<<resource_ptr<<i<<std::endl;
}
/*��δ����൱�����ˣ�Ҳ�㹻���ֳ�û��Ҫ���̻߳����⣬�ܶ�����������õ�һ
Щ�İ취��������£����������ǽ�ġ�˫�ؼ����ģʽ����*/
void undefined_behaviour_with_double_checked_locking()
{
  if(!resource_ptr)  // 1
  {
    std::lock_guard<std::mutex> lk(resource_mutex);
    if(!resource_ptr)  // 2
    {
      resource_ptr.reset(new some_resource);  // 3
    }
  }
  std::cout<<resource_ptr<<std::endl;  // 4
}
/*
ָ���һ�ζ�ȡ���ݲ���Ҫ��ȡ���٣�����ֻ����ָ��Ϊ��ʱ����Ҫ��ȡ����Ȼ��
����ȡ��֮�󣬻��ټ��һ��ָ��� (�����˫�ؼ��Ĳ���)��������һ�߳���
��һ�μ���������ʼ���������õ�ǰ�̻߳�ȡ����

���ģʽΪʲô�����ǽ��أ���Ϊ��Ǳ�ڵ�����������δ���������Ķ�ȡ������û��
�������߳��ﱻ��������д������۽���ͬ������˾ͻ���������������������
������������ָ�뱾������Ӱ�쵽��ָ��Ķ��󣻼�ʹһ���߳�֪����һ���߳�
��ɶ�ָ�����д�룬������û�п����´�����some_resourceʵ����Ȼ�����
�ܺ󣬵õ�����ȷ�Ľ���������������һ�ֵ��͵����������������ݾ�����
C++��׼��ָ��Ϊ��δ������Ϊ�������־����ǿ��Ա���ġ��Ķ���5��ʱ��
�����и�����ڴ�ģ�͵����ۣ�Ҳ�������ݾ����Ĺ��ɡ�

C++��׼ίԱ��Ҳ��Ϊ���������Ĵ������Ҫ������C++��׼���ṩ��std::once_flag
��std::call_once���������������������ס����������ʽ�ļ��ָ�룬ÿ���߳�ֻ��Ҫ
ʹ��std::call_once�Ϳ��ԣ���std::call_once�Ľ���ʱ�����ܰ�ȫ��֪��ָ���Ѿ�������
���̳߳�ʼ���ˡ�ʹ��std::call_once����ʽʹ�û��������ĵ���Դ���٣��ر��ǵ���ʼ��
��ɺ����������չʾ�������3.11�е�ͬ���Ĳ���������ʹ����std::call_once��
��������£���ʼ��ͨ�����ú�����ɣ������Ĳ���ʹ�����еĺ�����������ʵ��ͬ���ܼ򵥡�
��ͬ������ڱ�׼���еĺ���һ��������Ϊ���������ã�����Ϊ���������ݣ�
std::call_once���Ժ��κκ�����ɵ��ö���һ��ʹ�á�
*/
std::once_flag resource_flag;  // 1

void init_resource()
{
  resource_ptr.reset(new some_resource);
}

void foo(char c)
{
  std::call_once(resource_flag,init_resource);  // ���������Ľ���һ�γ�ʼ��
  std::cout<<resource_ptr<<c<<std::endl;  // 4
}
/*��������У�std::once_flag�ٺͳ�ʼ���õ����ݶ��������ռ�����Ķ���
��std::call_once()�ɽ���Ϊ�ӳٳ�ʼ�������ͳ�Ա����ͬ���������һ����*/

//����3.12 ʹ��std::call_once��Ϊ���Ա���ӳٳ�ʼ��(�̰߳�ȫ)
//class X
//{
//private:
//  connection_info connection_details;
//  connection_handle connection;
//  std::once_flag connection_init_flag;
//
//  void open_connection()
//  {
//    connection=connection_manager.open(connection_details);
//  }
//public:
//  X(connection_info const& connection_details_):
//      connection_details(connection_details_)
//  {}
//  void send_data(data_packet const& data)  // 1
//  {
//    std::call_once(connection_init_flag,&X::open_connection,this);  // 2
//    connection.send_data(data);
//  }
//  data_packet receive_data()  // 3
//  {
//    std::call_once(connection_init_flag,&X::open_connection,this);  // 2
//    return connection.receive_data();
//  }
//};
/*
�����е�һ�ε���send_data()�ٻ�receive_data()�۵��߳���ɳ�ʼ�����̡�ʹ�ó�Ա����
open_connection()ȥ��ʼ�����ݣ�Ҳ��Ҫ��thisָ�봫��ȥ���ͱ�׼���еĺ���һ����
���ܿɵ��ö��󣬱���std::thread�Ĺ��캯����std::bind()��ͨ����std::call_once()��
����һ������Ĳ�����������������

ֵ��ע����ǣ�std::mutex��std::once_flag��ʵ�����ܿ������ƶ�����Ҫͨ����ʽ����
��Ӧ�ĳ�Ա����������Щ���Ա���в�����

����һ�ֳ�ʼ��������Ǳ������������������һ���ֲ�����Ϊstatic���ͣ����ֱ���������
����Ѿ���ɳ�ʼ�������ڶ��̵߳��õĺ����������ζ������������������������ȥ����
����������ܶ��ڲ�֧��C++11��׼�ı������ϣ���ʵ�������У�����������������ȷʵ
���ڵģ���Ϊ�ڶ��߳��У�ÿ���̶߳���Ϊ�����ǵ�һ����ʼ����������̣߳���һ���߳�
�Ա������г�ʼ����������һ���߳�Ҫʹ���������ʱ����ʼ�����̻�û��ɡ���C++11��׼
�У���Щ���ⶼ������ˣ���ʼ����������ȫ��һ���߳��з���������û�������߳̿���
��ʼ�����ǰ������д�������������ֹ�ڳ�ʼ���׶Σ���������֮����ȥ����õĶࡣ
��ֻ��Ҫһ��ȫ��ʵ������£������ṩһ��std::call_once���������
*/
class my_class
{

};
my_class& get_my_class_instance()
{
  static my_class instance;  // �̰߳�ȫ�ĳ�ʼ������
  return instance;
}
/*���߳̿��԰�ȫ�ĵ���get_my_class_instance()�ٺ���������Ϊ���ݾ��������ġ�

���ں����и��µ����ݽṹ��˵��ֻ�ڳ�ʼ��ʱ�������ݡ����������£��������ݽṹ��
ֻ���ģ����Ҷ��̶߳��䲢���Ķ�ȡҲ�Ǻ����ģ�����һ�����ݽṹ��Ҫ���¾ͻ����������*/

///3.3.2 �����������µ����ݽṹ
/*
����Ϊ�˽���������Ϊ�����IP��ַ���ڻ����еĴ����һ��DNS��ڱ�ͨ��������DNS��Ŀ
�ںܳ���ʱ���ڱ��ֲ��䡣��Ȼ���û����ʲ�ͬ��վʱ���µ���ڿ��ܻᱻ��ӵ����У�
������Щ���ݿ����������������ڱ��ֲ��䡣���Զ��ڼ�黺������ڵ���Ч�Ծͱ��ʮ��
��Ҫ����Ҳ��Ҫһ�θ��£�Ҳ����θ���ֻ�Ƕ�һЩϸ�����˸Ķ���

��Ȼ����Ƶ�Ⱥܵͣ���Ҳ�п��ܷ��������ҵ�����̷߳��ʻ���ʱ������������Ҫ����
����ʱ״̬��״̬��Ҳ��Ϊ��ȷ��ÿ���̶߳���������Ч���ݡ�

û��ʹ��ר�����ݽṹʱ�����ַ�ʽ�Ƿ���Ԥ�ڵģ���Ϊ�������ºͶ�ȡ�������ر����
(����������ڵ�6�͵�7���н���)�������ĸ���Ҫ���̶߳�ռ���ݽṹ�ķ���Ȩ��ֱ������
������ɡ�����ɸ���ʱ�����ݽṹ���ڲ������̵߳ķ����ֻ��ǰ�ȫ�ġ�ʹ��std::mutex
���������ݽṹ���о���Щ��Ӧ����(��Ϊ��û�з����޸�ʱ����������������ȡ���ݵ�
������)��������Ҫ��һ�ֲ�ͬ�Ļ����������ֻ�����������Ϊ������-д����������Ϊ������
���ֲ�ͬ��ʹ�÷�ʽ��һ����д�ߡ��̶߳�ռ���ʺ͹�����ʣ��ö�������ߡ��̲߳������ʡ�

C++17��׼���ṩ�����ַǳ��õĻ���������std::shared_mutex��std::shared_timed_mutex��
C++14ֻ�ṩ��std::shared_timed_mutex��������C++11�в�δ�ṩ�κλ��������͡����
������֧��C++14��׼֮ǰ�ı�����������ʹ��Boost���еĻ�������std::shared_mutex��
std::shared_timed_mutex�Ĳ�ͬ�����ڣ�std::shared_timed_mutex֧�ָ���Ĳ�����ʽ
(�ο�4.3��)��std::shared_mutex�и��ߵ��������ƣ���֧�ֵĲ������١�

��8���лῴ������������Ҳ���ܰ��ΰٲ��������������ڲ������еĴ�����������ͬ��Ҳ
����ߺ�д���̵߳ĸ����йء�Ϊ��ȷ�����Ӹ��ӶȺ��ܻ���������棬Ŀ��ϵͳ�ϵ�
�������ܾͺ���Ҫ��

����ʹ��std::mutexʵ������ͬ��������ʹ��std::shared_mutex����ͬ�������ڸ��²�����
����ʹ��std::lock_guard<std::shared_mutex>��std::unique_lock<std::shared_mutex>
��������Ϊstd::mutex�������������std::mutex������һ��������ܱ�֤�����̵߳Ķ�ռ
���ʡ���Щ�����޸����ݽṹ���̣߳�����ʹ��std::shared_lock<std::shared_mutex>
��ȡ����Ȩ������RAII����ģ������C++14�е������ԣ�����ʹ��std::unique_lockһ����
���˶��߳̿���ͬʱ��ȡͬһ��std::shared_mutex�Ĺ�������Ψһ�����ƣ������߳�ӵ��
������ʱ�����Ի�ȡ��ռ�����̻߳ᱻ������ֱ�����������̷߳�����������һ�߳�ӵ��һ
����ռ��ʱ�������߳̾��޷���ù��������ռ����ֱ����һ���̷߳�����ӵ�е�����

��֮ͬǰ����������������Ĵ����嵥չʾ��һ���򵥵�DNS���棬ʹ��std::map���л���
���ݣ�ʹ��std::shared_mutex���б�����
*/
//����3.13 ʹ��std::shared_mutex�����ݽṹ���б���
#include <map>
#include <string>
#include <mutex>
#include <shared_mutex>

class dns_entry
{

};

class dns_cache
{
  std::map<std::string,dns_entry> entries;
  mutable std::shared_mutex entry_mutex;
public:
  dns_entry find_entry(std::string const& domain) const
  {
    std::shared_lock<std::shared_mutex> lk(entry_mutex);  // 1
    std::map<std::string,dns_entry>::const_iterator const it=
       entries.find(domain);
    return (it==entries.end())?dns_entry():it->second;
  }
  void update_or_add_entry(std::string const& domain,
                           dns_entry const& dns_details)
  {
    std::lock_guard<std::shared_mutex> lk(entry_mutex);  // 2
    entries[domain]=dns_details;
  }
};
/*����3.13�У�find_entry()ʹ��std::shared_lock<>�����������ֻ��Ȩ�ޢ١�
���ʹ�ö��߳̿���ͬʱ����find_entry()���Ҳ��������һ���棬
update_or_add_entry()ʹ��std::lock_guard<>ʵ�����������Ҫ����ʱ�ڣ�Ϊ���ṩ
��ռ����Ȩ�ޡ�update_or_add_entry()��������ʱ����ռ������ֹ�����̶߳����ݽṹ
�����޸ģ�������ֹ�̵߳���find_entry()��*/

///3.3.3 Ƕ����
/*
�̶߳��Ѿ���ȡ��std::mutex(�Ѿ�����)�ٴ������Ǵ���ģ������������ᵼ��δ������Ϊ��
��ĳЩ����£�һ���᳢̻߳�����ͷ�һ��������ǰ��λ�ȡ����ˣ�C++��׼���ṩ��
std::recursive_mutex�ࡣ���˿�����ͬһ�̵߳ĵ���ʵ���϶������������������
std::mutex��ͬ�������̶߳Ի���������ǰ����ǰ�̱߳����ͷ�ӵ�е������������������
����lock()���Σ�Ҳ�������unlock()���Ρ���ȷʹ��std::lock_guard<std::recursive_mutex>
��std::unique_lock<std::recursive_mutex>���԰��㴦����Щ���⡣

ʹ��Ƕ����ʱ��Ҫ�Դ�����ƽ��иĶ���Ƕ����һ�����ڿɲ������ʵ����ϣ�����ʹ�û�����
�������Ա���ݡ�ÿ��������Ա��������Ի�����������Ȼ����ɶ�Ӧ�Ĳ������ٽ�����������
��������ʱ��Ա�����������һ����Ա��������������£��ڶ�����Ա����Ҳ����ͼ��ס��������
��ͻᵼ��δ������Ϊ�ķ���������ͨ�ġ���������Ὣ������תΪǶ�������ڶ�����Ա����
���ܳɹ��Ľ������������Һ����ܼ���ִ�С�

�������ַ�ʽ���ڲ��ʺͲ��������Բ��Ƽ�������ʹ�÷�ʽ���ر��ǣ���Ӧ��Ĳ�����ͨ���ᱻ�ƻ���
����ζ�ţ������������ƻ�ʱ���ڶ�����Ա��������Ҫ����ִ�С�һ���ȽϺõķ�ʽ�ǣ�����
��ȡ��һ��������Ϊ���˽�г�Ա�����˽�г�Ա��������Ի�������������(����ǰ��������)��
Ȼ����Ҫ��ϸ����һ�£�������������º���ʱ���ݵ�״̬��
*/
int main()
{
    std::vector<int> v{ 0, 1, 2};
    std::vector<int> v2(3);
    std::transform(v.begin(), v.end(), v2.begin(), addValue<1, int>);
    for (auto x : v2)
        std::cout << x<<std::endl; // 123
    print(1, "shjs", "dsjak", "dsjak", 3, 7);
    return 0;
}
