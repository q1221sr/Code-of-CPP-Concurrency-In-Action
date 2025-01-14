#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
///第4章 同步操作
/*
本章主要内容

带有future的等待
在限定时间内等待
使用同步操作简化代码
上一章中，我们了解了线程间保护共享数据的方法。当然，我们不仅想要保护数据，还想对
单独的线程进行同步。例如，在第一个线程完成前，等待另一个线程执行完成。通常，线程会
等待特定事件发生，或者等待某一条件达成。这可能需要定期检查“任务完成”标识，或将类似
放到共享数据中。像这种情况就需要在线程中进行同步，C++标准库提供了一些工具可用于同步，
形式上表现为条件变量(condition variables)和future。并发技术规范中，为future添加了
非常多的操作，并可与新工具锁存器(latches)(轻量级锁资源)和栅栏(barriers)一起使用。

本章将讨论如何使用条件变量等待事件，介绍future，锁存器和栅栏，以及如何简化同步操作。*/
///4.1 等待事件或条件
/*假设你正在一辆在夜间运行的火车上，在夜间如何在正确的站点下车呢？有一种方法是整晚都要醒着，
每停一站都能知道，这样就不会错过你要到达的站点，但会很疲倦。另外，可以看一下时间
表，估计一下火车到达目的地的时间，然后在一个稍早的时间点上设置闹铃，然后安心的睡
会。这个方法听起来也很不错，也没有错过你要下车的站点，但是当火车晚点时，就要被过
早的叫醒了。当然，闹钟的电池也可能会没电了，并导致你睡过站。理想的方式是，无论是
早或晚，只要当火车到站的时候，有人或其他东西能把你叫醒就好了。

这和线程有什么关系呢？当一个线程等待另一个线程完成时，可以持续的检查共享数据标志
(用于做保护工作的互斥量)，直到另一线程完成工作时对这个标识进行重置。不过，这种方式
会消耗线程的执行时间检查标识，并且当互斥量上锁后，其他线程就没有办法获取锁，就会
持续等待。因为对等待线程资源的限制，并且在任务完成时阻碍对标识的设置。类似于保持
清醒状态和列车驾驶员聊了一晚上：驾驶员不得不缓慢驾驶，因为你分散了他的注意力，所以
火车需要更长的时间，才能到站。同样，等待的线程会等待更长的时间，也会消耗更多的系统
资源。

另外，在等待线程在检查间隙，使用std::this_thread::sleep_for()进行周期性的间歇(详见4.3节)：
*/
bool flag;
std::mutex m;

void wait_for_flag()
{
  std::unique_lock<std::mutex> lk(m);
  while(!flag)
  {
    lk.unlock();  // 1 解锁互斥量
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 2 休眠100ms
    lk.lock();   // 3 再锁互斥量
  }
}
/*
循环中，休眠前②函数对互斥量进行解锁①，并且在休眠结束后再对互斥量上锁，所以另外的
线程就有机会获取锁并设置标识。

这个实现就进步很多，当线程休眠时没有浪费执行时间，但很难确定正确的休眠时间。太短
的休眠和没有一样，都会浪费执行时间。太长的休眠时间，可能会让任务等待时间过久。休眠
时间过长比较少见，这会影响到程序的行为，在高节奏的游戏中，就意味着丢帧或错过了一个
时间片。

第三个选择(也是优先选择的)，使用C++标准库提供的工具去等待事件的发生。通过另一线程
触发等待事件的机制是最基本的唤醒方式(例如：流水线上存在额外的任务时)，这种机制就
称为“条件变量”。从概念上来说，条件变量会与多个事件或其他条件相关，并且一个或多个
线程会等待条件的达成。当某些线程被终止时，为了唤醒等待线程(允许等待线程继续执行)，
终止线程将会向等待着的线程广播“条件达成”的信息。
*/

