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

	// �^�X�N�����J�n
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
	// �^�X�N�������_�֐��ŋN��
	Launch(TEXT("Task Launch"), []
		{
			UE_LOG(LogTemp, Log, TEXT("Task Launch Begin"));
		}
	);
}

void ATaskActor::TaskWait()
{
	// �^�X�N�N����A�^�X�N����������܂ŃX���b�h���u���b�N
	FTask Task = Launch(TEXT("Task Wait"), &CalcFunc);

	UE_LOG(LogTemp, Log, TEXT("Task Wait Begin"));
	Task.Wait();
	UE_LOG(LogTemp, Log, TEXT("Task Wait End"));
}

void ATaskActor::TaskBusyWait()
{
	// ��Ƀ^�X�NA���N������
	FTask TaskA = Launch(TEXT("TaskA Busy Wait"), []
		{
			FPlatformProcess::Sleep(1.0f);
			UE_LOG(LogTemp, Log, TEXT("TaskA End"));
		}
	);

	UE_LOG(LogTemp, Log, TEXT("Task Busy Wait Begin"));

	// �^�X�NB�̓^�X�NA����Ɋ�������
	FTask TaskB = Launch(TEXT("TaskB Busy Wait"), []
		{
			FPlatformProcess::Sleep(0.5f);
			UE_LOG(LogTemp, Log, TEXT("TaskB End"));
		}
	);

	// �^�X�NA��BusyWait����ƁA��������������܂őҋ@����
	// ���̊Ԃ��^�X�NB�͏��������̂ŁA��Ƀ^�X�NB���������A��ɐi��
	TaskA.BusyWait();

	UE_LOG(LogTemp, Log, TEXT("Task Busy Wait End"));
}

void ATaskActor::TaskGetRusult()
{
	// �^�X�N��bool�l��Ԃ�
	TTask<bool> BoolTask = Launch(TEXT("Task Return Bool"), []
		{
			return true;
		}
	);

	// GetRusult�Œ��g���m�F����
	bool bResult = BoolTask.GetResult();
	UE_LOG(LogTemp, Log, TEXT("BoolTask Result = %s"), bResult ? TEXT("true") : TEXT("false"));
}

void ATaskActor::TaskPrerequisites()
{
	// �^�X�NA���N��
	FTask TaskA = Launch(TEXT("Task Prereqs TaskA"), []
		{
			FPlatformProcess::Sleep(1.0f);
			UE_LOG(LogTemp, Log, TEXT("TaskA End"));
		}
	);
	
	// �^�X�NB�ƃ^�X�NC�̓^�X�NA����������܂ł͋N�����Ȃ�
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

	// �^�X�ND�̓^�X�NB�ƃ^�X�NC����������܂ł͋N�����Ȃ�
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
	// �e�^�X�NA���N����^�X�NA���Ń^�X�NB���N�����A2�̃^�X�N����������܂Ńu���b�N����
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

	// Pipe�Ń^�X�NA���N��
	FTask TaskA = Pipe.Launch(TEXT("Task Pipe TaskA"), []
		{
			FPlatformProcess::Sleep(1.0f);
			UE_LOG(LogTemp, Log, TEXT("TaskA End"));
		}
	);

	// Pipe�Ń^�X�NB���N�����邪�A�^�X�NA����������܂ł͊J�n���Ȃ�
	FTask TaskB = Pipe.Launch(TEXT("Task Pipe TaskB"), []
		{
			UE_LOG(LogTemp, Log, TEXT("TaskB End"));
		}
	);

	// �^�X�NB���I���܂ő҂��AA��B�̃^�X�N�����ԂɊ�������
	TaskB.Wait();
	UE_LOG(LogTemp, Log, TEXT("Task Pipe End"));
}

void ATaskActor::TaskEvent()
{
	FTaskEvent Event{ TEXT("Event") };

	// TaskEvent��Launch�ň����̍Ō�ɓn��
	Launch(UE_SOURCE_LOCATION, []
		{
			UE_LOG(LogTemp, Log, TEXT("TaskEvent Completed"));
		}, Event
	);

	// �C�x���g�Ƃ��ēo�^����Ă���^�X�N���g���K�[���Ď��s����
	Event.Trigger();
}
