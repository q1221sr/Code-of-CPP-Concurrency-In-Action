#include <future>

///ʹ��future�ȴ������¼�
/*
������Ҫ�˷ɻ�ȥ����ȼ٣������������������ֵǻ������󣬻���Ҫ�ȴ������㲥֪ͨ�ǻ���
���ʱ���ڣ�����ܻ��ں����������һЩ��������ʱ�䣬���磺���飬������������һ�����ȡ�
����������ڵȴ�һ�����飺�����㲥֪ͨ�ǻ���

C++��׼�⽫�����¼���Ϊfuture�����߳���Ҫ�ȴ��ض��¼�ʱ��ĳ�̶ֳ�����˵����Ҫ֪��
�����Ľ����֮���̻߳�������(�϶̵�����)�ĵȴ������¼��Ƿ񴥷�(�����Ϣ��)��
����ڼ�Ҳ��ִ����������(Ʒ������Ŀ���)�����⣬�ȴ������ڼ�Ҳ������ִ�����������
ֱ����Ӧ�����񴥷�������ȴ�future��״̬���Ϊ����״̬��future�����Ǻ��������(����
���ǻ��ڱ��)��Ҳ���ܲ��ǡ����¼�����ʱ(״̬Ϊ����)�����future�Ͳ��������ˡ�

C++��׼����������future��������<future>ͷ�ļ���: unique future(std::future<>)��
shared futures(std::shared_future<>)����std::unique_ptr��std::shared_ptr�ǳ����ơ�std::futureֻ����ָ���¼����������std::shared_future���ܹ�������¼������ߵ�ʵ���У�����ʵ������ͬʱ��Ϊ����״̬�����ҿ��Է������¼���ص����ݡ����ֹ�����ģ���йأ�����std::unique_ptr ��std::shared_ptr��ģ�����������ص��������͡��������޹ش��ģ�����ʹ��std::future<void>��std::shared_future<void>���ػ�ģ�塣��Ȼ�����������߳�ͨѶ����future���������ṩͬ�����ʡ�������߳���Ҫ����һ������future����ʱ������ʹ�û�����������ͬ�����ƽ��б�����������������̶߳�һ��std::shared_future<>�������з��ʣ���ʹͬһ���첽�����Ҳ����Ҫͬ��future��

���м����淶��������ģ������std::experimental�����ռ��н�������չ��
std::experimental::future<>��std::experimental::shared_future<> ����������ռ���Ϊ
�˽�����std�����ռ��е�ģ����������֣�ʵ�������ռ���Ϊ������ģ��������˸���Ĺ��ܡ�
������std::experimental�е���������������޹�(��ϣ������Ҳ���нϸ�������ʵ��)����Ҫ
ǿ��������������ռ��ṩ�Ķ����Ǳ�׼��ͺ�������������ռ�����ͺ������﷨�����壬
�ܿ���������C++��׼(Ҳ����std�����ռ�)��������ͬ�������Ҫʹ�������������Ե�ģ���࣬
��Ҫ����<experimental/future>ͷ�ļ���

��򵥵��¼��������ں�̨���еļ����������2�����Ѿ������std::thread ִ�е�����
�����з���ֵ���������������ʹ��future���н����
*/
///4.2.1 ��̨����ķ���ֵ
/*������һ����Ҫ��ʱ������㣬��Ҫ�����һ����Чֵ��������������Ҫ���ֵ�������
�������߳���ִ��������㣬����Ҫ����Ľ������std::thread�����ṩֱ�ӽ��շ���ֵ��
���ơ��������Ҫstd::async����ģ��(Ҳ����ͷ�ļ�<future>)��

�����ż���������ʱ������ʹ��std::async����һ���첽������std::thread����ȴ���
��ʽ��ͬ��std::async�᷵��һ��std::future�����������������ռ�������Ľ����
����Ҫ���ֵʱ��ֻ��Ҫ������������get()��Ա�������ͻ������߳�ֱ��futureΪ����
Ϊֹ�������ؼ�������

����4.6 std::future���첽�����л�ȡ����ֵ*/
/*
#include <future>
#include <iostream>

int find_the_answer_to_ltuae();
void do_other_stuff();

int main()
{
  std::future<int> the_answer=std::async(find_the_answer_to_ltuae);
  do_other_stuff();
  std::cout<<"The answer is "<<the_answer.get()<<std::endl;
}
*/
/*��std::thread��ʽһ����std::async����ͨ����Ӷ���ĵ��ò������������ݶ����
��������һ��������ָ���Ա������ָ�룬�ڶ��������ṩ���������Ա��ľ������(��ͨ��
ָ�룬Ҳ���԰�װ��std::ref��)��ʣ��Ĳ�������Ϊ�����Ĳ������롣���򣬵ڶ��������
�Ĳ�������Ϊ�����Ĳ���������Ϊָ���ɵ��ö���ĵ�һ����������std::threadһ����
������Ϊ��ֵʱ������������ʹ���ƶ��ķ�ʽת��ԭʼ���ݣ��Ϳ���ʹ�á�ֻ�ƶ���������Ϊ
��������Ͳ�����

����4.7 ʹ��std::async�������ݲ���*/
/*
#include <string>
#include <future>
struct X
{
  void foo(int,std::string const&);
  std::string bar(std::string const&);
};
X x;
auto f1=std::async(&X::foo,&x,42,"hello");  // ����p->foo(42, "hello")��p��ָ��x��ָ��
auto f2=std::async(&X::bar,x,"goodbye");  // ����tmpx.bar("goodbye")�� tmpx��x�Ŀ�������
struct Y
{
  double operator()(double);
};
Y y;
auto f3=std::async(Y(),3.141);  // ����tmpy(3.141)��tmpyͨ��Y���ƶ����캯���õ�
auto f4=std::async(std::ref(y),2.718);  // ����y(2.718)
X baz(X&);
std::async(baz,std::ref(x));  // ����baz(x)
class move_only
{
public:
  move_only();
  move_only(move_only&&)
  move_only(move_only const&) = delete;
  move_only& operator=(move_only&&);
  move_only& operator=(move_only const&) = delete;

  void operator()();
};
auto f5=std::async(move_only());  // ����tmp()��tmp��ͨ��std::move(move_only())����õ�
*/
/*
future�ĵȴ�ȡ����std::async�Ƿ�����һ���̣߳����Ƿ��������ڽ���ͬ����
���������£�Ҳ�����ں�������֮ǰ��std::async����һ��������������������������
std::launch����������std::launch::defered���������������ӳٵ�wait()��get()��������ʱ
��ִ�У�std::launch::async�������������������ڵĶ����߳���ִ�У�
std::launch::deferred | std::launch::async����ʵ�ֿ���ѡ�������ַ�ʽ��һ�֡����
һ��ѡ����Ĭ�ϵģ������������ӳ٣��Ϳ��ܲ����������ˡ�������ʾ��*/
/*
auto f6=std::async(std::launch::async,Y(),1.2);  // �����߳���ִ��
auto f7=std::async(std::launch::deferred,baz,std::ref(x));  // ��wait()��get()����ʱִ��
auto f8=std::async(
              std::launch::deferred | std::launch::async,
              baz,std::ref(x));  // ʵ��ѡ��ִ�з�ʽ
auto f9=std::async(baz,std::ref(x));
f7.wait();  //  �����ӳٺ���
*/
/*���µĺ���С�ں͵�8���У����ٴο�����γ���ʹ��std::async�Ὣ�㷨�ָ���������У�
����������ܲ����ˡ��������ⲻ����std::future������ʵ���������Ψһ��ʽ��Ҳ���Խ�
�����װ��std::packaged_task<>�У���ͨ����д����ķ�ʽ��ʹ��std::promise<>ģ����ʽ
����ֵ����std::promise<>��ȣ�std::packaged_task<>���и��ߵĳ����������Ǵ�
���߳���ģ��˵��*/

