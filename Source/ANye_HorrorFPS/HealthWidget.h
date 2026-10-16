// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

class UProgressBar;

UCLASS()
class ANYE_HORRORFPS_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//**Declare the function that updates the health bar. 
	void UpdateHealth(float CurrentHealth, float MaxHealth);

protected:
	//**Connect to the progress bar name healthbar in the widget. 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	
};
