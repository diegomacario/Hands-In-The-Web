#include "pch.h"
#include "WebAlembicViewer.h"
#include "SceneGraph.h"

namespace wabc {

// Mul: e.g. [](float4x4, float3) -> float3
template<class Vec, class Mul>
bool Skin::deformImpl(span<Vec> dst, span<Vec> src, const Mul& mul) const
{
    if (m_counts.size() != src.size() || m_counts.size() != dst.size()) {
        printf("Skin::deformImpl(): vertex count mismatch\n");
        return false;
    }

    const JointWeight* weights = m_weights.data();
    size_t nvertices = src.size();
    for (size_t vi = 0; vi < nvertices; ++vi) {
        Vec p = src[vi];
        Vec r{};
        int cjoints = m_counts[vi];
        for (int bi = 0; bi < cjoints; ++bi) {
            JointWeight w = weights[bi];
            r += mul(m_matrices[w.index], p) * w.weight;
        }
        dst[vi] = r;
        weights += cjoints;
    }
    return true;
}

bool Skin::deformPoints(span<float3> dst, span<float3> src) const
{
    return deformImpl(dst, src,
        [](float4x4 m, float3 p) { return mul_p(m, p); });
}

bool Skin::deformNormals(span<float3> dst, span<float3> src) const
{
    return deformImpl(dst, src,
        [](float4x4 m, float3 p) { return mul_v(m, p); });
}


bool BlendShape::deformPoints(span<float3> dst, span<float3> src, float w) const
{
    if (dst.data() != src.data())
        memcpy(dst.data(), src.data(), src.size_bytes());

    size_t c = m_indices.size();
    for (size_t i = 0; i < c; ++i)
        dst[m_indices[i]] += m_delta_points[i];
    return true;
}

bool BlendShape::deformNormals(span<float3> dst, span<float3> src, float w) const
{
    if (dst.data() != src.data())
        memcpy(dst.data(), src.data(), src.size_bytes());

    size_t c = m_indices.size();
    for (size_t i = 0; i < c; ++i)
        dst[m_indices[i]] += m_delta_normals[i];
    return true;
}

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::clear()
{
    m_points.clear();
    m_points_ex.clear();
    m_normals.clear();
    m_normals_ex.clear();

    m_counts.clear();
    m_face_indices.clear();
    m_wireframe_indices.clear();
}

Points::Points()
{

}

Points::~Points()
{

}

void Points::clear()
{
    m_points.clear();
}

IScene* LoadScene_(const char* path)
{
    if (!path)
        return nullptr;
    size_t len = std::strlen(path);
    if (len > 4) {
        char ext[3]{};
        for (int i = 0; i < 3; ++i)
            ext[i] = std::tolower(path[len - 3 + i]);

        if (std::memcmp(ext, "abc", 3) == 0) {
            auto scene = CreateSceneABC_();
            if (scene->load(path))
                return scene;
            else
                scene->release();
        }
    }
    return nullptr;
}

} // namespace wabc
