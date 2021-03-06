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

#define R_SCHW 2

static inline __device__ State ic(const size_t i, size_t n, const real t)
{
  State s = {0};

  size_t m = (size_t)SQRT((real)n); // round down
  while(n % m) --m;
  n /= m;

  // Photon position and momentum in spherical coordinates
  real kphi, alpha, beta;
  {
    const real deg2rad = M_PI / 180;
    const real cos_obs = COS(deg2rad * c.i_obs);
    const real sin_obs = SIN(deg2rad * c.i_obs);
    const real half    = .5;

    real x, y, z;

    alpha =  (i % n + half) / n - half;
    beta  = ((i / n + half) / m - half) * m / n;

    x = c.r_obs * sin_obs - (c.imgy0 + c.imgsz * beta) * cos_obs;
    y =                      c.imgx0 + c.imgsz * alpha          ;
    z = c.r_obs * cos_obs + (c.imgy0 + c.imgsz * beta) * sin_obs;

    const real R2 = x * x + y * y;

    s.r     = SQRT(R2 + z * z);
    s.theta = ACOS(z / s.r);
    s.phi   = ATAN2(y, x);

    if(s.phi >= M_PI) s.phi -= 2 * M_PI;
    if(s.phi <  M_PI) s.phi += 2 * M_PI;

    s.kr     = c.r_obs / s.r;
    s.ktheta = (s.kr * z / s.r - cos_obs) / SQRT(R2);
      kphi   = -sin_obs * y / R2;
  }

  // Impact parameter = L / E
  {
    real g00, g11, g22, g33, g30;
    {
      real tmp, s2, r2, a2, sum;

      tmp = SIN(s.theta);
      s2  = tmp * tmp;
      r2  = s.r * s.r;
      a2  = c.a_spin * c.a_spin;

      sum = r2 + a2;
      g22 = sum - a2 * s2; // = r2 + a2 * [cos(theta)]^2 = Sigma
      g11 = g22 / (sum - R_SCHW * s.r);

      tmp = R_SCHW * s.r / g22;
      g00 = tmp - 1;
      g30 = -c.a_spin * tmp * s2;
      g33 = (sum - c.a_spin * g30) * s2;
    }

    real E, kt;
    {
      real g30_kphi = g30 * kphi;
      real Delta    = g30_kphi * g30_kphi - g00 * (g11 * s.kr     * s.kr     +
                                                   g22 * s.ktheta * s.ktheta +
                                                   g33 *   kphi   *   kphi  );
      E  = SQRT(Delta); // = -k_t
      kt = -(g30_kphi + E) / g00;
    }

    s.bimpact = -(g33 * kphi + g30 * kt) / (g00 * kt + g30 * kphi);
    s.kr     /= E; // so that E = 1
    s.ktheta /= E; // so that E = 1
  }

  for(int i = 0; i < N_NU; ++i)
    s.tau[i] = s.I[i] = 0;

  return s;
}
