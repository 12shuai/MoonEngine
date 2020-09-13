#pragma once
#include<memory>
#include<functional>
#include<queue>
#include<unordered_map>
#include<windows.h>

#define yield_return() Coroutine::get_current_coroutine()->yield()

enum class CoroutineState {
	init,
	running,
	suspend,
	term
};

// Based on: https://github.com/Taoist-Yu/Coroutine_Win.
class Coroutine : public std::enable_shared_from_this<Coroutine> {
public:
	#define MAX_COROUTINE_NUM 32768
	friend class std::shared_ptr<Coroutine>;
	typedef std::function<void(void*)> call_back;
	typedef std::shared_ptr<Coroutine> ptr;

	static std::unordered_map<int, Coroutine::ptr> co_pool;
	const static size_t default_stack_size = 1024 * 1024;

	void yield();
	void resume();
	void close();
	int get_id();
	ptr get_this();
	CoroutineState get_state();
	static Coroutine::ptr create_coroutine(call_back func, void* args, size_t stack_size = 0);
	static ptr get_current_coroutine();
	static int get_coroutine_number();
	static void deleter(Coroutine* co);

private:
	int m_id;
	void* cb_args;
	bool is_destroyed;
	PVOID m_fiber;
	call_back cb_func;
	CoroutineState m_state;

	static Coroutine::ptr main_coroutine;
	static ptr current_coroutine;
	static int coroutine_num;
	static std::queue<Coroutine::ptr> delete_queue;

	Coroutine();
	Coroutine(Coroutine&) = default;
	Coroutine(call_back func, void* args, size_t stack_size);
	~Coroutine() { if (m_id != 0) DeleteFiber(this->m_fiber); }

	void destroy_self();
	static void CALLBACK coroutine_main_func(LPVOID lpFiberParameter);
	static void auto_delete();
};

/*
* NOTE:
��Ĺ��캯��һ����public�ģ�����Ҳ������private�ġ����캯��Ϊ˽�е������������ص㣺
	<1>����ʵ��������Ϊʵ����ʱ���ⲿ�޷��������ڲ���˽�еĹ��캯����
	<2>���ܼ̳У�ͬ<1>��
ʵ��������������ķ����������ж���һ��static��������һ����Ԫ��ר�Ÿ���ʵ����������
Java�еĹ�����������������㡣
*/
