#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
///��4�� ͬ������
/*
������Ҫ����

����future�ĵȴ�
���޶�ʱ���ڵȴ�
ʹ��ͬ�������򻯴���
��һ���У������˽����̼߳䱣���������ݵķ�������Ȼ�����ǲ�����Ҫ�������ݣ������
�������߳̽���ͬ�������磬�ڵ�һ���߳����ǰ���ȴ���һ���߳�ִ����ɡ�ͨ�����̻߳�
�ȴ��ض��¼����������ߵȴ�ĳһ������ɡ��������Ҫ���ڼ�顰������ɡ���ʶ��������
�ŵ����������С��������������Ҫ���߳��н���ͬ����C++��׼���ṩ��һЩ���߿�����ͬ����
��ʽ�ϱ���Ϊ��������(condition variables)��future�����������淶�У�Ϊfuture�����
�ǳ���Ĳ������������¹���������(latches)(����������Դ)��դ��(barriers)һ��ʹ�á�

���½��������ʹ�����������ȴ��¼�������future����������դ�����Լ���μ�ͬ��������*/
///4.1 �ȴ��¼�������
/*����������һ����ҹ�����еĻ��ϣ���ҹ���������ȷ��վ���³��أ���һ�ַ���������Ҫ���ţ�
ÿͣһվ����֪���������Ͳ�������Ҫ�����վ�㣬�����ƣ�롣���⣬���Կ�һ��ʱ��
������һ�»𳵵���Ŀ�ĵص�ʱ�䣬Ȼ����һ�������ʱ������������壬Ȼ���ĵ�˯
�ᡣ�������������Ҳ�ܲ���Ҳû�д����Ҫ�³���վ�㣬���ǵ������ʱ����Ҫ����
��Ľ����ˡ���Ȼ�����ӵĵ��Ҳ���ܻ�û���ˣ���������˯��վ������ķ�ʽ�ǣ�������
�����ֻҪ���𳵵�վ��ʱ�����˻����������ܰ�����Ѿͺ��ˡ�

����߳���ʲô��ϵ�أ���һ���̵߳ȴ���һ���߳����ʱ�����Գ����ļ�鹲�����ݱ�־
(���������������Ļ�����)��ֱ����һ�߳���ɹ���ʱ�������ʶ�������á����������ַ�ʽ
�������̵߳�ִ��ʱ�����ʶ�����ҵ������������������߳̾�û�а취��ȡ�����ͻ�
�����ȴ�����Ϊ�Եȴ��߳���Դ�����ƣ��������������ʱ�谭�Ա�ʶ�����á������ڱ���
����״̬���г���ʻԱ����һ���ϣ���ʻԱ���ò�������ʻ����Ϊ���ɢ������ע����������
����Ҫ������ʱ�䣬���ܵ�վ��ͬ�����ȴ����̻߳�ȴ�������ʱ�䣬Ҳ�����ĸ����ϵͳ
��Դ��

���⣬�ڵȴ��߳��ڼ���϶��ʹ��std::this_thread::sleep_for()���������Եļ�Ъ(���4.3��)��
*/
bool flag;
std::mutex m;

