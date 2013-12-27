#ifndef NEWELLSMETHOD_H_
#define NEWELLSMETHOD_H_

#include "YafMisc.h"
#include <vector>

inline YafXYZ<> CalculateSurfaceNormal(const std::vector<YafXYZ<>>& polygon)
{
    YafXYZ<> normal;

    for (size_t i = 0; i < polygon.size(); ++i)
    {
        const YafXYZ<>& current = polygon[i];
        const YafXYZ<>& next = polygon[(i + 1) % polygon.size()];

        normal.X += (current.Y - next.Y) * (current.Z + next.Z);
        normal.Y += (current.Z - next.Z) * (current.X + next.X);
        normal.Z += (current.X - next.X) * (current.Y + next.Y);
    }

    return normal.GetNormalized();
}

/*

Begin Function CalculateSurfaceNormal (Input Polygon) Returns Vector

    Set Vertex Normal to (0, 0, 0)

    Begin Cycle for Index in [0, Polygon.vertexNumber)

    Set Vertex Current to Polygon.verts[Index]
Set Vertex Next    to Polygon.verts[(Index plus 1) mod Polygon.vertexNumber]

Set Normal.x to Sum of Normal.x and (multiply (Current.y minus Next.y) by (Current.z plus Next.z))
    Set Normal.y to Sum of Normal.y and (multiply (Current.z minus Next.z) by (Current.x plus Next.x))
    Set Normal.z to Sum of Normal.z and (multiply (Current.x minus Next.x) by (Current.y plus Next.y))

    End Cycle

    Returning Normalize(Normal)

    End Function

*/

#endif // NEWELLSMETHOD_H_
