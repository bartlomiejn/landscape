#pragma once

#include <stdint.h>
#include <array>
#include <algorithm>
#include <graphics/mesh.h>
#include <logger.h>

#define VOL_DBG Log::Logger(Log::Level::Debug, "gfx.volume")

#define VERT_FACE_COUNT 48
#define VERT_STRIDE 8

namespace GFX
{
using FaceVertArray = std::array<float, VERT_FACE_COUNT>;

enum class Side {
    Back,
    Front,
    Left,
    Right,
    Bottom,
    Top
};

std::string toString(GFX::Side side) {
    switch (side)
    {
        case (Side::Back):
            return "Side::Back";
        case (Side::Front):
            return "Side::Front";
        case (Side::Left):
            return "Side::Left";
        case (Side::Right):
            return "Side::Right";
        case (Side::Bottom):
            return "Side::Bottom";
        case (Side::Top):
            return "Side::Top";
    }
};

struct Voxel
{
    uint32_t material = 0;
    uint64_t reserved = 0;
    uint64_t reserved_2 = 0;
};

template <int x_count, int y_count, int z_count>
class Volume
{
public:
    Volume() {};
    virtual ~Volume() {};

    Voxel &voxel_at(int x, int y, int z)
    {
        return voxels.at(x_count * y_count * z + x_count * y + x);
    }

    bool is_empty_at(int x, int y, int z)
    {
        return !((bool)voxel_at(x, y, z).material);
    }

private:
    std::array<GFX::Voxel, x_count * y_count * z_count> voxels;
};

template <int x_count, int y_count, int z_count>
class VolumeMesh : public GFX::Mesh
{
public:
    VolumeMesh(GFX::Volume<x_count, y_count, z_count> &volume, float vox_sz) 
        : GFX::Mesh(), volume(volume), vox_sz(vox_sz)
    {
        generate_from(volume);
    };

    virtual ~VolumeMesh() 
    {
        free(verts);
    };

    void load() override {
        Mesh::load();
        add_vertex_attrib_array(0, 3, (void *)nullptr);
	    add_vertex_attrib_array(1, 3, (void *)(3 * sizeof(float)));
	    add_vertex_attrib_array(2, 2, (void *)(6 * sizeof(float)));
    };

private:
    GFX::Volume<x_count, y_count, z_count> &volume;
    float vox_sz;

    void generate_from(GFX::Volume<x_count, y_count, z_count> &volume)
    {
        std::vector<float> verts;

        for (int ix = 0; ix < x_count; ix++)
        for (int iy = 0; iy < y_count; iy++)
        for (int iz = 0; iz < z_count; iz++)
        {
            if (volume.is_empty_at(ix, iy, iz)) 
            {
                continue;
            }
                
            if (ix == 0 || (ix > 0 && volume.is_empty_at(ix - 1, iy, iz))) 
            {
                for (auto &v : face_verts(Side::Left, ix, iy, iz, 1.0))
                    verts.push_back(v);
            }
            if (ix == x_count - 1 
                || (ix < x_count - 1 && volume.is_empty_at(ix + 1, iy, iz))) 
            {
                for (auto &v : face_verts(Side::Right, ix, iy, iz, 1.0))
                    verts.push_back(v);
            }  

            if (iy == 0 || (iy > 0 && volume.is_empty_at(ix, iy - 1, iz))) 
            {
                for (auto &v : face_verts(Side::Bottom, ix, iy, iz, 1.0))
                    verts.push_back(v);
            }
            if (iy == y_count - 1 
                || (iy < y_count - 1 && volume.is_empty_at(ix, iy + 1, iz))) 
            {
                for (auto &v : face_verts(Side::Top, ix, iy, iz, 1.0))
                    verts.push_back(v);
            }

            if (iz == 0 || (iz > 0 && volume.is_empty_at(ix, iy, iz - 1))) 
            {
                for (auto &v : face_verts(Side::Back, ix, iy, iz, 1.0))
                    verts.push_back(v);
            }
            if (iz == z_count - 1 
                || (iz < z_count - 1 && volume.is_empty_at(ix, iy, iz + 1))) 
            {
                for (auto &v : face_verts(Side::Front, ix, iy, iz, 1.0))
                    verts.push_back(v);
            }
        }

        // Can do without a copy if not using std::array
        this->verts = (float *)malloc(verts.size() * sizeof(float));
        this->verts_count = verts.size() / VERT_STRIDE;
        this->vert_stride = VERT_STRIDE;

        std::copy_n(verts.data(), verts.size(), this->verts);
    };

