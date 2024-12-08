// Fill out your copyright notice in the Description page of Project Settings.

#include "GenerateRandomPoints.h"

// This is a  function to Generate a list of random (x,y) points to be used with the BowyerWatson Triagulation algorithm
TArray<FPoint>
UGenerateRandomPoints::GeneratePoints(float XBounds, float YBounds, int NumberOfPoints, int SpacingValue)
{

    TArray<FPoint> points;

    for (int i = 0; i < NumberOfPoints; i++)
    {

        int XLocation = FMath::RandRange(0.0f, XBounds);
        int YLocation = FMath::RandRange(0.0f, YBounds);
        FPoint p = FPoint(XLocation, YLocation, -1, SpacingValue);
        points.Add(p);
    }

    return points;
}

TArray<FTriangle> UGenerateRandomPoints::BowyerWatsonTriangulation(TArray<FPoint> InPoints)
{

    TArray<FTriangle> Triangles;
    int32 NPoints = InPoints.Num();
    if (NPoints < 3)
    {
        UE_LOG(LogActor, Error, TEXT("Triangulate needs at least 3 points."));
        return Triangles;
    }
    if (NPoints == 3)
    {
        Triangles.Add(FTriangle(InPoints[0], InPoints[1], InPoints[2]));
        return Triangles;
    }

    // Start (Bowyer Watson) Delaunay triangulation.

    // Get the max amount of expected triangles.
    int32 TrMax = NPoints * 4;
    // Get the min / max dimensions of the grid containing the points.
    float MinX = InPoints[0].x;
    float MinY = InPoints[0].y;
    float MaxX = MinX;
    float MaxY = MinY;

    for (int32 i = 0; i < NPoints; i++)
    {
        FPoint &Point = InPoints[i];
        Point.id = i;

        MinX = FMath::Min(MinX, Point.x);
        MaxX = FMath::Max(MaxX, Point.x);

        MinY = FMath::Min(MinY, Point.y);
        MaxY = FMath::Max(MaxY, Point.y);
    }

    float MidX = (MinX + MaxX) * 0.5f;
    float MidY = (MinY + MaxY) * 0.5f;

    // Add Super Triangle. For simplicity add the generated Super points on top of the point array.
    FPoint SuP1 = FPoint(-100, -50, NPoints, 500);
    FPoint SuP2 = FPoint(200, -50, NPoints + 1, 500);
    FPoint SuP3 = FPoint(100, 400, NPoints + 2, 500);
    InPoints.EmplaceAt(SuP1.id, SuP1);
    InPoints.EmplaceAt(SuP2.id, SuP2);
    InPoints.EmplaceAt(SuP3.id, SuP3);
    Triangles.Add(FTriangle(SuP1, SuP2, SuP3));

    // Use NPoints instead of InPoints.Num() when looping over the original points, to not include the initial super points.
    for (int32 i = 0; i < NPoints; i++)
    {
        TArray<FDEdge> Edges;

        // For each point, look which triangles their CircumCircle contains this point
        // , in which case we don't want the triangle because it is not a Delaunay triangle.
        for (int32 j = Triangles.Num() - 1; j >= 0; j--)
        {
            FTriangle CurTriangle = Triangles[j];
            if (CurTriangle.IsInCircumCircle(InPoints[i]))
            {

                Edges.Add(CurTriangle.E1);
                Edges.Add(CurTriangle.E2);
                Edges.Add(CurTriangle.E3);
                // Remove the unwanted triangle
                Triangles.RemoveAt(j);
            }
        }

        for (int32 j = Edges.Num() - 2; j >= 0; j--)
        {
            for (int32 k = Edges.Num() - 1; k >= j + 1; k--)
            {
                if (Edges.IsValidIndex(j) && Edges.IsValidIndex(k) && Edges[j].IsSimilar(Edges[k]))
                {
                    Edges.RemoveAt(j);
                    Edges.RemoveAt(k - 1);
                }
            }
        }

        for (int32 j = 0; j < Edges.Num(); j++)
        {
            if (Triangles.Num() > TrMax)
            {
                UE_LOG(LogActor, Error, TEXT("Made more triangles than required. "));
            }

            Triangles.Add(FTriangle(Edges[j].Point1, Edges[j].Point2, InPoints[i]));
        }
    }

    // Remove triangles using the initial super points.
    for (int i = Triangles.Num() - 1; i >= 0; i--)
    {
        FTriangle Triangle = Triangles[i];
        if (Triangle.P1.id >= NPoints || Triangle.P2.id >= NPoints || Triangle.P3.id >= NPoints)
        {
            Triangles.RemoveAt(i);
        }
    }
    // Remove the initial super points.
    InPoints.Remove(SuP1);
    InPoints.Remove(SuP2);
    InPoints.Remove(SuP3);

    return Triangles;
}