///4.2.2 future���������
/*std::packaged_task<>�Ὣfuture�뺯����ɵ��ö�����а󶨡�������std::packaged_task<>����ʱ��
�ͻ������غ�����ɵ��ö��󣬵�future״̬Ϊ����ʱ����洢����ֵ����������ڹ���
�̳߳�(�ɼ���9��)����������Ĺ����У����磺�����������߳��������������񣬻�����
����������һ������ĺ�̨�߳��ϡ������Ƚϴ�Ĳ������ֽ�Ϊ������������ʱ��ÿ��������
�����԰�����std::packaged_task<>ʵ���У�֮��ʵ�����ݵ�������������̳߳��С�
������ϸ�ڽ��г��󣬵�����������std::packaged_task<>ʵ�������Ǵ������ĺ�����

std::packaged_task<>��ģ�������һ������ǩ��������void()����һ��û�в���Ҳû�з���ֵ�ĺ���
����int(std::string&, double*)������һ����const���õ�std::string������һ��ָ��double����
��ָ����������ҷ���������int������std::packaged_task<>ʵ��ʱ���ͱ��봫�뺯����
�ɵ��ö������������ɵ��õĶ�����Ҫ�ܽ���ָ���Ĳ����ͷ���(��ת��Ϊָ���������͵�)ֵ��
���Ϳ��Բ���ȫƥ�䣬��Ϊ�������Ϳ�����ʽת����������int���Ͳ����ͷ���float���͵�
������������std::packaged_task<double(double)>ʵ����

����ǩ���ķ������Ϳ���������ʶ��get_future()���ص�std::future<>�����ͣ�������ǩ��
�Ĳ����б�������ָ��packaged_task�ĺ������ò����������磬ģ��ƫ�ػ�
std::packaged_task<std::string(std::vector<char>*,int)>��������Ĵ�����ʹ�õ���

����4.8 std::packaged_task<>��ƫ�ػ�*/
/*
template<>
class packaged_task<std::string(std::vector<char>*,int)>
{
public:
  template<typename Callable>
  explicit packaged_task(Callable&& f);
  std::future<std::string> get_future();
  void operator()(std::vector<char>*,int);
};
*/
/*std::packaged_task�Ǹ��ɵ��ö��󣬿��Է�װ��std::function�����У��Ӷ���Ϊ�̺߳���
���ݵ�std::thread�����У�����Ϊ�ɵ��ö��󴫵ݵ���һ�������л�ֱ�ӵ��á���std::packaged_task
��Ϊ��������ʱ��ʵ�ν��ɺ������ò������������ײ㺯�������ҷ���ֵ��Ϊ�첽����洢
��std::future�У����ҿ�ͨ��get_future()��ȡ����˿�����std::packaged_task������
���д��������ʱ��ȡ��future�����첽������Ҫ����ֵʱ�����Եȴ�future״̬��Ϊ����������
*/
///�̼߳䴫������

/*�ܶ�ͼ�μܹ���Ҫ�ض����߳�ȥ���½��棬���Ե��̶߳Խ������ʱ����Ҫ����һ����Ϣ
����ȷ���̣߳�����Ӧ���߳�����������¡�std::packaged_task�ṩ�����ֹ��ܣ��Ҳ���Ҫ
����һ���Զ�����Ϣ��ͼ�ν����̡߳�

����4.9 ʹ��std::packaged_taskִ��һ��ͼ�ν����߳�*/
/*
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex m;
std::deque<std::packaged_task<void()> > tasks;

bool gui_shutdown_message_received();
void get_and_process_gui_message();

void gui_thread()  // 1
{
  while(!gui_shutdown_message_received())  // 2
  {
    get_and_process_gui_message();  // 3
    std::packaged_task<void()> task;
    {
      std::lock_guard<std::mutex> lk(m);
      if(tasks.empty())  // 4
        continue;
      task=std::move(tasks.front());  // 5
      tasks.pop_front();
    }
    task();  // 6
  }
}

std::thread gui_bg_thread(gui_thread);

template<typename Func>
std::future<void> post_task_for_gui_thread(Func f)
{
  std::packaged_task<void()> task(f);  // 7
  std::future<void> res=task.get_future();  // 8
  std::lock_guard<std::mutex> lk(m);
  tasks.push_back(std::move(task));  // 9
  return res; // 10
}
*/

