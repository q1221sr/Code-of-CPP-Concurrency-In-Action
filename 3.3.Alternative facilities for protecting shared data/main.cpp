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
互斥量是一种通用的机制，但其并非保护共享数据的唯一方式。有很多方式可以在特定情况下，
对共享数据提供合适的保护。

一个特别极端的情况就是，共享数据在并发访问和初始化时(都需要保护)，
需要进行隐式同步。这可能是因为数据作为只读方式创建，所以没有同步问题，
或者因为必要的保护作为对数据操作的一部分。任何情况下，数据初始化后锁住一个互斥量，
纯粹是为了保护其初始化过程，并且会给性能带来不必要的影响。

出于以上的原因，C++标准提供了一种纯粹保护共享数据初始化过程的机制。
*/

///3.3.1 保护共享数据的初始化过程
/*
假设有一个共享源，构建代价很昂贵，它可能会打开一个数据库连接或分配出很多的内存。

延迟初始化(Lazy initialization)在单线程代码很常见——每一个操作都需要先对源进行检查，
为了了解数据是否被初始化，然后在其使用前决定，数据是否需要初始化：
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
转为多线程代码时，只有①处需要保护，这样共享数据对于并发访问就是安全的。
但是下面天真的转换会使得线程资源产生不必要的序列化，
为了确定数据源已经初始化，每个线程必须等待互斥量。
*/
//代码3.11 使用延迟初始化(线程安全)的过程
std::mutex resource_mutex;

void foo(int i)
{
  std::unique_lock<std::mutex> lk(resource_mutex);  // 所有线程在此序列化
  if(!resource_ptr)
  {
    resource_ptr.reset(new some_resource);  // 只有初始化过程需要保护
  }
  lk.unlock();
  std::cout<<resource_ptr<<i<<std::endl;
}
/*这段代码相当常见了，也足够表现出没必要的线程化问题，很多人能想出更好的一
些的办法来做这件事，包括声名狼藉的“双重检查锁模式”：*/
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
指针第一次读取数据不需要获取锁①，并且只有在指针为空时才需要获取锁。然后，
当获取锁之后，会再检查一次指针② (这就是双重检查的部分)，避免另一线程在
第一次检查后再做初始化，并且让当前线程获取锁。

这个模式为什么声名狼藉呢？因为有潜在的条件竞争。未被锁保护的读取操作①没有
与其他线程里被锁保护的写入操作③进行同步，因此就会产生条件竞争，这个条件
竞争不仅覆盖指针本身，还会影响到其指向的对象；即使一个线程知道另一个线程
完成对指针进行写入，它可能没有看到新创建的some_resource实例，然后调用
④后，得到不正确的结果。这个例子是在一种典型的条件竞争——数据竞争，
C++标准中指定为“未定义行为”，这种竞争是可以避免的。阅读第5章时，
那里有更多对内存模型的讨论，也包括数据竞争的构成。

C++标准委员会也认为条件竞争的处理很重要，所以C++标准库提供了std::once_flag
和std::call_once来处理这种情况。比起锁住互斥量并显式的检查指针，每个线程只需要
使用std::call_once就可以，在std::call_once的结束时，就能安全的知晓指针已经被其他
的线程初始化了。使用std::call_once比显式使用互斥量消耗的资源更少，特别是当初始化
完成后。下面的例子展示了与代码3.11中的同样的操作，这里使用了std::call_once。
这种情况下，初始化通过调用函数完成，这样的操作使用类中的函数操作符来实现同样很简单。
如同大多数在标准库中的函数一样，或作为函数被调用，或作为参数被传递，
std::call_once可以和任何函数或可调用对象一起使用。
*/
std::once_flag resource_flag;  // 1

void init_resource()
{
  resource_ptr.reset(new some_resource);
}

void foo(char c)
{
  std::call_once(resource_flag,init_resource);  // 可以完整的进行一次初始化
  std::cout<<resource_ptr<<c<<std::endl;  // 4
}
/*这个例子中，std::once_flag①和初始化好的数据都是命名空间区域的对象，
但std::call_once()可仅作为延迟初始化的类型成员，如同下面的例子一样：*/

//代码3.12 使用std::call_once作为类成员的延迟初始化(线程安全)
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
例子中第一次调用send_data()①或receive_data()③的线程完成初始化过程。使用成员函数
open_connection()去初始化数据，也需要将this指针传进去。和标准库中的函数一样，
接受可调用对象，比如std::thread的构造函数和std::bind()，通过向std::call_once()②
传递一个额外的参数来完成这个操作。

值得注意的是，std::mutex和std::once_flag的实例不能拷贝和移动，需要通过显式定义
相应的成员函数，对这些类成员进行操作。

还有一种初始化过程中潜存着条件竞争：其中一个局部变量为static类型，这种变量在声明
后就已经完成初始化。对于多线程调用的函数，这就意味着这里有条件竞争——抢着去定义
这个变量。很多在不支持C++11标准的编译器上，在实践过程中，这样的条件竞争是确实
存在的，因为在多线程中，每个线程都认为他们是第一个初始化这个变量线程，或一个线程
对变量进行初始化，而另外一个线程要使用这个变量时，初始化过程还没完成。在C++11标准
中，这些问题都被解决了：初始化及定义完全在一个线程中发生，并且没有其他线程可在
初始化完成前对其进行处理，条件竞争终止于初始化阶段，这样比在之后再去处理好的多。
在只需要一个全局实例情况下，这里提供一个std::call_once的替代方案
*/
class my_class
{

};
my_class& get_my_class_instance()
{
  static my_class instance;  // 线程安全的初始化过程
  return instance;
}
/*多线程可以安全的调用get_my_class_instance()①函数，不用为数据竞争而担心。

对于很少有更新的数据结构来说，只在初始化时保护数据。大多数情况下，这种数据结构是
只读的，并且多线程对其并发的读取也是很愉快的，不过一旦数据结构需要更新就会产生竞争。*/

