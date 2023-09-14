#include "MyCameraShake.h"

UMyCameraShake::UMyCameraShake()
{

    OscillationDuration = -1.0f;
    OscillationBlendInTime = 0.0f;
    OscillationBlendOutTime = 0.0f;

    LocOscillation.X.Amplitude = FMath::RandRange(0.5f, 2.0f);
    LocOscillation.X.Frequency = 1.0f;

    LocOscillation.Z.Amplitude = FMath::RandRange(1.0f, 5.0f);
    LocOscillation.Z.Frequency = 1.0f;

    LocOscillation.Z.Amplitude = FMath::RandRange(1.0f, 3.0f);
    LocOscillation.Z.Frequency = 1.0f;


    RotOscillation.Roll.Amplitude = FMath::RandRange(1.0f, 3.0f);
    RotOscillation.Roll.Frequency = 0.1f;
}