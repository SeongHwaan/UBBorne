// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Bloodborne, Log, All);

//define new logging macro
#define LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define LOG_S(Param) UE_LOG(Bloodborne, Param, TEXT("%s"), *LOG_CALLINFO)	
#define LOG(Param, Format, ...) UE_LOG(Bloodborne, Param, TEXT("%s %s"), *LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define CHECK(Expr, ...) {if (!(Expr)){LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}