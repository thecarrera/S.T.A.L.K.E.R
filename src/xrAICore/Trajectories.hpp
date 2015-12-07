#pragma once

#include "xrCore/xrCore.h"
#include "xrGame/GameObject.h"

struct trajectory_pick
{
    Fvector center;
    Fvector z_axis;
    Fvector y_axis;
    Fvector x_axis;
    Fvector sizes;
    u8 invert_x;
    u8 invert_y;
    u8 invert_z;
};

bool trajectory_intersects_geometry(
    float trajectory_time,
    const Fvector &trajectory_start,
    const Fvector &trajectory_end,
    const Fvector &trajectory_velocity,
    Fvector &collide_position,
    IGameObject *self_object,
    IGameObject *ignored_object,
    collide::rq_results &temp_rq_results,
    xr_vector<trajectory_pick> *out_trajectory_picks,
    xr_vector<Fvector> *out_collide_tris,
    const Fvector &box_size); // can be zero - ray is traced then
