// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeBase.h"
#include "SnakeElementBase.h"
#include "Interactable.h"
#include "Food.h"
#include "Wall.h"


// Sets default values
ASnakeBase::ASnakeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ElementSize = 100.f;
	MovementSpeed = 10.f;
	MaxMovementSpeed = 50.0f;
	LastMoveDirection = EMovementDirection::DOWN;
	bCanChangeDirection = true;
}

// Called when the game starts or when spawned
void ASnakeBase::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickInterval(MovementSpeed);
	AddSnakeElement(5);
}

// Called every frame
void ASnakeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Move();

}

void ASnakeBase::AddSnakeElement(int ElementsNum)
{
	for (int i = 0; i < ElementsNum; ++i)
	{
		FVector NewLocation;

		if (SnakeElements.Num() > 0)
		{
			ASnakeElementBase* LastElement = SnakeElements.Last();
			FVector LastLocation = LastElement->GetActorLocation();
			FVector DirectionOffset = FVector::ZeroVector;

			switch (LastMoveDirection)
			{
			case EMovementDirection::UP:
				DirectionOffset = FVector(-ElementSize, 0, 0);
				break;
			case EMovementDirection::DOWN:
				DirectionOffset = FVector(ElementSize, 0, 0);
				break;
			case EMovementDirection::LEFT:
				DirectionOffset = FVector(0, ElementSize, 0);
				break;
			case EMovementDirection::RIGHT:
				DirectionOffset = FVector(0, -ElementSize, 0);
				break;
			}

			NewLocation = LastLocation + DirectionOffset;
		}
		else
		{
			NewLocation = GetActorLocation();
		}

		FTransform NewTransform(NewLocation);
		ASnakeElementBase* NewSnakeElement = GetWorld()->SpawnActor<ASnakeElementBase>(SnakeElementClass, NewTransform);
		NewSnakeElement->SnakeOwner = this;
		int32 ElementIndex = SnakeElements.Add(NewSnakeElement);
		if (ElementIndex == 0)
		{
			NewSnakeElement->SetFirstElementType();
		}
	}
}

void ASnakeBase::Move()
{
	FVector MovementVector(FVector::ZeroVector);

	switch(LastMoveDirection)
	{
	case EMovementDirection::UP:
		MovementVector.X += ElementSize;
		break;
	case EMovementDirection::DOWN:
		MovementVector.X -= ElementSize;
		break;
	case EMovementDirection::LEFT:
		MovementVector.Y += ElementSize;
		break;
	case EMovementDirection::RIGHT:
		MovementVector.Y -= ElementSize;
		break;
	}

	//AddActorWorldOffset(MovementVector);
	SnakeElements[0]->ToggleCollision();

	for (int i = SnakeElements.Num() - 1; i > 0; i--)
	{
		auto CurrentElement = SnakeElements[i];
		auto PrevElement = SnakeElements[i - 1];
		FVector PrevLocation = PrevElement->GetActorLocation();
		CurrentElement->SetActorLocation(PrevLocation);
	}

	SnakeElements[0]->AddActorWorldOffset(MovementVector);
	SnakeElements[0]->ToggleCollision();

	bCanChangeDirection = true;
}

void ASnakeBase::SnakeElementOverlap(ASnakeElementBase* OverlappedElement, AActor* Other)
{
	if (IsValid(OverlappedElement))
	{
		int32 ElementIndex;
		SnakeElements.Find(OverlappedElement, ElementIndex);
		bool bIsFirst = ElementIndex == 0;
		IInteractable* InteractableInterface = Cast<IInteractable>(Other);
		if (InteractableInterface)
		{
			InteractableInterface->Interact(this, bIsFirst);

			if (Other->IsA<AFood>())
			{
				ChangeMovementSpeed();
			}

			//if (Other->IsA<AWall>())
			//{
				//OverlappedElement->Destroy();
			//}
		}

	}
}

void ASnakeBase::ChangeMovementSpeed()
{
	if (MovementSpeed < MaxMovementSpeed)
	{
		MovementSpeed *= 1.05f;
		SetActorTickInterval(MovementSpeed);
	}
	
}

