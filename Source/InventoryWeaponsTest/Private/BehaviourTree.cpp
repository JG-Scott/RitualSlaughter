// Fill out your copyright notice in the Description page of Project Settings.

#include "BehaviourTree.h"

void UBehaviourTree::BehaviorTree(int state, EMultipleInputPins InputPins, EMultipleOutputPins &OutputPins)
{
    switch (InputPins)
    {
    case EMultipleInputPins::Exec:
        switch (state)
        {
        case 0: // travel
            OutputPins = EMultipleOutputPins::Travel;
            break;
        case 1: // patrol
            OutputPins = EMultipleOutputPins::Patrol;
            break;
        case 2: // alert
            OutputPins = EMultipleOutputPins::Alert;
            break;
        case 3: // attack
            OutputPins = EMultipleOutputPins::Attack;
            break;
        case 4: // investigate
            OutputPins = EMultipleOutputPins::Investigate;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}