///4.1.1 等待条件达成
/*
C++标准库对条件变量有两套实现：std::condition_variable和std::condition_variable_any，
这两个实现都包含在<condition_variable>头文件的声明中。两者都需要与互斥量一起才能
工作(互斥量是为了同步)，前者仅能与std::mutex一起工作，而后者可以和合适的互斥量一起
工作，从而加上了_any的后缀。因为 std::condition_variable_any更加通用，不过在性能
和系统资源的使用方面会有更多的开销，所以通常会将std::condition_variable作为首选
类型。当对灵活性有要求时，才会考虑std::condition_variable_any。

所以，使用std::condition_variable去处理之前提到的情况——当有数据需要处理时，如何唤醒
休眠中的线程？以下代码展示了使用条件变量唤醒线程的方式。
*/
//代码4.1 使用std::condition_variable处理数据等待
//std::mutex mut;
//typedef int data_chunk;
//std::queue<data_chunk> data_queue;  // 1
//std::condition_variable data_cond;
//
//void data_preparation_thread()
//{
//  while(more_data_to_prepare())
//  {
//    data_chunk const data=prepare_data();
//    std::lock_guard<std::mutex> lk(mut);
//    data_queue.push(data);  // 2
//    data_cond.notify_one();  // 3
//  }
//}
//
//void data_processing_thread()
//{
//  while(true)
//  {
//    std::unique_lock<std::mutex> lk(mut);  // 4
//    data_cond.wait(
//         lk,[]{return !data_queue.empty();});  // 5
//    data_chunk data=data_queue.front();
//    data_queue.pop();
//    lk.unlock();  // 6
//    process(data);
//    if(is_last_chunk(data))
//      break;
//  }
//}
/*
首先，队列中有两个线程，两个线程之间会对数据进行传递①。数据准备好时，使用std::lock_guard锁定队列，
将准备好的数据压入队列②之后，线程会对队列中的数据上锁，并调用std::condition_variable
的notify_one()成员函数，对等待的线程(如果有等待线程)进行通知③。

另外的一个线程正在处理数据，线程首先对互斥量上锁(这里使用std::unique_lock要比
std::lock_guard④更加合适)。之后会调用std::condition_variable的成员函数wait()，
传递一个锁和一个Lambda表达式(作为等待的条件⑤)。Lambda函数是C++11添加的新特性，
可以让一个匿名函数作为其他表达式的一部分，并且非常合适作为标准函数的谓词。例子中，
简单的Lambda函数[]{return !data_queue.empty();}会去检查data_queue是否为空，当data_queue
不为空，就说明数据已经准备好了。附录A的A.5节有Lambda函数更多的信息。

wait()会去检查这些条件(通过Lambda函数)，当条件满足(Lambda函数返回true)时返回。
如果条件不满足(Lambda函数返回false)，wait()将解锁互斥量，并且将线程(处理数据的线程)
置于阻塞或等待状态。当准备数据的线程调用notify_one()通知条件变量时，处理数据的线程
从睡眠中苏醒，重新获取互斥锁，并且再次进行条件检查。在条件满足的情况下，从wait()
返回并继续持有锁。当条件不满足时，线程将对互斥量解锁，并重新等待。这就是为什么用
std::unique_lock而不使用std::lock_guard的原因——等待中的线程必须在等待期间解锁互斥量，
并对互斥量再次上锁，而std::lock_guard没有这么灵活。如果互斥量在线程休眠期间保持
锁住状态，准备数据的线程将无法锁住互斥量，也无法添加数据到队列中。同样，等待线程也
永远不会知道条件何时满足。

代码4.1使用了简单的Lambda函数用于等待⑤(用于检查队列何时不为空)，不过任意的函数和
可调用对象都可以传入wait()。当写好函数做为检查条件时，不一定非要放在一个Lambda表达式中，
也可以直接将这个函数传入wait()。调用wait()的过程中，在互斥量锁定时，可能会去检查条件变量
若干次，当提供测试条件的函数返回true就会立即返回。当等待线程重新获取互斥量并检查条件变量时，
并非直接响应另一个线程的通知，就是所谓的伪唤醒(spurious wakeup)。因为任何伪唤醒的数量
和频率都是不确定的，所以不建议使用有副作用的函数做条件检查。

本质上， std::condition_variable::wait是“忙碌-等待”的优化。下面用简单的循环实现了
一个“忙碌-等待”：
*/

