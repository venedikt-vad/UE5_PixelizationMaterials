// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"
#include "Misc/FileHelper.h"

#include "PixelizationMaterialsBPLibrary.generated.h"


/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UENUM()
enum FPalleteFileType {
	JASC,
	GIMP,
	PaintNET,
	ASEF,
	HEX,
};

UENUM(BlueprintType)
enum EColorSpace {
	RGB,
	HSV,
	XYZ,
	CIELUV,
};

UENUM(BlueprintType)
enum EColorSearchType {
	ClosestOffset = 4,
	ClosestLine = 3,
	ClosestX = 0,
	ClosestY = 1,
	ClosestZ = 2,
};

UCLASS()
class UPixelizationMaterialsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Math | Color ")
    static FLinearColor ConvertHexToColor(const FString& HexCode);

	//UFUNCTION(BlueprintCallable, Category = "File IO")
	static FString OpenFileDialog();

	//UFUNCTION(BlueprintCallable, Category = "File IO")
	//static bool LoadStringFromFile(FString& Result, FString FilePath = "File Path") {
	//	return FFileHelper::LoadFileToString(Result, *FilePath);
	//}

	UFUNCTION(BlueprintCallable, Category = "Math | Color ")
	static bool ReadPalleteFromFile(TArray<FLinearColor>& Pallete, FString& PalleteName);

	//----ColorSpaceConvertions

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "HSV to position", ToolTip = "converts HSV color to position in imaginary 3D cylinder"))
	static FVector HSVposition(FLinearColor HSV);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "position to HSV", ToolTip = "converts position in imaginary 3D cylinder to HSV color"))
	static FLinearColor positionHSV(FVector HSV);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "RGB to XYZ color", ReturnDisplayName = "XYZ color"))
	static FVector sRGBToXYZcolor(FColor color);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "XYZ color to CIELUV", ReturnDisplayName = "CIELUV"))
	static FVector XYZcolorToCIELUV(FVector XYZcolor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "RGB to CIELUV", ReturnDisplayName = "CIELUV"))
	static FVector sRGBToCIELUV(FColor color);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "CIELUV to XYZ color", ReturnDisplayName = "XYZ color"))
	static FVector CIELUVToXYZcolor(FVector CIELUV);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color", meta = (DisplayName = "XYZ color to RGB", ReturnDisplayName = "RGB"))
	static FColor XYZcolorTosRGB(FVector XYZcolor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "CIELUV to RGB", ReturnDisplayName = "RGB"))
	static FColor CIELUVTosRGB(FVector CIELUV);

	//----

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "Linear color to space", ToolTip = "Convert linear color to desired color space"))
	static FVector ConvertLinearColorToSpace(FLinearColor color, EColorSpace ColorSpace);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (DisplayName = "Convert From space to linear color", ToolTip = "Convert from desired color space to linear color"))
	static FLinearColor ConvertSpaceToLinearColor(FVector color, EColorSpace ColorSpace);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (ToolTip = "Convert linear color palette to needed color space"))
	static FVector ConvertColorForSearch(FLinearColor color, EColorSpace colorSpace, EColorSearchType searchType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color | Palettes", meta = (ToolTip = "Convert linear color palette to needed color space"))
	static TArray<FVector> ConvertPaletteForSearch(TArray<FLinearColor> Palette, EColorSpace ColorSpace, EColorSearchType SearchType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color ", meta = (ToolTip = "Convert linear color palette to needed color space"))
	static FLinearColor ConvertColorFromSearch(FVector color, EColorSpace colorSpace, EColorSearchType searchType);
	//----

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color | Palettes", meta = (DisplayName = "Color selection: Find closest and offset", ToolTip = "colorA is closest to target Color, colorB is offset color"))
	static void findClosestAndOffset(TArray<FVector> palette, FVector targetColor, FVector& colorA, FVector& colorB, float& blend);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color | Palettes", meta = (DisplayName = "Color selection: Find closest line", ToolTip = "line between colorA and colorB is closest to target color"))
	static void findClosestLine(TArray<FVector> palette, FVector targetColor, FVector& colorA, FVector& colorB, float& blend);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color | Palettes", meta = (DisplayName = "Color selection: Closest on axis", ToolTip = "Selects the nearest A and B around target color on selected color axis"))
	static void findClosestOnAxis(TArray<FVector> palette, FVector targetColor, EAxis::Type Axis, EColorSpace ColorSpace, FVector& colorA, FVector& colorB, float& blend) ;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Math | Color | Palettes", meta = ( ToolTip = ""))
	static void findClosestSelectSearchType(TArray<FVector> palette, FVector targetColor, EColorSearchType searchType, EColorSpace ColorSpace, FVector& colorA, FVector& colorB, float& blend);
	//----

};

