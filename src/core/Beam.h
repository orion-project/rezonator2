#ifndef BEAM_H
#define BEAM_H

namespace Z {

enum class BeamType
{
    RAY_VECTOR,
    GAUSS,
};

struct BeamProps
{
    BeamType type; ///< Beam approximation.
    double beamRadius; ///< Beam rdius.
    double frontRadius; ///< Wavefront ROC.
    double halfAngle; ///< Half of divergence angle.
};

} // namespace Z

#endif // BEAM_H
