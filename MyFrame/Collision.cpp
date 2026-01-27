#include "Collision.h"

namespace collide3d
{
    // -----------------------------
    // Box build: extract center/axes/scale
    // -----------------------------
    BoxCollider BoxCollider::BuildFromWorldMatrix(const dx::XMFLOAT4X4& world, const dx::XMFLOAT3& localHalf, float axisAlignedEps)
    {
        BoxCollider b{};

        // center from _41,_42,_43 (matches your ExtractTranslation)
        b.center = dx::XMFLOAT3(world._41, world._42, world._43);

        // Extract basis vectors from ROWS (row-major)
        dx::XMVECTOR row0 = dx::XMVectorSet(world._11, world._12, world._13, 0.f); // Right (with scale)
        dx::XMVECTOR row1 = dx::XMVectorSet(world._21, world._22, world._23, 0.f); // Up    (with scale)
        dx::XMVECTOR row2 = dx::XMVectorSet(world._31, world._32, world._33, 0.f); // Forward(with scale)

        float sx = Length3(row0);
        float sy = Length3(row1);
        float sz = Length3(row2);

        dx::XMVECTOR ax = Normalize3(row0);
        dx::XMVECTOR ay = Normalize3(row1);
        dx::XMVECTOR az = Normalize3(row2);

        // Apply scale to half extents (localHalf is model-space half extents)
        b.half = dx::XMFLOAT3(localHalf.x * sx, localHalf.y * sy, localHalf.z * sz);

        // Store axes
        dx::XMStoreFloat3(&b.axisX, ax);
        dx::XMStoreFloat3(&b.axisY, ay);
        dx::XMStoreFloat3(&b.axisZ, az);

        b.axisAligned = IsAxisAlignedAxes(ax, ay, az, axisAlignedEps);

        return b;
    }

    // -----------------------------
    // Typed intersections
    // -----------------------------
    bool Intersect(const SphereCollider& a, const SphereCollider& b)
    {
        dx::XMVECTOR ca = dx::XMLoadFloat3(&a.center);
        dx::XMVECTOR cb = dx::XMLoadFloat3(&b.center);
        dx::XMVECTOR d = dx::XMVectorSubtract(ca, cb);

        float r = a.radius + b.radius;
        return LengthSq3(d) <= r * r;
    }

    bool Intersect(const SphereCollider& s, const PointCollider& p)
    {
        dx::XMVECTOR c = dx::XMLoadFloat3(&s.center);
        dx::XMVECTOR pt = dx::XMLoadFloat3(&p.position);
        dx::XMVECTOR d = dx::XMVectorSubtract(c, pt);
        return LengthSq3(d) <= s.radius * s.radius;
    }

    // Point vs Box: if axisAligned, treat as AABB in world axes; else OBB local test.
    bool Intersect(const PointCollider& p, const BoxCollider& b)
    {
        dx::XMVECTOR pt = dx::XMLoadFloat3(&p.position);
        dx::XMVECTOR c = dx::XMLoadFloat3(&b.center);
        dx::XMVECTOR halfLocal = dx::XMLoadFloat3(&b.half);

        dx::XMVECTOR ax = dx::XMLoadFloat3(&b.axisX);
        dx::XMVECTOR ay = dx::XMLoadFloat3(&b.axisY);
        dx::XMVECTOR az = dx::XMLoadFloat3(&b.axisZ);

        dx::XMVECTOR d = dx::XMVectorSubtract(pt, c);

        if (b.axisAligned)
        {
            // AABB half extents in world axes (exact when axisAligned even with 90-degree swaps)
            dx::XMVECTOR halfWorld = ComputeAABBHalfFromAxes(ax, ay, az, halfLocal);
            dx::XMVECTOR ad = AbsV3(d);

            return (dx::XMVectorGetX(ad) <= dx::XMVectorGetX(halfWorld)) &&
                (dx::XMVectorGetY(ad) <= dx::XMVectorGetY(halfWorld)) &&
                (dx::XMVectorGetZ(ad) <= dx::XMVectorGetZ(halfWorld));
        }
        else
        {
            // OBB local coordinates
            float lx = Dot3(d, ax);
            float ly = Dot3(d, ay);
            float lz = Dot3(d, az);

            float hx = b.half.x;
            float hy = b.half.y;
            float hz = b.half.z;

            return (Abs(lx) <= hx) && (Abs(ly) <= hy) && (Abs(lz) <= hz);
        }
    }

