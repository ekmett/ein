/** \file

      \license
        SPDX-FileType: Source
        SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
        SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
      \endlicense

      \brief numerics external template instantiations

      \ingroup numerics */


module;

#include "prelude.hpp"

using namespace std;

module ein.numerics;

namespace ein {

template bool cmp_unord(float, float) noexcept;
template bool cmp_unord(double, double) noexcept;
template bool cmp_ord(float, float) noexcept;
template bool cmp_ord(double, double) noexcept;

#define X
#include "numerics.x"
#undef X

}
