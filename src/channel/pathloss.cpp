#include <channel/pathloss.hpp>

PathLossModel::PathLossModel(terrain_t terrain, model_t model, double fc, double htx, double hrx) :
        m_terrain(terrain),
        m_model(model),
        m_fc(fc),
        m_htx(htx),
        m_hrx(hrx)
{
}

double PathLossModel::getPathLossForDistance(double dist) const
{
    constexpr double d0 = 100.0;
    double PLf = 0.0;
    double PLh = 0.0;

    switch(m_model) {
        case ATT:
            PLf =  6.0 * log10(m_fc/2e9); // Eq 1.13
            PLh = -10.8 * log10(m_hrx / 2.0); // Eq 1.14
            break;
        case OKUMURA:
            PLf = 6.0 * log10(m_fc/2e9);
            if (m_hrx <= 3.0) {
                PLh = -10.0 * log10(m_hrx / 3.0); //Eq 1.15
            } else {
                PLh = -20.0 * log10(m_hrx / 3.0);
            }
            break;
        case NULL_MODEL:
        default:
            break;
    }
    double a, b, c;
    switch(m_terrain) {
        case TERRAIN_A:
            a = 4.6;
            b = 0.0075;
            c = 12.6;
            break;
        case TERRAIN_B:
            a = 4.0;
            b = 0.0065;
            c = 17.1;
            break;
        case TERRAIN_C:
            a = 3.6;
            b = 0.005;
            c = 20.0;
        default:
            break;
    }
    double lambda = 3e8/m_fc;
    double gamma = a - b*m_htx + c/m_htx;
    double d0_pr = d0 * pow(10,-((PLf+PLh)/(10*gamma))); // Eq.(1.17)

    if (dist > d0_pr) {
        double A = 20 * log10(4 * M_PI * d0_pr / lambda) + PLf + PLh;
        return A + 10 * gamma * log10(dist/d0); // Eq.(1.18)
    } else {
        return 20 * log10(4 * M_PI * dist / lambda);
    }
}