/*����ʮ�ּ򵥣�ͼ�ν����̢߳�ѭ��ֱ���յ�һ���ر�ͼ�ν������Ϣ��رս���ڡ�
�رս���ǰ��������ѯ������Ϣ����ۣ����磺�û������ִ���ڶ����е����񡣵�������
û�������ʱ��ѭ�����������������ڶ�������ȡ��һ������ݣ��ͷŶ����ϵ���������ִ��
����ޡ�����future��������أ�������ִ����ʱ����״̬����Ϊ����������

����������У��ṩ�ĺ����߿����ṩһ������õ�����ͨ�������������get_future()
��Ա������ȡfuture���󣬲��������������б��֮ǰ��future�����ص��ú����⡣

������ʹ��std::packaged_task<void()>�����������а�����һ���޲����޷���ֵ�ĺ���
��ɵ��ö���(�������������з���ֵʱ������ֵ�ᱻ����)�����������򵥵�����
std::packaged_taskҲ�������ڸ��ӵ��������ͨ��ָ����ͬ�ĺ���ǩ����Ϊģ�����������
���Ըı��䷵������(��˸����͵����ݻ����������ص�״̬��)��Ҳ���Ըı亯��������
�Ĳ������͡�������ӿ��Լ򵥵���չ����������������ͼ�ν����߳��ϣ����ҽ��ܴ��Σ�
������ͨ��std::future��ȡ����ֵ��

��Щ��������Ϊ�򵥵ĺ�������������𣿻��У�����Ľ���ܴӺܶ�ط��õ�����Щ��
�����ʹ�õ����ַ�������future�������ʹ��std::promise��ֵ������ʾ���á�*/