void UGenerateRandomPoints::DrawTriangluation(TArray<FTriangle> Triangulation, const UObject *WorldContextObject)
{

    for (const FTriangle &TriangleX : Triangulation)
    {
        for (int32 i = 0; i < 3; i++)
        {
            const FDEdge EdgeX = (i == 0   ? TriangleX.E1
                                  : i == 1 ? TriangleX.E2
                                  : i == 2 ? TriangleX.E3
                                           // Invalid
                                           : FDEdge(FPoint(0.f, 0.f, -1, 1), FPoint(0.f, 0.f, -1, 1)));
            // For each edge per triangle per room:

            UKismetSystemLibrary::DrawDebugLine(
                WorldContextObject, FVector(EdgeX.Point1.x, EdgeX.Point1.y, 0.f), FVector(EdgeX.Point2.x, EdgeX.Point2.y, 0.f), FColor::Silver, 5.f, 8.f);
        }
    }
}

TArray<FDEdge> UGenerateRandomPoints::PrimsAlgorithm(TArray<FTriangle> Triangulation, float AddBackPercentage)
{
    TArray<FPoint> points;
    TArray<FDEdge> edges;
    TArray<FDEdge> MST;
    TArray<FPoint> visitedPoints;

    // creates a list of points and edges without repetition.
    for (FTriangle t : Triangulation)
    {
        if (!points.Contains(t.P1))
        {
            points.Add(t.P1);
        }
        if (!points.Contains(t.P2))
        {
            points.Add(t.P2);
        }
        if (!points.Contains(t.P3))
        {
            points.Add(t.P3);
        }

        if (!edges.Contains(t.E1))
        {
            edges.Add(t.E1);
        }
        if (!edges.Contains(t.E2))
        {
            edges.Add(t.E2);
        }
        if (!edges.Contains(t.E3))
        {
            edges.Add(t.E3);
        }
    }

    // now that we have a list of each of the nodes we iterate through the nodes adding chosen edges to the mst array
    //
    int totalPoints = points.Num();

    // if there are no points -> no edges, return empty array
    if (points.Num() <= 0)
        return MST;

    // add first node into visited points
    visitedPoints.Add(points[0]);
    FDEdge minEdge;
    while (visitedPoints.Num() != totalPoints)
    {
        float currentMin = BIG_NUMBER;

        for (FPoint point : visitedPoints)
        {
            // for each edge in edges
            for (FDEdge edge : edges)
            {
                if (edge.Point1 == point && !visitedPoints.Contains(edge.Point2) || edge.Point2 == point && !visitedPoints.Contains(edge.Point1))
                {
                    if (edge.DistanceBetweenPoints < currentMin)
                    {
                        currentMin = edge.DistanceBetweenPoints;
                        minEdge = edge;
                    }
                }
            }
        }
        MST.Add(minEdge);
        edges.Remove(minEdge);
        if (visitedPoints.Contains(minEdge.Point1))
        {
            visitedPoints.Add(minEdge.Point2);
        }
        else
        {
            visitedPoints.Add(minEdge.Point1);
        }
    }

    // Adds back edges after creating the MST based on the AddBackPercentage Parameter.
    // This is to make the map more circular/interesting.
    if (AddBackPercentage > 0.0)
    {
        for (FDEdge e : edges)
        {
            if (FMath::RandRange(0.0, 1.0) <= AddBackPercentage)
            {
                MST.Add(e);
            }
        }
    }

    return MST;
}