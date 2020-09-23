#include <iostream>
#include <mutex>
#include<thread>
#include <list>
#include <algorithm>
#include <deque>
#include <exception>
#include <memory>
#include <stack>

/*
ͨ��ʵ����std::mutex����������ʵ������Ա����lock()�ɶԻ�����������unlock()Ϊ������
���������Ƽ�ֱ��ȥ���ó�Ա���������ó�Ա��������ζ�ţ�������ÿ���������ڶ�Ҫȥ����
unlock()(�����쳣�����)��C++��׼��Ϊ�������ṩ��RAIIģ����std::lock_guard��
�ڹ���ʱ�����ṩ�����Ļ���������������ʱ���н������Ӷ���֤�˻������ܱ���ȷ����
*/

std::list<int> some_list;    // 1
std::mutex some_mutex;    // 2��ȫ�ֵĻ�����������ȫ�ֱ���some_list

void add_to_list(int new_value)
{
  std::lock_guard<std::mutex> guard(some_mutex);    // 3
  some_list.push_back(new_value);
}

//list_contains()�����ܿ������ڱ�add_to_list()�޸ĵ��б���Ϊ���Ƕ�ʹ����
//std::lock_guard<std::mutex>�����Զ��߶������ݵķ����ǻ����
bool list_contains(int value_to_find)
{
//  std::lock_guard<std::mutex> guard(some_mutex);    // 4
  std::lock_guard guard(some_mutex);//c++17������ģ��������Ƶ�����һ�д�����Լ�Ϊ���д���
  return std::find(some_list.begin(),some_list.end(),value_to_find) != some_list.end();
}

/*
ĳЩ�����ʹ��ȫ�ֱ���û���⣬�����������£�������ͨ��������Ҫ���������ݷ���ͬһ���У������Ƕ����ȫ�ֱ�����
�������������Ƶ�׼�򣺽������һ�����У��Ϳ���������ϵ��һ��Ҳ�ɶ���Ĺ��ܽ��з�װ�����������ݱ�����
��������£�����add_to_list��list_contains������Ϊ�����ĳ�Ա����������������Ҫ���������ݣ������ж�����Ϊprivate��Ա��
����ô�������������ҷ����˽�ʲôʱ��Ի��������������г�Ա���������ڵ���ʱ����������������ʱ�����ݽ�����
��ͱ�֤�˷���ʱ���ݲ�������״̬�ȶ���

��Ȼ��Ҳ����������ô���룺������һ����Ա�������ص��Ǳ������ݵ�ָ�������ʱ��Ҳ���ƻ����ݡ�
���з���������ָ������ÿ��Է���(�������޸�)�������ݣ������ᱻ���������ơ�
�����Ҫ�Խӿڽ�����ƣ�Ҫȷ������������ס���ݷ��ʣ����Ҳ������š�
*/

/*
ʹ�û��������������ݣ���������ÿһ����Ա�����м���һ��std::lock_guard������ô�򵥡�
һ��ָ������ã�Ҳ�������ֱ�����ͬ���衣���������ָ������ú����ף�ֻҪû�г�Ա����
ͨ������ֵ���������������ʽ����������߷���ָ���ܱ������ݵ�ָ������ã����ݾ��ǰ�ȫ�ġ�
ȷ����Ա�������ᴫ��ָ������õ�ͬʱ������Ա�����Ƿ�ͨ��ָ������õķ�ʽ������Ҳ�Ǻ���Ҫ��
(�������������������Ŀ�����ʱ)����������û�ڻ����������������ڴ洢ָ������ã������ͺ�Σ�ա�
��Σ�յ��ǣ�������������Ϊһ������ʱ��������ͬ�����������ʾ��
*/
class some_data
{
  int a;
  std::string b;
public:
  void do_something()
  {
      std::cout<<a<<b<<std::endl;
  }
};

class data_wrapper
{
private:
  some_data data;
  std::mutex m;
public:
  template<typename Function>
  void process_data(Function func)
  {
    std::lock_guard<std::mutex> l(m);
    func(data);    // 1 ���ݡ����������ݸ��û�����
  }
};

some_data* unprotected;

void malicious_function(some_data& protected_data)
{
  unprotected=&protected_data;
}

data_wrapper x;
void foo()
{
  x.process_data(malicious_function);    // 2 ����һ�����⺯��
  unprotected->do_something();    // 3 ���ޱ���������·��ʱ�������
}
/*
process_data������û�����⣬std::lock_guard���������˺ܺõı������������û��ṩ�ĺ���func�٣�
����ζ��foo�ܹ��ƹ��������ƽ�����malicious_function���ݽ�ȥ�ڣ�������û������������������µ���do_something()

��δ�����������ڸ���û�б�����ֻ�ǽ����пɷ��ʵ����ݽṹ������Ϊ���⡣����foo()�е���unprotected->do_something()
�Ĵ���δ�ܱ����Ϊ���⡣��������£�C++�޷��ṩ�κΰ�����ֻ���ɿ�����ʹ����ȷ�Ļ��������������ݡ�
���ֹ۵ĽǶ��Ͽ��������з����ģ������ܱ������ݵ�ָ������ô��ݵ�������������֮�⡣
*/

