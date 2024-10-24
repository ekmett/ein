#pragma once

/**
    \defgroup headers headers
    \file
    \ingroup headers
    \license
    SPDX-FileType: Source
    SPDX-FileCopyrightText: 2024 Edward Kmett <ekmett@gmail.com>
    SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
    \endlicense
    \brief precompiled header
    \details
    this file contains the same files as the precompiled header would
    but clangd can't handle precompiled headers and c++ modules together
    so we just include the files we need directly when running in that mode.

    we'd just make this the precompiled header, but then cmake would fail
    to rebuild it when we change any of the local header files we include here.
    so instead we tell cmake about each of our headers in turn, and only use
    this header when not running in that mode.

    \hideincludedbygraph \hideincludegraph
*/

#include "third-party.hpp"
#include "attributes.hpp" // EIN(..)
#include "assert.hpp" // ein_assert(...)
