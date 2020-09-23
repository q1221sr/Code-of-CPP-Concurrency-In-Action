#include <iostream>
#include <thread>
#include<numeric>
#include<vector>

/*
������ʵ���˲��а��std::accumulate�����뽫���幤����ֳ�С���񣬽���ÿ���߳�ȥ����
��������С���������������̫����̣߳�������ڲ�������Ϊ0ʱ�׳��쳣��
���磬std::thread�޷������̣߳��ͻ��׳��쳣��
*/

template<typename Iterator,typename T>
struct accumulate_block
{
    void operator()(Iterator first,Iterator last,T& result)
    {
        //accumulate������#include<numeric>�У�������������һ�����ۼ���ͣ�
        //��һ�����Զ����������ݵĴ���
        result=std::accumulate(first,last,result);
    }
};

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first,Iterator last,T init)
{
    unsigned long const length=std::distance(first,last);//distanceȷ����ָ��ľ��룬Ҳ����Χ�ڰ�����Ԫ�ظ���

    if(!length) // 1,�������ķ�ΧΪ�գ��ͻ�õ�init��ֵ��
        return init;

    unsigned long const min_per_thread=25;
// 2,�����Χ�ڵ�Ԫ�ض���һ��ʱ����Ҫ�÷�Χ��Ԫ�ص������������߳�(��)����С��������
//�Ӷ�ȷ�������̵߳��������
    unsigned long const max_threads=
        (length+min_per_thread-1)/min_per_thread;

//std::thread::hardware_concurrency()���°�C++�зǳ����ã���᷵�ز����̵߳�������
//���磬���ϵͳ�У�����ֵ������CPU��о������������ֵҲ������һ����ʶ�����޷���ȡʱ����������0��
    unsigned long const hardware_threads=
        std::thread::hardware_concurrency();
    std::cout<<"hardware_threads="<<hardware_threads<<std::endl;

//��Ϊ������Ƶ���л��ή���̵߳����ܣ����Լ����������ֵ��Ӳ��֧���߳�����
//��С��ֵΪ�����̵߳�����
    unsigned long const num_threads=  // 3
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    std::cout<<"num_threads="<<num_threads<<std::endl;

//ÿ���߳��д����Ԫ���������Ƿ�Χ��Ԫ�ص����������̵߳ĸ����ó���
    unsigned long const block_size=length/num_threads; // 4
    std::cout<<"block_size="<<block_size<<std::endl;

    std::vector<T> results(num_threads);//����м���
    std::vector<std::thread> threads(num_threads-1);  // 5,����һ��std::vector<std::thread>����

    Iterator block_start=first;
//��Ϊ������֮ǰ�Ѿ�����һ���߳�(���߳�)�������������߳��������num_threads��1
    for(unsigned long i=0; i < (num_threads-1); ++i)
    {
        Iterator block_end=block_start;
        //ʹ��ѭ���������̣߳�block_end������ָ��ǰ���ĩβ
        std::advance(block_end,block_size);  // 6,advance()����ָ����λ
        threads[i]=std::thread(     // 7������һ�����߳�Ϊ��ǰ���ۼӽ��
                       accumulate_block<Iterator,T>(),
                       block_start,block_end,std::ref(results[i]));
        block_start=block_end;  // 8����������ָ��ǰ���ĩβʱ��������һ����
    }
    accumulate_block<Iterator,T>()(
        block_start,last,results[num_threads-1]); //9�����������̺߳��̻߳ᴦ�����տ�Ľ����
        //��Ϊ֪�����տ�����һ�����������տ����ж��ٸ�Ԫ�ؾ�����ν�ˡ�

    for (auto& entry : threads)
        entry.join();  // 10���ۼ����տ�Ľ���󣬿ɵȴ�std::for_each�����߳�

    return std::accumulate(results.begin(),results.end(),init); // 11�������н�������ۼ�
}

int main()
{
    std::vector<int> data{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
    int ret = 0;
    ret = parallel_accumulate(data.begin(), data.end(), ret);
    std::cout<<ret<<std::endl;
    return 0;
}