///3.3.2 保护不常更新的数据结构
/*
试想为了将域名解析为其相关IP地址，在缓存中的存放了一张DNS入口表。通常，给定DNS数目
在很长的时间内保持不变。虽然，用户访问不同网站时，新的入口可能会被添加到表中，
但是这些数据可能在其生命周期内保持不变。所以定期检查缓存中入口的有效性就变的十分
重要。但也需要一次更新，也许这次更新只是对一些细节做了改动。

虽然更新频度很低，但也有可能发生，并且当多个线程访问缓存时，这个缓存就需要保护
更新时状态的状态，也是为了确保每个线程读到都是有效数据。

没有使用专用数据结构时，这种方式是符合预期的，并为并发更新和读取进行了特别设计
(更多的例子在第6和第7章中介绍)。这样的更新要求线程独占数据结构的访问权，直到更新
操作完成。当完成更新时，数据结构对于并发多线程的访问又会是安全的。使用std::mutex
来保护数据结构，感觉有些反应过度(因为在没有发生修改时，它将削减并发读取数据的
可能性)。这里需要另一种不同的互斥量，这种互斥量常被称为“读者-写者锁”，因为其允许
两种不同的使用方式：一个“写者”线程独占访问和共享访问，让多个“读者”线程并发访问。

C++17标准库提供了两种非常好的互斥量——std::shared_mutex和std::shared_timed_mutex。
C++14只提供了std::shared_timed_mutex，并且在C++11中并未提供任何互斥量类型。如果
还在用支持C++14标准之前的编译器，可以使用Boost库中的互斥量。std::shared_mutex和
std::shared_timed_mutex的不同点在于，std::shared_timed_mutex支持更多的操作方式
(参考4.3节)，std::shared_mutex有更高的性能优势，但支持的操作较少。

第8章中会看到，这种锁的也不能包治百病，其性能依赖于参与其中的处理器数量，同样也
与读者和写者线程的负载有关。为了确保增加复杂度后还能获得性能收益，目标系统上的
代码性能就很重要。

比起使用std::mutex实例进行同步，不如使用std::shared_mutex来做同步。对于更新操作，
可以使用std::lock_guard<std::shared_mutex>和std::unique_lock<std::shared_mutex>
上锁。作为std::mutex的替代方案，与std::mutex所做的一样，这就能保证更新线程的独占
访问。那些无需修改数据结构的线程，可以使用std::shared_lock<std::shared_mutex>
获取访问权。这种RAII类型模板是在C++14中的新特性，这与使用std::unique_lock一样，
除了多线程可以同时获取同一个std::shared_mutex的共享锁。唯一的限制：当有线程拥有
共享锁时，尝试获取独占锁的线程会被阻塞，直到所有其他线程放弃锁。当任一线程拥有一
个独占锁时，其他线程就无法获得共享锁或独占锁，直到第一个线程放弃其拥有的锁。

如同之前描述的那样，下面的代码清单展示了一个简单的DNS缓存，使用std::map持有缓存
数据，使用std::shared_mutex进行保护。
*/
//代码3.13 使用std::shared_mutex对数据结构进行保护
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
/*代码3.13中，find_entry()使用std::shared_lock<>来保护共享和只读权限①。
这就使得多线程可以同时调用find_entry()，且不会出错。另一方面，
update_or_add_entry()使用std::lock_guard<>实例，当表格需要更新时②，为其提供
独占访问权限。update_or_add_entry()函数调用时，独占锁会阻止其他线程对数据结构
进行修改，并且阻止线程调用find_entry()。*/

///3.3.3 嵌套锁
/*
线程对已经获取的std::mutex(已经上锁)再次上锁是错误的，尝试这样做会导致未定义行为。
在某些情况下，一个线程会尝试在释放一个互斥量前多次获取。因此，C++标准库提供了
std::recursive_mutex类。除了可以在同一线程的单个实例上多次上锁，其他功能与
std::mutex相同。其他线程对互斥量上锁前，当前线程必须释放拥有的所有锁，所以如果你
调用lock()三次，也必须调用unlock()三次。正确使用std::lock_guard<std::recursive_mutex>
和std::unique_lock<std::recursive_mutex>可以帮你处理这些问题。

使用嵌套锁时，要对代码设计进行改动。嵌套锁一般用在可并发访问的类上，所以使用互斥量
保护其成员数据。每个公共成员函数都会对互斥量上锁，然后完成对应的操作后再解锁互斥量。
不过，有时成员函数会调用另一个成员函数，这种情况下，第二个成员函数也会试图锁住互斥量，
这就会导致未定义行为的发生。“变通的”解决方案会将互斥量转为嵌套锁，第二个成员函数
就能成功的进行上锁，并且函数能继续执行。

但是这种方式过于草率和不合理，所以不推荐这样的使用方式。特别是，对应类的不变量通常会被破坏。
这意味着，当不变量被破坏时，第二个成员函数还需要继续执行。一个比较好的方式是，从中
提取出一个函数作为类的私有成员，这个私有成员函数不会对互斥量进行上锁(调用前必须获得锁)。
然后，需要仔细考虑一下，这种情况调用新函数时数据的状态。
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
