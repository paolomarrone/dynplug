/*
 * Dynplug
 *
 * Copyright (C) 2022 Orastron Srl unipersonale
 *
 * Copyright is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Copyright is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Copyright.  If not, see <http://www.gnu.org/licenses/>.
 *
 * File authors: Paolo Marrone, Stefano D'Angelo
 */

#ifndef _VST3_COMMON_H
#define _VST3_COMMON_H

#include "config.h"

#if defined(P_PITCH_BEND)
# define TAG_PITCH_BEND		NUM_PARAMETERS
# if defined(P_MOD_WHEEL)
#  define TAG_MOD_WHEEL		(NUM_PARAMETERS + 1)
# endif
#elif defined(P_MOD_WHEEL)
# define TAG_MOD_WHEEL		NUM_PARAMETERS
#endif

#endif