    // Sphere vs Box: clamp closest point
    bool Intersect(const SphereCollider& s, const BoxCollider& b)
    {
        dx::XMVECTOR sc = dx::XMLoadFloat3(&s.center);
        dx::XMVECTOR c = dx::XMLoadFloat3(&b.center);
        dx::XMVECTOR halfLocal = dx::XMLoadFloat3(&b.half);

        dx::XMVECTOR ax = dx::XMLoadFloat3(&b.axisX);
        dx::XMVECTOR ay = dx::XMLoadFloat3(&b.axisY);
        dx::XMVECTOR az = dx::XMLoadFloat3(&b.axisZ);

        dx::XMVECTOR d = dx::XMVectorSubtract(sc, c);

        if (b.axisAligned)
        {
            // Compute exact world AABB half extents
            dx::XMVECTOR halfWorld = ComputeAABBHalfFromAxes(ax, ay, az, halfLocal);

            float hx = dx::XMVectorGetX(halfWorld);
            float hy = dx::XMVectorGetY(halfWorld);
            float hz = dx::XMVectorGetZ(halfWorld);

            // Closest point on AABB to sphere center
            float cx = dx::XMVectorGetX(sc);
            float cy = dx::XMVectorGetY(sc);
            float cz = dx::XMVectorGetZ(sc);

            float bx = dx::XMVectorGetX(c);
            float by = dx::XMVectorGetY(c);
            float bz = dx::XMVectorGetZ(c);

            float qx = Clamp(cx, bx - hx, bx + hx);
            float qy = Clamp(cy, by - hy, by + hy);
            float qz = Clamp(cz, bz - hz, bz + hz);

            dx::XMVECTOR q = dx::XMVectorSet(qx, qy, qz, 0.f);
            dx::XMVECTOR diff = dx::XMVectorSubtract(sc, q);
            return LengthSq3(diff) <= s.radius * s.radius;
        }
        else
        {
            // Sphere vs OBB: transform to OBB local, clamp, transform back
            float lx = Dot3(d, ax);
            float ly = Dot3(d, ay);
            float lz = Dot3(d, az);

            float hx = b.half.x;
            float hy = b.half.y;
            float hz = b.half.z;

            float clx = Clamp(lx, -hx, hx);
            float cly = Clamp(ly, -hy, hy);
            float clz = Clamp(lz, -hz, hz);

            // closestWorld = c + ax*clx + ay*cly + az*clz
            dx::XMVECTOR closest =
                dx::XMVectorAdd(c,
                    dx::XMVectorAdd(
                        dx::XMVectorScale(ax, clx),
                        dx::XMVectorAdd(dx::XMVectorScale(ay, cly), dx::XMVectorScale(az, clz))
                    )
                );

            dx::XMVECTOR diff = dx::XMVectorSubtract(sc, closest);
            return LengthSq3(diff) <= s.radius * s.radius;
        }
    }