///4.2.3 ʹ��std::promises
/*����Ҫ����ܶ���������ʱ����ʹ�ò�ͬ�̳߳�������ÿ���ӿڣ���ʹ���羡����ͨ��
���ҵ��ǣ������������������������ַ�ʽ���Խ��Խ�����ʡ���Ϊ�������̻߳����Ĵ���
��ϵͳ��Դ�����п�������߳�������Ƶ���л�(���߳���������Ӳ���ɽ��ܵĲ�����ʱ)��
�ⶼ���������Ӱ�졣��˵����ӣ��̻߳Ὣϵͳ��Դ���Ĵ�����ϵͳ���������������
��ļ�����ͨ�������̴߳����������ӣ�ÿ���߳�ͬʱ���������ӣ�����Ҫ�������
�������ӵ�Ӧ�ö��ԣ�����һ�ֱȽ��ձ��������

���̴߳����������¼������Բ�ͬ�Ķ˿����ӵ����ݰ�������������ʽ���д���
ͬ���ģ����ݰ�Ҳ��������ķ�ʽ������С��ܶ�����£�һЩӦ�ò��ǵȴ����ݳɹ���
���ͣ����ǵȴ�(�µ�)ָ������ӿ����ݵĽ��ճɹ���

std::promise<T>�ṩ�趨ֵ�ķ�ʽ(����ΪT)��������ͻ�ͺ��濴����std::future<T>
�����������std::promise/std::future���ṩһ�ֻ��ƣ�future���������ȴ��̣߳��ṩ
���ݵ��߳̿���ʹ��promise�����ֵ�������ã�����future��״̬��Ϊ����������

����ͨ��������std::promise��get_future()��Ա��������ȡ��֮��ص�std::future����
��std::packaged_task���÷����ơ���promise�������(ʹ��set_value()��Ա����)ʱ����
Ӧ��future״̬�ͱ�Ϊ�������������ҿ����ڼ����Ѵ洢��ֵ��������ֵ֮ǰ����std::promise��
����洢һ���쳣����4.2.4���У�����ϸ�����쳣����δ��͵��̵߳ġ�

����4.10���ǵ��̴߳����ӿڵ�ʵ�֣���������У�����ʹ��һ��std::promise<bool>
/std::future<bool>�ҳ������ɹ������ݿ飬��future��ص�ֻ�Ǽ򵥵ġ��ɹ�/ʧ�ܡ���ʶ��
���ڴ��������future��ص����ݾ������ݰ�����Ч���ء�

����4.10 ʹ��promise������̶߳���������*/
/*
#include <future>

void process_connections(connection_set& connections)
{
  while(!done(connections))  // 1
  {
    for(connection_iterator  // 2
            connection=connections.begin(),end=connections.end();
          connection!=end;
          ++connection)
    {
      if(connection->has_incoming_data())  // 3
      {
        data_packet data=connection->incoming();
        std::promise<payload_type>& p=
            connection->get_promise(data.id);  // 4
        p.set_value(data.payload);
      }
      if(connection->has_outgoing_data())  // 5
      {
        outgoing_packet data=
            connection->top_of_outgoing_queue();
        connection->send(data.payload);
        data.promise.set_value(true);  // 6
      }
    }
  }
}
*/

/*process_connections()��(ֱ��done()����true��Ϊֹ)ÿһ��ѭ�����������εļ��ÿ��
���Ӣڣ������Ƿ������ݢۻ����ڷ�������ӵĴ������ݢݡ������������ݰ��Ǿ���ID����Ч����
��(��ʵ�ʵ���������)��һ��IDӳ�䵽һ��std::promise(����������������н��е�����
����)�ܣ�����ֵ���ڰ�����Ч�����С����������ڴ��������м������ӽӿ�ֱ�ӷ��ͳ�ȥ��
��������ɣ�����������ص�promise����Ϊtrue������������ɹ��ޡ��Ƿ���ӳ�䵽ʵ��
����Э���ϣ�ȡ��������Э�顣

����Ĵ��벻����쳣��һ�й�������ܺõ�ִ�У�����㣳�����ʱ��������أ���ʱ��
���Ҳ�����������ʱ�������ϣ�����ʱ�����ݿ�����������Ҫĳ�������Ľ��ʱ����
��Ҫ�ڶ�Ӧ���߳���ִ�������������Ϊ�������ͨ���쳣��������󡣲���������ʹ��
std::packaged_task��std::promise����һЩ����Ҫ�����ơ���ˣ�C++��׼���ṩ��һ��
����������������쳣�ķ��������������쳣�洢Ϊ��ؽ����һ���֡�
*/

///4.2.4 ���쳣����future��
/*��������Ĵ���Σ�˼��һ�£����㴫��-1��square_root()��ʱ�������׳�һ���쳣��
���������õ����߿�������쳣��*/
/*
double square_root(double x)
{
  if(x<0)
  {
    throw std::out_of_range(��x<0��);
  }
  return sqrt(x);
}
double y=square_root(-1);
//�������square_root()�������ǵ�ǰ�̣߳������ø�Ϊ�첽���ã�
std::future<double> f=std::async(square_root,-1);
double y=f.get();
*/