//template<typename Predicate>
//void minimal_wait(std::unique_lock<std::mutex>& lk, Predicate pred){
//  while(!pred()){
//    lk.unlock();
//    lk.lock();
//  }
//}
/*
为wait()准备一个最小化实现，只需要notify_one()或notify_all()。

std::unique_lock的灵活性，不仅适用于对wait()的调用，还可以用于待处理的数据⑥。
处理数据可能是耗时的操作，并且长时间持有锁是个糟糕的主意。

使用队列在多个线程中转移数据(如代码4.1)很常见。做得好的话，同步操作可以在队列内部
完成，这样同步问题和条件竞争出现的概率也会降低。鉴于这些好处，需要从代码4.1中提取
出一个通用线程安全的队列。
*/
///4.1.2 构建线程安全队列
/*设计通用队列时，就要花时间想想，哪些操作需要添加到队列实现中去，就如之前在3.2.3节
看到的线程安全的栈。可以看一下C++标准库提供的实现，找找灵感。std::queue<>容器的接口展示如下：

代码4.2 std::queue接口*/
//template <class T, class Container = std::deque<T> >
//class queue {
//public:
//  explicit queue(const Container&);
//  explicit queue(Container&& = Container());
//  template <class Alloc> explicit queue(const Alloc&);
//  template <class Alloc> queue(const Container&, const Alloc&);
//  template <class Alloc> queue(Container&&, const Alloc&);
//  template <class Alloc> queue(queue&&, const Alloc&);
//
//  void swap(queue& q);
//
//  bool empty() const;
//  size_type size() const;
//
//  T& front();
//  const T& front() const;
//  T& back();
//  const T& back() const;
//
//  void push(const T& x);
//  void push(T&& x);
//  void pop();
//  template <class... Args> void emplace(Args&&... args);
//};
/*
忽略构造、赋值以及交换操作，剩下了三组操作：

对整个队列的状态进行查询(empty()和size())
查询在队列中的各个元素(front()和back())
修改队列的操作(push(), pop()和emplace())
和3.2.3中的栈一样，也会遇到接口上的条件竞争。因此，需要将front()和pop()合并成一个函数调用，
就像之前在栈实现时合并top()和pop()一样。与代码4.1不同的是，当队列在多个线程中
传递数据时，接收线程通常需要等待数据的压入。这里提供pop()函数的两个变种：
try_pop()和wait_and_pop()。

try_pop() ，尝试从队列中弹出数据，即使没有值可检索，也会直接返回。

wait_and_pop()，将会等待有值可检索的时候才返回。

当使用之前栈的方式来实现队列，接口可能会是下面这样：

代码4.3 线程安全队列的接口*/
//
//#include <memory> // 为了使用std::shared_ptr
//
//template<typename T>
//class threadsafe_queue
//{
//public:
//  threadsafe_queue();
//  threadsafe_queue(const threadsafe_queue&);
//  threadsafe_queue& operator=(
//      const threadsafe_queue&) = delete;  // 不允许简单的赋值
//
//  void push(T new_value);
//
//  bool try_pop(T& value);  // 1
//  std::shared_ptr<T> try_pop();  // 2
//
//  void wait_and_pop(T& value);
//  std::shared_ptr<T> wait_and_pop();
//
//  bool empty() const;
//};
/*
就像之前的栈，裁剪了很多构造函数，并禁止简单赋值。需要提供两个版本的try_pop()
和wait_for_pop()。第一个重载的try_pop()①在引用变量中存储着检索值，可以用来返回
队列中值的状态。当检索到一个变量时，将返回true，否则返回false(详见A.2节)。第二个
重载②就不行了，因为它是用来直接返回检索值的，当没有值可检索时，这个函数返回NULL。

那么问题来了，如何将以上这些和代码4.1相关联呢？从之前的代码中提取push()和
wait_and_pop()，如以下代码所示。

代码4.4 从代码4.1中提取push()和wait_and_pop()
*/

