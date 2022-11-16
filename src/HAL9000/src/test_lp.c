#include "test_lp.h"

#include "thread.h"
#include "thread_internal.h"

static volatile DWORD gNumberOfThreads;

STATUS
(__cdecl _ThreadLpTest)(
	IN_OPT PVOID Context
	)
{
	DWORD numberOfChildren = (DWORD)(QWORD)Context;

	PTHREAD* childHandles = ExAllocatePoolWithTag(
		PoolAllocatePanicIfFail | PoolAllocateZeroMemory,
		sizeof(PTHREAD) * numberOfChildren,
		HEAP_THREAD_TAG, 0);

	STATUS status = STATUS_SUCCESS;
	for (DWORD i = 0; i < numberOfChildren; i++)
	{
		PTHREAD thread;
		char thName[MAX_PATH];
		snprintf(thName, MAX_PATH, "ThreadLp-%d",
			_InterlockedIncrement(&gNumberOfThreads));

		status = ThreadCreate(thName,
			ThreadPriorityDefault,
			_ThreadLpTest,
			(PVOID)(QWORD)(numberOfChildren - 1),
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
	}
	for (DWORD i = 0; i < numberOfChildren; i++)
	{
		STATUS exitStatus;
		PTHREAD cThread = childHandles[i];
		ThreadWaitForTermination(cThread, &exitStatus);
	}
	return status;
}
