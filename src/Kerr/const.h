// Copyright (C) 2012--2014 Chi-kwan Chan
// Copyright (C) 2012--2014 Steward Observatory
//
// This file is part of GRay.
//
// GRay is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GRay is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GRay.  If not, see <http://www.gnu.org/licenses/>.

#ifndef CONST_H
#define CONST_H

#include "harm.h"

#define DT_DUMP (-64)
#define N_R     264 // large enough to hold our grids
#define N_RS    8   // number of cells to skip near the outer boundary
#define N_NU    4

typedef struct {
  // Parameters for geodesic integration
  real imgsz;     // image size in GM/c^2
  real imgx0;     // the location of alpha-origin
  real imgy0;     // the location of beta-origin
  real r_obs;     // observer radius in GM/c^2
  real i_obs;     // observer theta in degrees
  real j_obs;     // observer phi in degrees
  real a_spin;    // dimensionless spin j/mc
  real dt_scale;  // typical step size
  real epsilon;   // stop photon
  real tolerance; // if xi+1 > tolerance, fall back to forward Euler

  // Parameters for radiative transfer
  real   m_BH, ne_rho, threshold, tgas_max, Ti_Te_d, Ti_Te_f;
  real   nu0[N_NU];
  size_t n_nu;

  Coord *coord;
  Field *field;
  size_t n_rx, n_r, n_theta, n_phi;
  real   Gamma;
  float  r[N_R];
} Const;

namespace harm {
  extern Coord *load_coord(Const &, const char *);
  extern Field *load_field(Const &, const char *);
  extern bool   load      (Const &, const char *);
  extern bool   setx      (Const &, const char *);
  extern bool   using_harm;
  extern size_t n_nu;
}

#endif // CONST_H
