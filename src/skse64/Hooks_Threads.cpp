#include "Hooks_Threads.h"
#include "skse64_common/SafeWrite.h"
#include "skse64_common/Utilities.h"
#include "skse64_common/BranchTrampoline.h"
#include "GameThreads.h"

#include "common/ICriticalSection.h"
#include <queue>

ICriticalSection			s_taskQueueLock;
std::queue<TaskDelegate*>	s_tasks;

void BSTaskPool::ProcessTasks()
{
	CALL_MEMBER_FN(this, ProcessTaskQueue_HookTarget)();

	s_taskQueueLock.Enter();
	while (!s_tasks.empty())
	{
		TaskDelegate * cmd = s_tasks.front();
		s_tasks.pop();

		cmd->Run();
		cmd->Dispose();
	}
	s_taskQueueLock.Leave();
}

void TaskInterface::AddTask(TaskDelegate * cmd)
{
	s_taskQueueLock.Enter();
	s_tasks.push(cmd);
	s_taskQueueLock.Leave();
}

void Hooks_Threads_Init(void)
{
	
}

RelocAddr <uintptr_t> ProcessTasks_HookTarget_Enter(0x005BAB10 + 0x75E);
RelocAddr <uintptr_t> ProcessTasks_HookTarget_Enter2(0x005BC100 + 0x1C);

void Hooks_Threads_Commit(void)
{
	g_branchTrampoline.Write5Call(ProcessTasks_HookTarget_Enter, GetFnAddr(&BSTaskPool::ProcessTasks));
	g_branchTrampoline.Write5Call(ProcessTasks_HookTarget_Enter2, GetFnAddr(&BSTaskPool::ProcessTasks));
}
