// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TaskActor.generated.h"

UCLASS()
class TASKSYSTEMSAMPLE_API ATaskActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATaskActor();

protected:
	virtual void BeginPlay() override;

public:	

	void TaskLaunch();
	void TaskWait();
	void TaskBusyWait();
	void TaskGetRusult();
	void TaskPrerequisites();
	void TaskNested();
	void TaskPipe();
	void TaskEvent();
};
