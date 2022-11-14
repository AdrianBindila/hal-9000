#include "test_lp.h"

#include "thread.h"
#include "thread_internal.h"

static volatile long gNumberOfThreads;

STATUS
(__cdecl _ThreadLpTest)(
	IN_OPT PVOID Context
	)
{
	int numberOfChildren = (int)Context;
	int i;

	PTHREAD* childHandles = ExAllocatePoolWithTag(
		PoolAllocateZeroMemory,
		sizeof(THREAD) * numberOfChildren,
		HEAP_THREAD_TAG, 0);

	for (i = 0; i < numberOfChildren; i++)
	{
		PTHREAD thread;
		char thName[MAX_PATH];
		snprintf(thName, MAX_PATH, "ThreadLp-%d",
			_InterlockedIncrement(&gNumberOfThreads));

		STATUS status = ThreadCreate(thName,
			ThreadPriorityDefault,
			_ThreadLpTest,
			numberOfChildren - 1,
			&thread);

		if (!SUCCEEDED(status))
		{
			LOG_FUNC_ERROR("ThreadCreate", status);
		}
		else
		{
			childHandles[i] = thread;
			ThreadCloseHandle(thread);
		}
		for (int j = 0; j < numberOfChildren; j++)//TODO: recheck cause this doesn't look right
		{
			PTHREAD cThread = childHandles[j];
			ThreadWaitForTermination(cThread, cThread->ExitStatus);
			ThreadCloseHandle(thread);
		}
		return status;
	}
}