void wait_for_flag()
{
  std::unique_lock<std::mutex> lk(m);
  while(!flag)
  {
    lk.unlock();  // 1 ����������
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 2 ����100ms
    lk.lock();   // 3 ����������
  }
}
/*
ѭ���У�����ǰ�ں����Ի��������н����٣����������߽������ٶԻ��������������������
�߳̾��л����ȡ�������ñ�ʶ��

���ʵ�־ͽ����ܶ࣬���߳�����ʱû���˷�ִ��ʱ�䣬������ȷ����ȷ������ʱ�䡣̫��
�����ߺ�û��һ���������˷�ִ��ʱ�䡣̫��������ʱ�䣬���ܻ�������ȴ�ʱ����á�����
ʱ������Ƚ��ټ������Ӱ�쵽�������Ϊ���ڸ߽������Ϸ�У�����ζ�Ŷ�֡������һ��
ʱ��Ƭ��

������ѡ��(Ҳ������ѡ���)��ʹ��C++��׼���ṩ�Ĺ���ȥ�ȴ��¼��ķ�����ͨ����һ�߳�
�����ȴ��¼��Ļ�����������Ļ��ѷ�ʽ(���磺��ˮ���ϴ��ڶ��������ʱ)�����ֻ��ƾ�
��Ϊ���������������Ӹ�������˵�����������������¼�������������أ�����һ������
�̻߳�ȴ������Ĵ�ɡ���ĳЩ�̱߳���ֹʱ��Ϊ�˻��ѵȴ��߳�(����ȴ��̼߳���ִ��)��
��ֹ�߳̽�����ȴ��ŵ��̹߳㲥��������ɡ�����Ϣ��
*/

///4.1.1 �ȴ��������
/*
C++��׼�����������������ʵ�֣�std::condition_variable��std::condition_variable_any��
������ʵ�ֶ�������<condition_variable>ͷ�ļ��������С����߶���Ҫ�뻥����һ�����
����(��������Ϊ��ͬ��)��ǰ�߽�����std::mutexһ�����������߿��Ժͺ��ʵĻ�����һ��
�������Ӷ�������_any�ĺ�׺����Ϊ std::condition_variable_any����ͨ�ã�����������
��ϵͳ��Դ��ʹ�÷�����и���Ŀ���������ͨ���Ὣstd::condition_variable��Ϊ��ѡ
���͡������������Ҫ��ʱ���Żῼ��std::condition_variable_any��

���ԣ�ʹ��std::condition_variableȥ����֮ǰ�ᵽ�������������������Ҫ����ʱ����λ���
�����е��̣߳����´���չʾ��ʹ���������������̵߳ķ�ʽ��
*/
//����4.1 ʹ��std::condition_variable�������ݵȴ�
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
���ȣ��������������̣߳������߳�֮�������ݽ��д��ݢ١�����׼����ʱ��ʹ��std::lock_guard�������У�
��׼���õ�����ѹ����Т�֮���̻߳�Զ����е�����������������std::condition_variable
��notify_one()��Ա�������Եȴ����߳�(����еȴ��߳�)����֪ͨ�ۡ�

�����һ���߳����ڴ������ݣ��߳����ȶԻ���������(����ʹ��std::unique_lockҪ��
std::lock_guard�ܸ��Ӻ���)��֮������std::condition_variable�ĳ�Ա����wait()��
����һ������һ��Lambda���ʽ(��Ϊ�ȴ���������)��Lambda������C++11��ӵ������ԣ�
������һ������������Ϊ�������ʽ��һ���֣����ҷǳ�������Ϊ��׼������ν�ʡ������У�
�򵥵�Lambda����[]{return !data_queue.empty();}��ȥ���data_queue�Ƿ�Ϊ�գ���data_queue
��Ϊ�գ���˵�������Ѿ�׼�����ˡ���¼A��A.5����Lambda�����������Ϣ��

wait()��ȥ�����Щ����(ͨ��Lambda����)������������(Lambda��������true)ʱ���ء�
�������������(Lambda��������false)��wait()�����������������ҽ��߳�(�������ݵ��߳�)
����������ȴ�״̬����׼�����ݵ��̵߳���notify_one()֪ͨ��������ʱ���������ݵ��߳�
��˯�������ѣ����»�ȡ�������������ٴν���������顣���������������£���wait()
���ز�������������������������ʱ���߳̽��Ի����������������µȴ��������Ϊʲô��
std::unique_lock����ʹ��std::lock_guard��ԭ�򡪡��ȴ��е��̱߳����ڵȴ��ڼ������������
���Ի������ٴ���������std::lock_guardû����ô��������������߳������ڼ䱣��
��ס״̬��׼�����ݵ��߳̽��޷���ס��������Ҳ�޷�������ݵ������С�ͬ�����ȴ��߳�Ҳ
��Զ����֪��������ʱ���㡣