    // OBB-OBB SAT helper (standard 15-axis test).
    // Uses floats for the core SAT math (stable & fast). Axes come from DirectXMath vectors.
    static bool IntersectOBB_OBB_SAT(const BoxCollider& A, const BoxCollider& B)
    {
        constexpr float EPS = 1e-6f;

        dx::XMVECTOR A0 = dx::XMLoadFloat3(&A.axisX);
        dx::XMVECTOR A1 = dx::XMLoadFloat3(&A.axisY);
        dx::XMVECTOR A2 = dx::XMLoadFloat3(&A.axisZ);

        dx::XMVECTOR B0 = dx::XMLoadFloat3(&B.axisX);
        dx::XMVECTOR B1 = dx::XMLoadFloat3(&B.axisY);
        dx::XMVECTOR B2 = dx::XMLoadFloat3(&B.axisZ);

        dx::XMVECTOR CA = dx::XMLoadFloat3(&A.center);
        dx::XMVECTOR CB = dx::XMLoadFloat3(&B.center);

        dx::XMVECTOR D = dx::XMVectorSubtract(CB, CA); // from A to B

        // Translation in A's frame
        float t[3] = { Dot3(D, A0), Dot3(D, A1), Dot3(D, A2) };

        // Rotation matrix R = Ai dot Bj
        float R[3][3] = {
            { Dot3(A0,B0), Dot3(A0,B1), Dot3(A0,B2) },
            { Dot3(A1,B0), Dot3(A1,B1), Dot3(A1,B2) },
            { Dot3(A2,B0), Dot3(A2,B1), Dot3(A2,B2) }
        };

        float AbsR[3][3];
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                AbsR[i][j] = std::fabs(R[i][j]) + EPS;

        const float a[3] = { A.half.x, A.half.y, A.half.z };
        const float b[3] = { B.half.x, B.half.y, B.half.z };

        auto Sep = [&](float dist, float ra, float rb) -> bool
            {
                return std::fabs(dist) > (ra + rb);
            };

        // 1) Axes A0,A1,A2
        if (Sep(t[0], a[0], b[0] * AbsR[0][0] + b[1] * AbsR[0][1] + b[2] * AbsR[0][2])) return false;
        if (Sep(t[1], a[1], b[0] * AbsR[1][0] + b[1] * AbsR[1][1] + b[2] * AbsR[1][2])) return false;
        if (Sep(t[2], a[2], b[0] * AbsR[2][0] + b[1] * AbsR[2][1] + b[2] * AbsR[2][2])) return false;

        // 2) Axes B0,B1,B2 (translation in B frame)
        float tB[3] = {
            t[0] * R[0][0] + t[1] * R[1][0] + t[2] * R[2][0],
            t[0] * R[0][1] + t[1] * R[1][1] + t[2] * R[2][1],
            t[0] * R[0][2] + t[1] * R[1][2] + t[2] * R[2][2]
        };

        if (Sep(tB[0], a[0] * AbsR[0][0] + a[1] * AbsR[1][0] + a[2] * AbsR[2][0], b[0])) return false;
        if (Sep(tB[1], a[0] * AbsR[0][1] + a[1] * AbsR[1][1] + a[2] * AbsR[2][1], b[1])) return false;
        if (Sep(tB[2], a[0] * AbsR[0][2] + a[1] * AbsR[1][2] + a[2] * AbsR[2][2], b[2])) return false;

        // 3) 9 cross products Ai x Bj
        // A0 x B0
        if (Sep(t[2] * R[1][0] - t[1] * R[2][0],
            a[1] * AbsR[2][0] + a[2] * AbsR[1][0],
            b[1] * AbsR[0][2] + b[2] * AbsR[0][1])) return false;
        // A0 x B1
        if (Sep(t[2] * R[1][1] - t[1] * R[2][1],
            a[1] * AbsR[2][1] + a[2] * AbsR[1][1],
            b[0] * AbsR[0][2] + b[2] * AbsR[0][0])) return false;
        // A0 x B2
        if (Sep(t[2] * R[1][2] - t[1] * R[2][2],
            a[1] * AbsR[2][2] + a[2] * AbsR[1][2],
            b[0] * AbsR[0][1] + b[1] * AbsR[0][0])) return false;

        // A1 x B0
        if (Sep(t[0] * R[2][0] - t[2] * R[0][0],
            a[0] * AbsR[2][0] + a[2] * AbsR[0][0],
            b[1] * AbsR[1][2] + b[2] * AbsR[1][1])) return false;
        // A1 x B1
        if (Sep(t[0] * R[2][1] - t[2] * R[0][1],
            a[0] * AbsR[2][1] + a[2] * AbsR[0][1],
            b[0] * AbsR[1][2] + b[2] * AbsR[1][0])) return false;
        // A1 x B2
        if (Sep(t[0] * R[2][2] - t[2] * R[0][2],
            a[0] * AbsR[2][2] + a[2] * AbsR[0][2],
            b[0] * AbsR[1][1] + b[1] * AbsR[1][0])) return false;

        // A2 x B0
        if (Sep(t[1] * R[0][0] - t[0] * R[1][0],
            a[0] * AbsR[1][0] + a[1] * AbsR[0][0],
            b[1] * AbsR[2][2] + b[2] * AbsR[2][1])) return false;
        // A2 x B1
        if (Sep(t[1] * R[0][1] - t[0] * R[1][1],
            a[0] * AbsR[1][1] + a[1] * AbsR[0][1],
            b[0] * AbsR[2][2] + b[2] * AbsR[2][0])) return false;
        // A2 x B2
        if (Sep(t[1] * R[0][2] - t[0] * R[1][2],
            a[0] * AbsR[1][2] + a[1] * AbsR[0][2],
            b[0] * AbsR[2][1] + b[1] * AbsR[2][0])) return false;

        return true;
    }

