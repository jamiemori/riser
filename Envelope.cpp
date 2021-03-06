/*
  ==============================================================================

    Envelope.cpp
    Created: 22 Jan 2015 8:54:53pm
    Author:  Yoda

  ==============================================================================
*/

#include "Envelope.h"
#include <algorithm>
#include <math.h>

Envelope::Envelope() : 
	minimumLevel(0.0001),	
    currentStage(stageOff),
    currentLevel(minimumLevel),
    multiplier(1.0),
    currentSampleIndex(0),
    nextStageSampleIndex(0)
{
	stageValue[stageOff]     = 0.01;
	stageValue[stageAttack]  = 0.5;
	stageValue[stageDecay]   = 0.0;
	stageValue[stageSustain] = 0.1;
	stageValue[stageRelease] = 1.0;
}

Envelope::~Envelope(){}

float Envelope::nextSample()
{
	if (currentStage != stageOff && currentStage != stageSustain)
	{
		if (currentSampleIndex == nextStageSampleIndex)
		{
			envelopeStage newStage = static_cast<envelopeStage>((currentStage + 1) % numEnvelopeStages);
			enterStage(newStage);
		}
		
		currentLevel *= multiplier;
		currentSampleIndex++;
	}
	return currentLevel;
}

void Envelope::calculateMultiplier (float startLevel, float stopLevel, int lengthInSamples)
{
	multiplier = 1.0 + (log(stopLevel) - log(startLevel)) / (lengthInSamples);
}


void Envelope::setEnvelopeParameter(envelopeStage stage, float newValue)
{
	stageValue[stage] = newValue;
	
	if (stage == currentStage)
	{
		if (currentStage == stageAttack || currentStage == stageDecay || currentStage || stageRelease)
		{
			float nextLevelValue;
			switch (currentStage) 
			{
				case stageAttack:
					nextLevelValue = 1.0;
					break;

				case stageDecay:
					nextLevelValue = std::max(stageValue[stageSustain], minimumLevel);
					break;

				case stageRelease:
					nextLevelValue = minimumLevel;
					break;
				default:
					break;
			}

			float currentStageProcess = (currentSampleIndex + 0.0) / nextStageSampleIndex;
			float remainingStageProcess =  1.0 - currentStageProcess ;
			int samplesUntilNextStage = remainingStageProcess * newValue * 44100;
			nextStageSampleIndex =  currentSampleIndex + samplesUntilNextStage;
			calculateMultiplier(currentLevel, nextLevelValue, samplesUntilNextStage);
		}

		else if (currentStage == stageSustain)
		{
			currentLevel = newValue;
		} 
	}

	if (currentStage == stageDecay && stage == stageSustain) 
	{
    unsigned long long samplesUntilNextStage = nextStageSampleIndex - currentSampleIndex;
    calculateMultiplier(currentLevel, std::max(stageValue[stageSustain], minimumLevel), samplesUntilNextStage);
	}
}

void Envelope::enterStage (envelopeStage newStage)
{
	currentStage = newStage;
	currentSampleIndex = 0;
 	if(currentStage == stageOff || currentStage == stageSustain)
 	{
 		nextStageSampleIndex = 0;
 	}
 	else
 	{
 		nextStageSampleIndex = stageValue[currentStage] * 44100.0f;
 	}
 	switch (newStage)
 	{
 		case stageOff:
 			currentLevel = 0.0;
 			multiplier = 1.0;
 			break;
 		case stageAttack:
 			currentLevel = minimumLevel;
 			calculateMultiplier (currentLevel, 1.0, nextStageSampleIndex);
 			break;
 		case stageSustain:
 			currentLevel = stageValue[stageSustain];
 			multiplier = 1.0;
 			break;
 		case stageDecay:
 			currentLevel = 1.0;
 			calculateMultiplier(currentLevel , std::max(stageValue[stageSustain], minimumLevel), nextStageSampleIndex);
 			break;
 		case stageRelease:
 			calculateMultiplier (currentLevel, minimumLevel, nextStageSampleIndex);
 			break;
 		default:
 			break;
 	}
}
