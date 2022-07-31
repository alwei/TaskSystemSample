// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskActor.h"

#include "Tasks/Task.h"

using namespace UE::Tasks;

void CalcFunc()
{
	UE_LOG(LogTemp, Log, TEXT("CalcFunc Begin"));
	for (int i = 0; i < 1000000; i++)
	{
		FString s(TEXT("CalcFunc"));
		s = s + s;
	}
	UE_LOG(LogTemp, Log, TEXT("CalcFunc Completed"));
}

ATaskActor::ATaskActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATaskActor::BeginPlay()
{
	Super::BeginPlay();

	// タスク処理開始
	TaskLaunch();
	TaskWait();
	TaskBusyWait();
	TaskGetRusult();
	TaskPrerequisites();
	TaskNested();
	TaskPipe();
	TaskEvent();
}

void ATaskActor::TaskLaunch()
{
	// タスクをラムダ関数で起動
	Launch(TEXT("Task Launch"), []
		{
			UE_LOG(LogTemp, Log, TEXT("Task Launch Begin"));
		}
	);
}

void ATaskActor::TaskWait()
{
	// タスク起動後、タスクが完了するまでスレッドをブロック
	FTask Task = Launch(TEXT("Task Wait"), &CalcFunc);

	UE_LOG(LogTemp, Log, TEXT("Task Wait Begin"));
	Task.Wait();
	UE_LOG(LogTemp, Log, TEXT("Task Wait End"));
}

void ATaskActor::TaskBusyWait()
{
	// 先にタスクAを起動する
	FTask TaskA = Launch(TEXT("TaskA Busy Wait"), []
		{
			FPlatformProcess::Sleep(1.0f);
			UE_LOG(LogTemp, Log, TEXT("TaskA End"));
		}
	);

	UE_LOG(LogTemp, Log, TEXT("Task Busy Wait Begin"));

	// タスクBはタスクAより先に完了する
	FTask TaskB = Launch(TEXT("TaskB Busy Wait"), []
		{
			FPlatformProcess::Sleep(0.5f);
			UE_LOG(LogTemp, Log, TEXT("TaskB End"));
		}
	);

	// タスクAをBusyWaitすると、処理が完了するまで待機する
	// その間もタスクBは処理されるので、先にタスクBが完了し、先に進む
	TaskA.BusyWait();

	UE_LOG(LogTemp, Log, TEXT("Task Busy Wait End"));
}

void ATaskActor::TaskGetRusult()
{
	// タスクでbool値を返す
	TTask<bool> BoolTask = Launch(TEXT("Task Return Bool"), []
		{
			return true;
		}
	);

	// GetRusultで中身を確認する
	bool bResult = BoolTask.GetResult();
	UE_LOG(LogTemp, Log, TEXT("BoolTask Result = %s"), bResult ? TEXT("true") : TEXT("false"));
}

void ATaskActor::TaskPrerequisites()
{
	// タスクAを起動
	FTask TaskA = Launch(TEXT("Task Prereqs TaskA"), []
		{
			FPlatformProcess::Sleep(1.0f);
			UE_LOG(LogTemp, Log, TEXT("TaskA End"));
		}
	);
	
	// タスクBとタスクCはタスクAが完了するまでは起動しない
	FTask TaskB = Launch(TEXT("Task Prereqs TaskB"), [] {
			FPlatformProcess::Sleep(0.2f);
			UE_LOG(LogTemp, Log, TEXT("TaskB End"));
		}, TaskA
	);
	FTask TaskC = Launch(TEXT("Task Prereqs TaskC"), []
		{
			FPlatformProcess::Sleep(0.5f);
			UE_LOG(LogTemp, Log, TEXT("TaskC End"));
		}, TaskA
	);

	// タスクDはタスクBとタスクCが完了するまでは起動しない
	FTask TaskD = Launch(TEXT("Task Prereqs TaskD"), []
		{
			UE_LOG(LogTemp, Log, TEXT("TaskD End"));
		}, Prerequisites(TaskB, TaskC)
	);

	TaskD.Wait();
	UE_LOG(LogTemp, Log, TEXT("Task Prerequisites End"));
}

void ATaskActor::TaskNested()
{
	// 親タスクAを起動後タスクA内でタスクBを起動し、2つのタスクが完了するまでブロックする
	FTask TaskA = Launch(TEXT("Task Nasted Outer"), []
		{
			FTask TaskB = Launch(TEXT("Task Nasted Inner"), &CalcFunc);
			AddNested(TaskB);
		}
	);

	TaskA.Wait();
	UE_LOG(LogTemp, Log, TEXT("Task Nasted End"));
}

void ATaskActor::TaskPipe()
{
	FPipe Pipe{ TEXT("Pipe") };

	// PipeでタスクAを起動
	FTask TaskA = Pipe.Launch(TEXT("Task Pipe TaskA"), []
		{
			FPlatformProcess::Sleep(1.0f);
			UE_LOG(LogTemp, Log, TEXT("TaskA End"));
		}
	);

	// PipeでタスクBを起動するが、タスクAが完了するまでは開始しない
	FTask TaskB = Pipe.Launch(TEXT("Task Pipe TaskB"), []
		{
			UE_LOG(LogTemp, Log, TEXT("TaskB End"));
		}
	);

	// タスクBが終わるまで待ち、AとBのタスクが順番に完了する
	TaskB.Wait();
	UE_LOG(LogTemp, Log, TEXT("Task Pipe End"));
}

void ATaskActor::TaskEvent()
{
	FTaskEvent Event{ TEXT("Event") };

	// TaskEventをLaunchで引数の最後に渡す
	Launch(UE_SOURCE_LOCATION, []
		{
			UE_LOG(LogTemp, Log, TEXT("TaskEvent Completed"));
		}, Event
	);

	// イベントとして登録されているタスクをトリガーして実行する
	Event.Trigger();
}
