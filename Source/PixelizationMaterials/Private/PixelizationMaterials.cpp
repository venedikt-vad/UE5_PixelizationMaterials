// Copyright Epic Games, Inc. All Rights Reserved.

#include "PixelizationMaterials.h"

#define LOCTEXT_NAMESPACE "FPixelizationMaterialsModule"

void FPixelizationMaterialsModule::StartupModule() {
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FPixelizationMaterialsModule::ShutdownModule() {
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPixelizationMaterialsModule, PixelizationMaterials)