/*��y��ú������õĽ�����̵߳���f.get()ʱ�������ٿ����쳣�ˡ�

������Ϊstd::async��һ����ʱ���������׳�һ���쳣ʱ������쳣�ͻ�洢��future�У�
֮��future��״̬��Ϊ����������֮�����get()���׳��Ѵ洢���쳣(ע�⣺��׼����û��
ָ�������׳�������쳣��ԭʼ���쳣���󣬻���һ����������ͬ�ı������Ϳ⽫������
����������ͬ��ѡ��)�������������std::packaged_task������󣬵��������ʱ��ͬ����
����Ҳ�ᷢ������������׳�һ���쳣������쳣���洢��future�У���get()����ʱ���ٴ�
�׳���

��Ȼ��ͨ����������ʽ���ã�std::promiseҲ���ṩͬ���Ĺ��ܡ�����������쳣������ֵʱ��
����Ҫ����set_exception()��Ա����������set_value()����ͨ��������һ��catch���У�
����Ϊ�㷨��һ���֡�Ϊ�˲����쳣������ʹ���쳣���promise��*/

/*
extern std::promise<double> some_promise;
try
{
  some_promise.set_value(calculate_value());
}
catch(...)
{
  some_promise.set_exception(std::current_exception());
//��������ʹ��try/catch��������������쳣������֪����Ӧ������ʹ�á�������Ϊ����ʵ��
//�򵥣����Ǹ��������ṩ�˼�����Ż��ռ䡣
//  some_promise.set_exception(std::copy_exception(std::logic_error("foo ")));
}
*/
/*����ʹ��std::current_exception()�������׳����쳣������std::copy_exception()��Ϊ
���������std::copy_exception()��ֱ�Ӵ洢�µ��쳣�����׳���*/

/*��һ����future�д洢�쳣�ķ�ʽ����û�е���promise�ϵ��κ����ú���ǰ�������ڵ���
��װ�õ�����ʱ��������std::promise��std::packaged_task��ص�future�����κ����
�£���future��״̬�����ǡ�������ʱ������std::promise��std::packaged_task������������
����洢һ����std::future_errc::broken_promise����״̬��ص�std::future_error�쳣��
ͨ������һ��future�����Թ���һ��promiseΪ���ṩֵ���쳣��Ҳ����ͨ������ֵ���쳣Դ��
ȥΥ��promise����������£�������û����future�д洢�κζ������߳̿��ܻ���Զ�ĵ���ȥ��

���ڣ������ж�����std::future������std::futureҲ�о����ԡ��ܶ��߳��ڵȴ���ʱ��
ֻ��һ���߳��ܻ�ȡ�����������̵߳ȴ���ͬ�¼��Ľ��ʱ������Ҫʹ��std::shared_future��
���std::future�ˡ�*/

///4.2.5 ����̵߳ĵȴ�

/*��Ȼstd::future���Դ����������̼߳�����ת�Ƶ�ͬ�������ǵ���ĳһ���� std::future����
�ĳ�Ա�������ͻ�������̵߳����ݺ������̵߳����ݲ�ͬ�������߳���û�ж���ͬ��������£�
���ʶ���std::future����ʱ���ͻ������ݾ�����δ������Ϊ����Ϊstd::future����ͬ�������
����ͨ������get()������һ���ԵĻ�ȡ���ݣ�����ò������ʱ�ĺ������塣

������д���û�취�ö���̵߳ȴ�ͬһ���¼���std::shared_future���԰�����������⡣
��Ϊstd::future��ֻ�ƶ��ģ�����������Ȩ�����ڲ�ͬ��ʵ���л��ഫ�ݣ���ֻ��һ��ʵ��
���Ի���ض���ͬ���������std::shared_futureʵ���ǿɿ����ģ����Զ�������������
ͬһ��������ֵ�Ľ����

ÿһ��std::shared_future�Ķ��������ϣ���Ա�������÷��صĽ�����ǲ�ͬ���ģ�����Ϊ
���ڶ���̷߳���һ����������ʱ�������ݾ���������ʹ�������Է��ʽ��б���������ʹ��
�İ취��Ϊ�����ֻ��һ����������������������ÿ���̶߳�ӵ���Լ���Ӧ�Ŀ�������
��������ÿ���̶߳�ͨ���Լ�ӵ�е�std::shared_future�����ȡ�������ô����̷߳���
����ͬ��������ǰ�ȫ�ġ�
���ܻ�ʹ��std::shared_future�ĳ��������磺ʵ�������ڸ��ӵĵ��ӱ��Ĳ���ִ�У�
ÿһ����Ԫ����Ψһ��ֵ�������ֵ������������Ԫ���е�����ͨ����ʽ����õ�����ʽ����
�õ��Ľ��������������Ԫ��Ȼ�����ʹ��std::shared_future�������õ�һ����Ԫ���
���ݡ���ÿ����Ԫ���ڵ����й�ʽ����ִ�к�������������ķ�ʽ��ɹ�������������
�����м�����Ҫ����������Ԫ���ֵʱ�ͻ�������ֱ��������Ԫ�������׼�������������
��ϵͳ�����̶���ʹ��Ӳ��������

std::shared_future��ʵ��ͬ��std::futureʵ����״̬����std::future����û��������
������ͬ��״̬����Ȩ����ô����Ȩ����ʹ��std::move������Ȩ���ݵ�std::shared_future��
��Ĭ�Ϲ��캯�����£�
*/
std::promise<int> p;
std::future<int> f(p.get_future());
assert(f.valid());  // 1 ����ֵ f �ǺϷ���
std::shared_future<int> sf(std::move(f));
assert(!f.valid());  // 2 ����ֵ f �����ǲ��Ϸ���
assert(sf.valid());  // 3 sf �����ǺϷ���

/*����ֵf��ʼ�ǺϷ��Ģ٣���Ϊ���õ���promise p��ͬ��״̬��������ת��sf��״̬��
f�Ͳ��Ϸ��ˢڣ���sf���ǺϷ����ˢۡ�

���������ƶ�����һ����ת������Ȩ�Ƕ���ֵ����ʽ���������Կ���ͨ��std::promise����
�ĳ�Ա����get_future()�ķ���ֵ��ֱ�ӹ���һ��std::shared_future�������磺*/

std::promise<std::string> p;
std::shared_future<std::string> sf(p.get_future());  // 1 ��ʽת������Ȩ

/*ת������Ȩ����ʽ�ģ�����ֵ����std::shared_future<>���õ�std::future<std::string>
���͵�ʵ���١�

std::future���������ԣ��ɴٽ�std::shared_future��ʹ�ã����������Զ��Ķ����ͽ���
�ƶϣ��Ӷ���ʼ�������͵ı���(�����¼A��A.6��)��std::future��һ��share()��Ա������
�����������µ�std::shared_future �����ҿ���ֱ��ת��future������Ȩ������Ҳ���ܱ���
�ܶ����ͣ�����ʹ�ô��������޸ģ�*/
std::promise< std::map< SomeIndexType, SomeDataType, SomeComparator,
     SomeAllocator>::iterator> p;
auto sf=p.get_future().share();
/*��������У�sf�������Ƶ�Ϊstd::shared_future<std::map<SomeIndexType, SomeDataType, SomeComparator, SomeAllocator>::iterator>��
����ĳ������Ƚ���������������Ķ���ֻ��Ҫ��promise�����ͽ����޸ļ��ɡ�future��
���ͻ��Զ���promise���޸Ľ���ƥ�䡣

��ʱ��Ҫ�޶��ȴ��¼���ʱ�䣬��������Ϊʱ������Ӳ�Թ涨(һ��ָ���Ĵ�����Ҫ��ĳ��
ʱ�������)��������Ϊ���¼�û�кܿ�Ĵ����������й�����Ҫ�ض��߳�����ɣ�Ϊ�˴���
�����������Ҫ�ȴ������ܶԳ�ʱ����ָ����
*/