//#include <queue>
//#include <mutex>
//#include <condition_variable>
//
//template<typename T>
//class threadsafe_queue
//{
//private:
//  std::mutex mut;
//  std::queue<T> data_queue;
//  std::condition_variable data_cond;
//public:
//  void push(T new_value)
//  {
//    std::lock_guard<std::mutex> lk(mut);
//    data_queue.push(new_value);
//    data_cond.notify_one();
//  }
//
//  void wait_and_pop(T& value)
//  {
//    std::unique_lock<std::mutex> lk(mut);
//    data_cond.wait(lk,[this]{return !data_queue.empty();});
//    value=data_queue.front();
//    data_queue.pop();
//  }
//};
//threadsafe_queue<data_chunk> data_queue;  // 1
//
//void data_preparation_thread()
//{
//  while(more_data_to_prepare())
//  {
//    data_chunk const data=prepare_data();
//    data_queue.push(data);  // 2
//  }
//}
//
//void data_processing_thread()
//{
//  while(true)
//  {
//    data_chunk data;
//    data_queue.wait_and_pop(data);  // 3
//    process(data);
//    if(is_last_chunk(data))
//      break;
//  }
//}
/*线程队列中有互斥量和条件变量，所以独立的变量就不需要了①，并且push()不需要外部
同步②。当然，wait_and_pop()还要兼顾条件变量的等待③。

另一个wait_and_pop()的重载写起来就很琐碎，剩下的函数就像从代码3.5实现的栈中粘过来一样。

代码4.5 使用条件变量的线程安全队列(完整版)*/

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue
{
private:
  mutable std::mutex mut;  // 1 互斥量必须是可变的
  std::queue<T> data_queue;
  std::condition_variable data_cond;
public:
  threadsafe_queue()
  {}
  threadsafe_queue(threadsafe_queue const& other)
  {
    //拷贝赋值时上锁，防止在拷贝过程中值发生变更
    std::lock_guard<std::mutex> lk(other.mut);
    data_queue=other.data_queue;
  }

  void push(T new_value)
  {
    std::lock_guard<std::mutex> lk(mut);
    data_queue.push(new_value);
    data_cond.notify_one();
  }

  void wait_and_pop(T& value)
  {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk,[this]{return !data_queue.empty();});
    value=data_queue.front();
    data_queue.pop();
  }

  std::shared_ptr<T> wait_and_pop()
  {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(lk,[this]{return !data_queue.empty();});
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }

  bool try_pop(T& value)
  {
    std::lock_guard<std::mutex> lk(mut);
    if(data_queue.empty())
      return false;
    value=data_queue.front();
    data_queue.pop();
    return true;
  }

  std::shared_ptr<T> try_pop()
  {
    std::lock_guard<std::mutex> lk(mut);
    if(data_queue.empty())
      return std::shared_ptr<T>();
    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
    data_queue.pop();
    return res;
  }

  bool empty() const
  {
    std::lock_guard<std::mutex> lk(mut);
    return data_queue.empty();
  }
};
/*
empty()是一个const成员函数，并且传入拷贝构造函数的other形参是一个const引用。因为
其他线程可能有非const引用对象，并调用变种成员函数，所以这里有必要对互斥量上锁。
又因为锁住互斥量是个可变操作，所以互斥量成员必须为mutable①才能在empty()和拷贝
构造函数中进行上锁。

条件变量在多个线程等待同一个事件时也很有用。当线程用来分解工作负载，并且只有一个
线程可以对通知做出反应时，与代码4.1中结构完全相同。当数据准备完成时，调用
notify_one()将会唤醒一个正在wait()的线程，检查条件和wait()函数的返回状态(因为仅
是向data_queue添加了一个数据项)。 这里不保证线程一定会被通知到，即使只有一个等待
线程收到通知，其他处理线程也有可能因为在处理数据，而忽略了这个通知。

另一种可能是，很多线程等待同一事件。对于通知，都需要做出回应。这会发生在共享数据
初始化的时候，当处理线程使用同一数据时，就要等待数据被初始化，或等待共享数据的更
新，比如：周期性初始化(periodic reinitialization)。这些情况下，线程准备好数据时，
就会通过条件变量调用notify_all()，而非调用notify_one()。顾名思义，这就是全部线程
在都去执行wait()(检查他们等待的条件是否满足)的原因。

当条件为true时，等待线程只等待一次，就不会再等待条件变量了，所以尤其是在等待一组
可用的数据块时，一个条件变量并非同步操作最好的选择。

接下来就来了解一下future，对于条件变量的补足。
*/

int main()
{
    std::cout << "Hello world!" << std::endl;
    return 0;
}
