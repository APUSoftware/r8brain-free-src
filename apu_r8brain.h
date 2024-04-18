//
// File: apu_r8brain.h
// Desc: Module declaration for apu_r8brain
//
// Copyright (C) APU Software, LLC <caustik@apu.software> - All Rights Reserved.
// Unauthorized copying and/or usage of this file is strictly prohibited.
// Proprietary and confidential.
//

/*******************************************************************************

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                 apu_r8brain
  vendor:             caustik
  version:            0.5.0
  name:               r8brain module
  description:        Wrapper around r8brain free sample rate converter
  website:            https://apu.software/
  license:            Commercial

  dependencies:       juce_gui_extra

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#ifndef APU_R8BRAIN_H
#define APU_R8BRAIN_H

// r8brain configuration
#define R8B_FASTTIMING 1
#define R8B_EXTFFT 1
#define R8B_PFFFT_DOUBLE 1

// project includes
#include "CDSPResampler.h"

#endif