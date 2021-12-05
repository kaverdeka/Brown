//
// Created by ka on 04.12.2021.
//

#pragma once

#include "Common.h"

#include <memory>

std::unique_ptr<ITexture> MakeTextureSolid(Size size, char pixel);

std::unique_ptr<ITexture> MakeTextureCheckers(Size size, char pixel1,
                                              char pixel2);

std::unique_ptr<ITexture> MakeTextureCow();