����4.1ʹ���˼򵥵�Lambda�������ڵȴ���(���ڼ����к�ʱ��Ϊ��)����������ĺ�����
�ɵ��ö��󶼿��Դ���wait()����д�ú�����Ϊ�������ʱ����һ����Ҫ����һ��Lambda���ʽ�У�
Ҳ����ֱ�ӽ������������wait()������wait()�Ĺ����У��ڻ���������ʱ�����ܻ�ȥ�����������
���ɴΣ����ṩ���������ĺ�������true�ͻ��������ء����ȴ��߳����»�ȡ�������������������ʱ��
����ֱ����Ӧ��һ���̵߳�֪ͨ��������ν��α����(spurious wakeup)����Ϊ�κ�α���ѵ�����
��Ƶ�ʶ��ǲ�ȷ���ģ����Բ�����ʹ���и����õĺ�����������顣

�����ϣ� std::condition_variable::wait�ǡ�æµ-�ȴ������Ż��������ü򵥵�ѭ��ʵ����
һ����æµ-�ȴ�����
*/

//template<typename Predicate>
//void minimal_wait(std::unique_lock<std::mutex>& lk, Predicate pred){
//  while(!pred()){
//    lk.unlock();
//    lk.lock();
//  }
//}
/*
Ϊwait()׼��һ����С��ʵ�֣�ֻ��Ҫnotify_one()��notify_all()��

std::unique_lock������ԣ����������ڶ�wait()�ĵ��ã����������ڴ���������ݢޡ�
�������ݿ����Ǻ�ʱ�Ĳ��������ҳ�ʱ��������Ǹ��������⡣

ʹ�ö����ڶ���߳���ת������(�����4.1)�ܳ��������úõĻ���ͬ�����������ڶ����ڲ�
��ɣ�����ͬ������������������ֵĸ���Ҳ�ή�͡�������Щ�ô�����Ҫ�Ӵ���4.1����ȡ
��һ��ͨ���̰߳�ȫ�Ķ��С�
*/
///4.1.2 �����̰߳�ȫ����
/*���ͨ�ö���ʱ����Ҫ��ʱ�����룬��Щ������Ҫ��ӵ�����ʵ����ȥ������֮ǰ��3.2.3��
�������̰߳�ȫ��ջ�����Կ�һ��C++��׼���ṩ��ʵ�֣�������С�std::queue<>�����Ľӿ�չʾ���£�

����4.2 std::queue�ӿ�*/
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
���Թ��졢��ֵ�Լ�����������ʣ�������������

���������е�״̬���в�ѯ(empty()��size())
��ѯ�ڶ����еĸ���Ԫ��(front()��back())
�޸Ķ��еĲ���(push(), pop()��emplace())
��3.2.3�е�ջһ����Ҳ�������ӿ��ϵ�������������ˣ���Ҫ��front()��pop()�ϲ���һ���������ã�
����֮ǰ��ջʵ��ʱ�ϲ�top()��pop()һ���������4.1��ͬ���ǣ��������ڶ���߳���
��������ʱ�������߳�ͨ����Ҫ�ȴ����ݵ�ѹ�롣�����ṩpop()�������������֣�
try_pop()��wait_and_pop()��

try_pop() �����ԴӶ����е������ݣ���ʹû��ֵ�ɼ�����Ҳ��ֱ�ӷ��ء�

wait_and_pop()������ȴ���ֵ�ɼ�����ʱ��ŷ��ء�

��ʹ��֮ǰջ�ķ�ʽ��ʵ�ֶ��У��ӿڿ��ܻ�������������

����4.3 �̰߳�ȫ���еĽӿ�*/
//
//#include <memory> // Ϊ��ʹ��std::shared_ptr
//
//template<typename T>
//class threadsafe_queue
//{
//public:
//  threadsafe_queue();
//  threadsafe_queue(const threadsafe_queue&);
//  threadsafe_queue& operator=(
//      const threadsafe_queue&) = delete;  // ������򵥵ĸ�ֵ
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
����֮ǰ��ջ���ü��˺ܶ๹�캯��������ֹ�򵥸�ֵ����Ҫ�ṩ�����汾��try_pop()
��wait_for_pop()����һ�����ص�try_pop()�������ñ����д洢�ż���ֵ��������������
������ֵ��״̬����������һ������ʱ��������true�����򷵻�false(���A.2��)���ڶ���
���آھͲ����ˣ���Ϊ��������ֱ�ӷ��ؼ���ֵ�ģ���û��ֵ�ɼ���ʱ�������������NULL��

��ô�������ˣ���ν�������Щ�ʹ���4.1������أ���֮ǰ�Ĵ�������ȡpush()��
wait_and_pop()�������´�����ʾ��

����4.4 �Ӵ���4.1����ȡpush()��wait_and_pop()
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
/*�̶߳������л��������������������Զ����ı����Ͳ���Ҫ�ˢ٣�����push()����Ҫ�ⲿ
ͬ���ڡ���Ȼ��wait_and_pop()��Ҫ������������ĵȴ��ۡ�

��һ��wait_and_pop()������д�����ͺ����飬ʣ�µĺ�������Ӵ���3.5ʵ�ֵ�ջ��ճ����һ����

����4.5 ʹ�������������̰߳�ȫ����(������)*/

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue
{
private:
  mutable std::mutex mut;  // 1 �����������ǿɱ��
  std::queue<T> data_queue;
  std::condition_variable data_cond;
public:
  threadsafe_queue()
  {}
  threadsafe_queue(threadsafe_queue const& other)
  {
    //������ֵʱ��������ֹ�ڿ���������ֵ�������
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
empty()��һ��const��Ա���������Ҵ��뿽�����캯����other�β���һ��const���á���Ϊ
�����߳̿����з�const���ö��󣬲����ñ��ֳ�Ա���������������б�Ҫ�Ի�����������
����Ϊ��ס�������Ǹ��ɱ���������Ի�������Ա����Ϊmutable�ٲ�����empty()�Ϳ���
���캯���н���������

���������ڶ���̵߳ȴ�ͬһ���¼�ʱҲ�����á����߳������ֽ⹤�����أ�����ֻ��һ��
�߳̿��Զ�֪ͨ������Ӧʱ�������4.1�нṹ��ȫ��ͬ��������׼�����ʱ������
notify_one()���ỽ��һ������wait()���̣߳����������wait()�����ķ���״̬(��Ϊ��
����data_queue�����һ��������)�� ���ﲻ��֤�߳�һ���ᱻ֪ͨ������ʹֻ��һ���ȴ�
�߳��յ�֪ͨ�����������߳�Ҳ�п�����Ϊ�ڴ������ݣ������������֪ͨ��

��һ�ֿ����ǣ��ܶ��̵߳ȴ�ͬһ�¼�������֪ͨ������Ҫ������Ӧ����ᷢ���ڹ�������
��ʼ����ʱ�򣬵������߳�ʹ��ͬһ����ʱ����Ҫ�ȴ����ݱ���ʼ������ȴ��������ݵĸ�
�£����磺�����Գ�ʼ��(periodic reinitialization)����Щ����£��߳�׼��������ʱ��
�ͻ�ͨ��������������notify_all()�����ǵ���notify_one()������˼�壬�����ȫ���߳�
�ڶ�ȥִ��wait()(������ǵȴ��������Ƿ�����)��ԭ��

������Ϊtrueʱ���ȴ��߳�ֻ�ȴ�һ�Σ��Ͳ����ٵȴ����������ˣ������������ڵȴ�һ��
���õ����ݿ�ʱ��һ��������������ͬ��������õ�ѡ��

�����������˽�һ��future���������������Ĳ��㡣
*/

int main()
{
    std::cout << "Hello world!" << std::endl;
    return 0;
}
