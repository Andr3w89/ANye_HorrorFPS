// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"
#include "Components/ProgressBar.h"

void UHealthWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	//**If the health bar exists, update its fill based on the players health. 
	if (HealthBar)
	{
		const float HealthPercent = MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;

		HealthBar->SetPercent(FMath::Clamp(HealthPercent, 0.0f, 1.0f));
	}
}