    FaceVertArray face_verts(GFX::Side side, int x, int y, int z, float uv_scale)
    {
        float cx = vox_sz * (float)x;
        float cy = vox_sz * (float)y;
        float cz = vox_sz * (float)z;

        switch (side) 
        {
            case Side::Back:
                return {
                    // Position                                                       // Normals         // UV 
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 0.0f, -1.0f, 0.0f,     0.0f,
                    cx + (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 0.0f, -1.0f, uv_scale, 0.0f,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 0.0f, -1.0f, uv_scale, uv_scale,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 0.0f, -1.0f, uv_scale, uv_scale,
                    cx - (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 0.0f, -1.0f, 0.0f,     uv_scale,
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 0.0f, -1.0f, 0.0f,     0.0f,
                };
            case Side::Front:
                return {
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 0.0f, 1.0f, 0.0f,     0.0f,
                    cx + (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 0.0f, 1.0f, uv_scale, 0.0f,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 0.0f, 1.0f, uv_scale, uv_scale,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 0.0f, 1.0f, uv_scale, uv_scale,
                    cx - (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 0.0f, 1.0f, 0.0f,     uv_scale,
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 0.0f, 1.0f, 0.0f,     0.0f,
                };
            case Side::Left:
                return {
                    cx - (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), -1.0f, 0.0f, 0.0f, uv_scale, 0.0f,
                    cx - (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz - (vox_sz / 2.0f), -1.0f, 0.0f, 0.0f, uv_scale, uv_scale,
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), -1.0f, 0.0f, 0.0f, 0.0f,     uv_scale,
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), -1.0f, 0.0f, 0.0f, 0.0f,     uv_scale,
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz + (vox_sz / 2.0f), -1.0f, 0.0f, 0.0f, 0.0f,     0.0f,
                    cx - (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), -1.0f, 0.0f, 0.0f, uv_scale, 0.0f,
                };
            case Side::Right:
                return {
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 1.0f, 0.0f, 0.0f, uv_scale, 0.0f,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 1.0f, 0.0f, 0.0f, uv_scale, uv_scale,
                    cx + (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 1.0f, 0.0f, 0.0f, 0.0f,     uv_scale,
                    cx + (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 1.0f, 0.0f, 0.0f, 0.0f,     uv_scale,
                    cx + (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 1.0f, 0.0f, 0.0f, 0.0f,     0.0f,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 1.0f, 0.0f, 0.0f, uv_scale, 0.0f,
                };
            case Side::Bottom:
                return {
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, -1.0f, 0.0f, 0.0f,     uv_scale,
                    cx + (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, -1.0f, 0.0f, uv_scale, uv_scale,
                    cx + (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, -1.0f, 0.0f, uv_scale, 0.0f,
                    cx + (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, -1.0f, 0.0f, uv_scale, 0.0f,
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, -1.0f, 0.0f, 0.0f,     0.0f,
                    cx - (vox_sz / 2.0f), cy - (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, -1.0f, 0.0f, 0.0f,     uv_scale,
                };
            case Side::Top:
                return {
            		cx - (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 1.0f, 0.0f, 0.0f,     uv_scale,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 1.0f, 0.0f, uv_scale, uv_scale,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 1.0f, 0.0f, uv_scale, 0.0f,
                    cx + (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 1.0f, 0.0f, uv_scale, 0.0f,
                    cx - (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz + (vox_sz / 2.0f), 0.0f, 1.0f, 0.0f, 0.0f,     0.0f,
                    cx - (vox_sz / 2.0f), cy + (vox_sz / 2.0f), cz - (vox_sz / 2.0f), 0.0f, 1.0f, 0.0f, 0.0f,     uv_scale,
                };
        }
    };
};

}
