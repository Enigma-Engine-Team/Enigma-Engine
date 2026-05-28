#include "emath/collision.h"
#include <iostream>
#include <algorithm>

bool Math::Collision::CheckAABBRay(const Ray& ray, const AABB& aabb, float& tOut)
{
    float tMin = 0.0f;
    float tMax = ray.length;
    
    // Pour chaque axe (X, Y, Z)
    for (int i = 0; i < 3; ++i) {
        // Astuce : on traite le vecteur comme un tableau [0], [1], [2]
        float invDir = 1.0f / (&ray.direction.x)[i];
        float t0 = ((&aabb.xMin)[i * 2] - (&ray.origin.x)[i]) * invDir;
        float t1 = ((&aabb.xMax)[i * 2] - (&ray.origin.x)[i]) * invDir;
    
        if (invDir < 0.0f) std::swap(t0, t1);
    
        tMin = std::max(tMin, t0);
        tMax = std::min(tMax, t1);
    
        if (tMin > tMax) return false;
    }
    
    tOut = tMin;
    return true;
}

// Möller–Trumbore algorithme
bool Math::Collision::CheckTriangleRay(const Ray& ray, const Triangle& triangle, float& tOut)
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();

    Math::Vector3D edge1 = triangle.b - triangle.a;
    Math::Vector3D edge2 = triangle.c - triangle.a;

    // Backface culling, assuming CCW-wound triangles.
    const Math::Vector3D normal = edge1.CrossProduct(edge2); // No need to normalize
    if (normal.DotProduct(ray.direction) > 0) 
        return false;

    Math::Vector3D ray_cross_e2 = ray.direction.CrossProduct(edge2);
    float det = edge1.DotProduct(ray_cross_e2);

    if (abs(det) < epsilon) 
        return false; // Ray is parallel to triangle

    float inv_det = 1.0 / det;
    Math::Vector3D s = ray.origin - triangle.a;
    float u = inv_det * s.DotProduct(ray_cross_e2);

    if (u < -epsilon || u - 1 > epsilon)
        return false; // Ray passes outside edge2's bounds

    Math::Vector3D s_cross_e1 = s.CrossProduct(edge1);
    float v = inv_det * ray.direction.DotProduct(s_cross_e1);

    if (v < -epsilon || u + v - 1 > epsilon) 
        return false; // Ray passes outside edge1's bounds

    // The ray line intersects with the triangle.
    // We compute t to find where on the ray the intersection is.
    float t = inv_det * edge2.DotProduct(s_cross_e1);

    if (t > epsilon) // Ray intersection
    {
        tOut = t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

bool Math::Collision::CheckFrustumAABB(const AABB& aabb, const Frustum& frustum)
{
    return CheckFrustumPlane(frustum.nearFace, aabb)
        && CheckFrustumPlane(frustum.farFace, aabb)
        && CheckFrustumPlane(frustum.leftFace, aabb)
        && CheckFrustumPlane(frustum.rightFace, aabb)
        && CheckFrustumPlane(frustum.topFace, aabb)
        && CheckFrustumPlane(frustum.bottomFace, aabb);
}


bool Math::Collision::CheckFrustumPlane(const Plane& plane, const Math::AABB& aabb)
{
    float px = (plane.normal.x >= 0.f) ? aabb.xMax : aabb.xMin;
    float py = (plane.normal.y >= 0.f) ? aabb.yMax : aabb.yMin;
    float pz = (plane.normal.z >= 0.f) ? aabb.zMax : aabb.zMin;

    Math::Vector3D pVertex(px, py, pz);
    return plane.GetSignedDistanceToPlane(pVertex) >= 0.f;
}
