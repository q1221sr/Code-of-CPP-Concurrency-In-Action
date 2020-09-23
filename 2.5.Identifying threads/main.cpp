#include <iostream>
#include <thread>

/*
�̱߳�ʶΪstd::thread::id���ͣ�����ͨ�����ַ�ʽ���м�������һ�֣�
����ͨ������std::thread����ĳ�Ա����get_id()��ֱ�ӻ�ȡ�����std::thread����
û�����κ�ִ���߳��������get_id()������std::thread::typeĬ�Ϲ���ֵ�����ֵ��ʾ�����̡߳���
�ڶ��֣���ǰ�߳��е���std::this_thread::get_id()
(�������������<thread>ͷ�ļ���)Ҳ���Ի���̱߳�ʶ��

std::thread::id����������ɵĿ����ͶԱȣ���Ϊ��ʶ�����Ը��á�
������������std::thread::id��ȣ��Ǿ���ͬһ���̣߳����߶������̡߳���
������ȣ���ô�ʹ�����������ͬ�̣߳�����һ�����̣߳���һû���̡߳�

C++�߳̿ⲻ��������ȥ����̱߳�ʶ�Ƿ�һ����std::thread::id���Ͷ����ṩ���൱�ḻ�ĶԱȲ�����
���磬Ϊ��ͬ��ֵ������������ζ�ſ����߿��Խ��䵱��Ϊ�����ļ�ֵ�����򣬻��������Ƚϡ�
��Ĭ��˳��Ƚϲ�ͬ��std::thread::id����a<b��b<cʱ����a<c���ȵȡ���׼��Ҳ�ṩstd::hash<std::thread::id>������
std::thread::idҲ������Ϊ���������ļ�ֵ��

std::thread::idʵ������������߳��Ƿ���Ҫ����һЩ���������磺�����߳����ָ�һ���(����Ŀ2.4)��
���߳̿���Ҫ��һЩ�������̲߳�ͬ�Ĺ��������������߳�ǰ������ͨ��std::this_thread::get_id()�õ��Լ����߳�ID��
ÿ���̶߳�Ҫ���һ�£���ӵ�е��߳�ID�Ƿ����ʼ�̵߳�ID��ͬ��
*/

std::thread::id master_thread;

/**��֤��some_core_part_of_algorithm()��main()����һ���߳�*/
void some_core_part_of_algorithm()
{
  if(std::this_thread::get_id()==master_thread)
  {
    std::cout<<"master_thread_work"<<std::endl;
  }
  std::cout<<"do_common_work"<<std::endl;
}

/*
���⣬��ǰ�̵߳�std::thread::id���洢�����ݽṹ�С�֮������ṹ��Ե�ǰ�̵߳�ID��洢���߳�ID���Աȣ�
�����������ǡ����������ǡ���Ҫ��(permitted/required)��

ͬ������Ϊ�̺߳ͱ��ش洢�����������������߳�ID�������п���Ϊ��ֵ�����磬
�������Դ洢���ƿ���ÿ���̵߳���Ϣ�����ڶ���߳��л�����Ϣ��
*/

int main()
{
    some_core_part_of_algorithm();
    return 0;
}
