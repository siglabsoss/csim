#pragma once

#include <cmath>

class PathLossModel
{
public:
    enum terrain_t
    {
        TERRAIN_A = 0,
        TERRAIN_B,
        TERRAIN_C
    };
    enum model_t
    {
        NULL_MODEL = 0,
        ATT,
        OKUMURA
    };
    PathLossModel(terrain_t terrain, model_t model, double fc, double htx, double hrx);
    double getPathLossForDistance(double dist) const;
private:
    terrain_t           m_terrain;  //terrain type
    model_t             m_model;    //pathloss model
    double              m_fc;       //carrier frequency
    double              m_htx;      //tx height
    double              m_hrx;      //rx height
};
