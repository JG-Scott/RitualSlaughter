// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "GenerateRandomPoints.generated.h"
/**
 *
 */

USTRUCT(BlueprintType)
struct FPoint
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	float x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	float y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	int32 id;

	FPoint() : x(0.0), y(0.0), id(0){};
	FPoint(float x, float y, int id, int SpacingValue) : x(x * SpacingValue), y(y * SpacingValue), id(id) {}

	float GetDistanceSquared(const FVector2D Other) const
	{
		return FVector2D::DistSquared(Other, FVector2d(x, y));
	}

	float GetDistance(const FVector2D &Other) const
	{
		return FMath::Sqrt(GetDistanceSquared(Other));
	}

	bool IsInCircle(const FVector2D &CenterPoint, float Radius) const
	{
		return FMath::Square(CenterPoint.X - x) + FMath::Square(CenterPoint.Y - y) <= FMath::Square(Radius);
	}

	// a helper function which determines if 2 vectors are nearly equal.
	bool IsNearlyEqual(const FPoint &Other) const
	{
		return (FMath::IsNearlyEqual(x, Other.x) && FMath::IsNearlyEqual(y, Other.y));
	}

	bool operator==(const FPoint &Other) const
	{
		return id == Other.id || (x == Other.x && y == Other.y);
	}

	// Override the inequality (!=) operator for FTriangle
	bool operator!=(const FPoint &Other) const
	{
		return !(*this == Other);
	}
};

USTRUCT(BlueprintType)
struct FDEdge
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	FPoint Point1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	FPoint Point2;

	float DistanceBetweenPoints = 0;

	FDEdge(){};
	FDEdge(const FPoint &P1, const FPoint &P2) : Point1(P1), Point2(P2)
	{
		DistanceBetweenPoints = GetLength();
	}

	float GetLength() const
	{
		return FVector2D::Distance(FVector2D(Point1.x, Point1.y), FVector2D(Point2.x, Point2.y));
	}

	bool IsSimilar(const FDEdge &Edge) const
	{
		// Returns true if P1 and P2 match in any direction
		return ((Point1.IsNearlyEqual(Edge.Point1) && Point2.IsNearlyEqual(Edge.Point2)) || (Point2.IsNearlyEqual(Edge.Point1) && Point1.IsNearlyEqual(Edge.Point2)));
	}

	bool IsNearlyEqual(const FDEdge &Edge) const
	{
		return (Point1.IsNearlyEqual(Edge.Point1) && Point2.IsNearlyEqual(Edge.Point2));
	}

	// Operators
	bool operator==(const FDEdge &Other) const
	{
		return (Point1 == Other.Point1 && Point2 == Other.Point2) || (Point1 == Other.Point2 && Point2 == Other.Point1);
	}
};

USTRUCT(BlueprintType)
struct FTriangle
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	FPoint P1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	FPoint P2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	FPoint P3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	FDEdge E1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	FDEdge E2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyCategory)
	FDEdge E3;

	// Initialize
	FTriangle() {}
	FTriangle(const FPoint &InP1, const FPoint &InP2, const FPoint &InP3)
		: P1(InP1), P2(InP2), P3(InP3), E1(FDEdge(InP1, InP2)), E2(FDEdge(InP2, InP3)), E3(FDEdge(InP3, InP1))
	{
	}
	float QuatCross(float InA, float InB, float InC) const
	{
		return FMath::Sqrt(
			(InA + InB + InC) * (InA + InB - InC) * (InA - InB + InC) * (-InA + InB + InC));
	}
	FVector GetSidesLength() const
	{
		return FVector(E1.GetLength(), E2.GetLength(), E3.GetLength());
	}

	FVector2D GetCenter() const
	{
		return FVector2D(
			(P1.x + P2.x + P3.x) / 3.f, (P1.y + P2.y + P3.y) / 3.f);
	}

	float GetCircumRadius() const
	{
		FVector SidesLength = GetSidesLength();
		float Cross = QuatCross(SidesLength.X, SidesLength.Y, SidesLength.Z);
		// Safe division
		return (Cross != 0.f ? (SidesLength.X * SidesLength.Y * SidesLength.Z) / Cross : 0.f);
	}

	FVector2D GetCircumCenter() const
	{

		float D = (P1.x * (P2.y - P3.y) + P2.x * (P3.y - P1.y) + P3.x * (P1.y - P2.y)) * 2;
		float X = ((P1.x * P1.x + P1.y * P1.y) * (P2.y - P3.y) + (P2.x * P2.x + P2.y * P2.y) * (P3.y - P1.y) + (P3.x * P3.x + P3.y * P3.y) * (P1.y - P2.y));
		float Y = ((P1.x * P1.x + P1.y * P1.y) * (P3.x - P2.x) + (P2.x * P2.x + P2.y * P2.y) * (P1.x - P3.x) + (P3.x * P3.x + P3.y * P3.y) * (P2.x - P1.x));

		return (D != 0.f ? FVector2D(X / D, Y / D) : FVector2D(0.f, 0.f));
	}

	float GetArea() const
	{
		FVector SidesLength = GetSidesLength();
		return (QuatCross(SidesLength.X, SidesLength.Y, SidesLength.Z) / 4.f);
	}

	bool IsInCircumCircle(const FPoint &InPoint) const
	{
		FVector2D Center = GetCircumCenter();
		return (FMath::Square(Center.X - InPoint.x) + FMath::Square(Center.Y - InPoint.y) <= FMath::Square(GetCircumRadius()));
	}

	bool IsNearlyEqual(const FTriangle &InTriangle) const
	{
		return (P1.IsNearlyEqual(InTriangle.P1) && P2.IsNearlyEqual(InTriangle.P2) && P3.IsNearlyEqual(InTriangle.P3));
	}

	// Operators
	bool operator==(const FTriangle &Other) const
	{
		return (
			P1 == Other.P1 && P2 == Other.P2 && P3 == Other.P3);
	}
};

UCLASS()
class INVENTORYWEAPONSTEST_API UGenerateRandomPoints : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	static TArray<FPoint> GeneratePoints(float XBounds, float YBounds, int NumberOfPoints, int SpacingValue);

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	static TArray<FTriangle> BowyerWatsonTriangulation(TArray<FPoint> InPoints);

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	static void DrawTriangluation(TArray<FTriangle> Triangulation, const UObject *WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	static TArray<FDEdge> PrimsAlgorithm(TArray<FTriangle> Triangulation, float AddBackPercentage);

public:
	static bool PointInsideCircumcircle(const FPoint &Point, const FTriangle &Triangle);

	static FVector CalculateCircumcenter(const FPoint &p1, const FPoint &p2, const FPoint &p3);
};
