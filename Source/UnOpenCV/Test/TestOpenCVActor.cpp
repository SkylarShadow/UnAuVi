// Fill out your copyright notice in the Description page of Project Settings.


#include "TestOpenCVActor.h"

#if WITH_OPENCV
#include "opencv2/core.hpp"   
#endif

// Sets default values
ATestOpenCVActor::ATestOpenCVActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestOpenCVActor::BeginPlay()
{
	Super::BeginPlay();
	const auto OpenCVVersion = cv::getVersionString();
	//打印OpenCV版本号，验证是否使用了正确的版本
	UE_LOG(LogTemp, Warning, TEXT("OpenCV version：%s"), *FString(OpenCVVersion.c_str()));
	
}

// Called every frame
void ATestOpenCVActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