/*
ʹ���˻��������������Ʊ����˹������ݣ��Ͳ�����Ϊ���������������𣿲����ǣ�������Ҫȷ�������Ƿ��ܵ��˱�����
����֮ǰ˫��������ӣ�Ϊ�������̰߳�ȫ��ɾ��һ���ڵ㣬��Ҫȷ����ֹ���������ڵ�(��ɾ���Ľڵ㼰��ǰ�����ڵĽڵ�)�Ĳ������ʡ�
���ֻ��ָ��ÿ���ڵ��ָ����з��ʱ������Ǿͺ�û��ʹ�û�����һ�������������Իᷢ����������ָ�룬
�������ݽṹ������ɾ��������Ҫ�����������������򵥵Ľ����������ʹ�û���������������������add_to_list()��ʾ��

��������ĸ�������ǰ�ȫ�ģ������ɿ��������������������磬����һ��������std::stack��ջ(������Ĵ���)��
���˹��캯����swap()���⣬��Ҫ��std::stack�ṩ���������push()һ����Ԫ�ؽ�ջ��pop()һ��Ԫ�س�ջ��
top()�鿴ջ��Ԫ�أ�empty()�ж�ջ�Ƿ��ǿ�ջ��size()�˽�ջ���ж��ٸ�Ԫ�ء���ʹ�޸���top()��
����һ��������������(����ѭ��3.2.2�ڵ�׼��)������ӿ��Դ�������������������ⲻ�������ڻ�����ʵ�ֽӿ��У�
������ʵ�ֽӿ��У�Ҳ������������������ǽӿڵ����⣬��ʵ�ַ�ʽ�޹ء�
*/
template<typename T,typename Container=std::deque<T> >
class stack
{
public:
  explicit stack(const Container&);
  explicit stack(Container&& = Container());
  template <class Alloc> explicit stack(const Alloc&);
  template <class Alloc> stack(const Container&, const Alloc&);
  template <class Alloc> stack(Container&&, const Alloc&);
  template <class Alloc> stack(stack&&, const Alloc&);

  bool empty() const;
  size_t size() const;
  T& top();
  T const& top() const;
  void push(T const&);
  void push(T&&);
  void pop();
  void swap(stack&&);
  template <class... Args> void emplace(Args&&... args); // C++14��������
};
/*
��Ȼempty()��size()�����ڷ���ʱ����ȷ�ģ���������ɿ��������غ������߳̾Ϳ������ɵط���ջ��
���ҿ���push()�����Ԫ�ص�ջ�У�Ҳ����pop()һЩ����ջ�е�Ԫ�ء������Ļ���
֮ǰ��empty()��size()�õ�����ֵ���������ˡ�
*/


struct empty_stack: std::exception
{
  const char* what() const throw() {
	return "empty stack!";
  };
};

/**�̰߳�ȫ��ջ*/
template<typename T>
class threadsafe_stack
{
private:
  std::stack<T> data;
  mutable std::mutex m;

public:
  threadsafe_stack()
	: data(std::stack<T>()){}

  threadsafe_stack(const threadsafe_stack& other)
  {
    std::lock_guard<std::mutex> lock(other.m);
    data = other.data; // 1 �ڹ��캯�����е�ִ�п���
  }

  threadsafe_stack& operator=(const threadsafe_stack&) = delete;

  void push(T new_value)
  {
    std::lock_guard<std::mutex> lock(m);
    data.push(new_value);
  }

  std::shared_ptr<T> pop()
  {
    std::lock_guard<std::mutex> lock(m);
    if(data.empty()) throw empty_stack(); // �ڵ���popǰ�����ջ�Ƿ�Ϊ��

    std::shared_ptr<T> const res(std::make_shared<T>(data.top())); // ���޸Ķ�ջǰ�����������ֵ
    data.pop();
    return res;
  }

  void pop(T& value)
  {
    std::lock_guard<std::mutex> lock(m);
    if(data.empty()) throw empty_stack();

    value=data.top();
    data.pop();
  }

  bool empty() const
  {
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
  }
};
/*
��ջ���Կ��������������캯���Ի������������ٿ�����ջ�����캯�����ТٵĿ���
ʹ�û�������ȷ�����ƽ������ȷ�ԣ������ķ�ʽ�ȳ�Ա��ʼ���б�á�

֮ǰ��top()��pop()�����������У���Ϊ��������̫С���������������Ѿ����֣���Ҫ�����Ĳ�����δȫ���ǵ���
���������Ŀ����ȹ���ͬ���������⡣����һ�����⣬һ��ȫ�ֻ�����Ҫȥ����ȫ���������ݣ�
��һ��ϵͳ�д����д����Ĺ�������ʱ���߳̿���ǿ�����У��������Է��ʲ�ͬλ�õ����ݣ�
�����˲���������������������һ��Ϊ�ദ����ϵͳ���Linux�ں��У���ʹ����һ��ȫ���ں�����
���������������������˫�˴���ϵͳ���ϵ�����Ҫ����������ϵͳ�����ܲ�ܶ࣬�ĺ�ϵͳ�͸��������ˡ�
̫������ȥ����ռ���ںˣ�ʹ�������ڴ��������е��߳�û�а취�ܺõĹ��������������Linux�ں˼�����һ��
ϸ��������������Ϊ���˺ܶ��ں˾�������ʱ�ĺ˴���ϵͳ�����ܾͺ͵��˴�����ı�����ˡ�

ʹ�ö���������������е����ݣ�ϸ������Ҳ�����⡣��ǰ�����������󻥳����������ݵ�����ʱ��ֻ��Ҫ��סһ����������
�����ַ������Ƿ�֮�ĺ���׼������������һ���������ʵ��������״̬����һ���׶Σ������뿪���������������򻹸��û���
�����ж����Ļ�����ȥ����������ȫ��ʵ�������ַ�ʽ������ô�á�

һ������������Ҫ�������������ϵĻ�����ʱ����һ��Ǳ�ڵ����⽫���֣�������
������������ȫ�෴������ͬ�������̻߳ụ��ȴ����Ӷ�ʲô��û����
*/

int main()
{
    if(list_contains(42))
        std::cout << "42 included" << std::endl;
    add_to_list(42);
    if(list_contains(42))
        std::cout << "42 included" << std::endl;
    foo();
    std::cout << "" << std::endl;
    return 0;
}
