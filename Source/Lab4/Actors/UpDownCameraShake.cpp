// Fill out your copyright notice in the Description page of Project Settings.


#include "UpDownCameraShake.h"

UUpDownCameraShake::UUpDownCameraShake()
{
    OscillationDuration = -1.0f;
    OscillationBlendInTime = 0.0f;
    OscillationBlendOutTime = 0.0f;


    LocOscillation.Z.Amplitude = FMath::RandRange(0.5f, 2.5f);
    LocOscillation.Z.Frequency = 1.0f;
}