    // Box vs Box: fast path AABB-AABB when both are axisAligned, otherwise SAT.
    bool Intersect(const BoxCollider& a, const BoxCollider& b)
    {
        dx::XMVECTOR ca = dx::XMLoadFloat3(&a.center);
        dx::XMVECTOR cb = dx::XMLoadFloat3(&b.center);
        dx::XMVECTOR d = dx::XMVectorSubtract(ca, cb);

        dx::XMVECTOR axA = dx::XMLoadFloat3(&a.axisX);
        dx::XMVECTOR ayA = dx::XMLoadFloat3(&a.axisY);
        dx::XMVECTOR azA = dx::XMLoadFloat3(&a.axisZ);

        dx::XMVECTOR axB = dx::XMLoadFloat3(&b.axisX);
        dx::XMVECTOR ayB = dx::XMLoadFloat3(&b.axisY);
        dx::XMVECTOR azB = dx::XMLoadFloat3(&b.axisZ);

        dx::XMVECTOR halfA = dx::XMLoadFloat3(&a.half);
        dx::XMVECTOR halfB = dx::XMLoadFloat3(&b.half);

        if (a.axisAligned && b.axisAligned)
        {
            // exact AABB extents in world axes (works even for 90-degree swaps)
            dx::XMVECTOR extA = ComputeAABBHalfFromAxes(axA, ayA, azA, halfA);
            dx::XMVECTOR extB = ComputeAABBHalfFromAxes(axB, ayB, azB, halfB);

            dx::XMVECTOR ad = AbsV3(d);

            return (dx::XMVectorGetX(ad) <= dx::XMVectorGetX(dx::XMVectorAdd(extA, extB))) &&
                (dx::XMVectorGetY(ad) <= dx::XMVectorGetY(dx::XMVectorAdd(extA, extB))) &&
                (dx::XMVectorGetZ(ad) <= dx::XMVectorGetZ(dx::XMVectorAdd(extA, extB)));
        }

        // General OBB vs OBB SAT
        return IntersectOBB_OBB_SAT(a, b);
    }

    // -----------------------------
    // Dispatch table
    // -----------------------------
    template<class A, class B>
    static bool Dispatch(const Collision3D& a, const Collision3D& b)
    {
        return Intersect(static_cast<const A&>(a), static_cast<const B&>(b));
    }

    const std::array<std::array<CollideFn, (size_t)CollideType::Count>, (size_t)CollideType::Count>&
        CollisionSystem::Table()
    {
        static std::array<std::array<CollideFn, (size_t)CollideType::Count>, (size_t)CollideType::Count> tbl = [] {
            decltype(tbl) t{};
            for (auto& row : t) row.fill(nullptr);

            auto set = [&](CollideType A, CollideType B, CollideFn fnAB, CollideFn fnBA = nullptr)
                {
                    t[(size_t)A][(size_t)B] = fnAB;
                    t[(size_t)B][(size_t)A] = fnBA ? fnBA : fnAB; // symmetric by default
                };

            set(CollideType::Sphere, CollideType::Sphere, &Dispatch<SphereCollider, SphereCollider>);
            set(CollideType::Sphere, CollideType::Point, &Dispatch<SphereCollider, PointCollider>);

            // Point-Box is not symmetric if you want Point-Box to call Intersect(Point,Box) directly:
            set(CollideType::Point, CollideType::Box, &Dispatch<PointCollider, BoxCollider>);

            // Sphere-Box: implement as Intersect(Sphere,Box); symmetric dispatch uses the same function
            set(CollideType::Sphere, CollideType::Box, &Dispatch<SphereCollider, BoxCollider>);

            set(CollideType::Box, CollideType::Box, &Dispatch<BoxCollider, BoxCollider>);

            return t;
            }();
        return tbl;
    }

    bool CollisionSystem::IsOverlap(const Collision3D& a, const Collision3D& b)
    {
        const auto ta = (size_t)a.GetType();
        const auto tb = (size_t)b.GetType();
        auto fn = Table()[ta][tb];
        return fn ? fn(a, b) : false;
    }

} // namespace collide3d