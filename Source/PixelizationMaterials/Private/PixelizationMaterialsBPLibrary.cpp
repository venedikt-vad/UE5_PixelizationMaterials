// Copyright Epic Games, Inc. All Rights Reserved.

#include "PixelizationMaterialsBPLibrary.h"
#include "PixelizationMaterials.h"




UPixelizationMaterialsBPLibrary::UPixelizationMaterialsBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FLinearColor UPixelizationMaterialsBPLibrary::ConvertHexToColor(const FString& HexCode) {
    FString ProcessedHexCode = HexCode.Replace(TEXT("#"), TEXT(""));
    
    if (ProcessedHexCode.Len() == 6) ProcessedHexCode += TEXT("FF");
    if (ProcessedHexCode.Len() != 8) return FLinearColor::Transparent;

    uint8 R = FParse::HexDigit(ProcessedHexCode[0]) * 16 +
        FParse::HexDigit(ProcessedHexCode[1]);
    uint8 G = FParse::HexDigit(ProcessedHexCode[2]) * 16 +
        FParse::HexDigit(ProcessedHexCode[3]);
    uint8 B = FParse::HexDigit(ProcessedHexCode[4]) * 16 +
        FParse::HexDigit(ProcessedHexCode[5]);
    uint8 A = FParse::HexDigit(ProcessedHexCode[6]) * 16 +
        FParse::HexDigit(ProcessedHexCode[7]);

    return FLinearColor::FromSRGBColor(FColor(R, G, B, A));
}



FString UPixelizationMaterialsBPLibrary::OpenFileDialog() {

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

    FString filePath;
    if (DesktopPlatform) {
        uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
        TArray<FString> outputPaths;
        if (!DesktopPlatform->OpenFileDialog(nullptr, "Select a Palette File", "", FString(""), "palettes|*.txt;*.pal;*.gpl;*.ase;*.hex", SelectionFlag, outputPaths)) return "";
        if (outputPaths.IsEmpty()) return"";
        filePath = *outputPaths.GetData();
    }

    return filePath;
}

float ReverseFloat(const float inFloat) {
    float retVal;
    char* floatToConvert = (char*)&inFloat;
    char* returnFloat = (char*)&retVal;

    // swap the bytes into a temporary buffer
    returnFloat[0] = floatToConvert[3];
    returnFloat[1] = floatToConvert[2];
    returnFloat[2] = floatToConvert[1];
    returnFloat[3] = floatToConvert[0];

    return retVal;
}

bool UPixelizationMaterialsBPLibrary::ReadPalleteFromFile(TArray<FLinearColor>& Palette, FString& PalleteName) {
    FString filePath = OpenFileDialog();

    FString fileContent = "";
    if (!FFileHelper::LoadFileToString(fileContent, *filePath)) return false;

    TArray64<uint8> fileBYTES;
    FFileHelper::LoadFileToArray(fileBYTES, *filePath);
    
    FPalleteFileType fileType = FPalleteFileType::HEX;
    int n = 0;
    filePath.FindLastChar('/', n);
    PalleteName = filePath.RightChop(n+1);
    filePath.FindLastChar('.', n);
    //FString fileEXT = filePath.RightChop(n+1);
    PalleteName = filePath.LeftChop(n-1);

    TArray<FString> content;
    fileContent.ParseIntoArrayLines(content);
    if (content.IsEmpty()) return false;

    if(content[0].Contains("JASC")) fileType = FPalleteFileType::JASC;
    if(content[0].Contains("ASEF")) fileType = FPalleteFileType::ASEF;
    if(content[0].Contains("GIMP")) fileType = FPalleteFileType::GIMP;
    if(content[0].Contains("paint.net")) fileType = FPalleteFileType::PaintNET;

    switch (fileType) {
    case JASC:
        for (size_t i = 3; i < content.Num(); i++) {
            FString delim = " ";
            TArray<FString> arrCol;
            content[i].ParseIntoArrayWS(arrCol, *delim);
            uint8 R = FCString::Atoi(*arrCol[0]);
            uint8 G = FCString::Atoi(*arrCol[1]);
            uint8 B = FCString::Atoi(*arrCol[2]);

            FLinearColor newColor = FLinearColor::FromSRGBColor(FColor(R, G, B));
            Palette.AddUnique(newColor);
        }
        break;
    case GIMP:
        for (size_t i = 1; i < content.Num(); i++) {
            if (content[i].Contains("#")) {
                if(content[i].Contains("Palette Name: ")){
                    int m = -1;
                    content[i].FindChar(':', m);
                    PalleteName = content[i].RightChop(m+2);
                }
                continue;
            }
            FString delim = "\t";
            TArray<FString> arrCol;
            content[i].ParseIntoArrayWS(arrCol, *delim);
            uint8 R = FCString::Atoi(*arrCol[0]);
            uint8 G = FCString::Atoi(*arrCol[1]);
            uint8 B = FCString::Atoi(*arrCol[2]);

            FLinearColor newColor = FLinearColor::FromSRGBColor(FColor(R, G, B));
            Palette.AddUnique(newColor);
        }
        break;
    case PaintNET:
        for (size_t i = 1; i < content.Num(); i++) {
            if (content[i].Contains(";")) {
                if (content[i].Contains("Palette Name: ")) {
                    int m = -1;
                    content[i].FindChar(':', m);
                    PalleteName = content[i].RightChop(m + 2);
                }
                continue;
            }
            Palette.AddUnique(ConvertHexToColor(content[i].RightChop(2)));
        }
        break;
    case ASEF:
        if (fileContent.FindChar('"', n)) {
            while (fileContent.FindChar('"', n)) {
                FString temp = "";
                temp.AppendChar(fileContent[n + 4]);
                temp.AppendChar(fileContent[n + 6]);
                temp.AppendChar(fileContent[n + 8]);
                temp.AppendChar(fileContent[n + 10]);
                temp.AppendChar(fileContent[n + 12]);
                temp.AppendChar(fileContent[n + 14]);
                Palette.AddUnique(ConvertHexToColor(temp));
                fileContent = fileContent.RightChop(n + 1);
            }
        }

        if (Palette.IsEmpty()) {
            UE_LOG(LogTemp, Warning, TEXT("Searching for RGB in ASEF File"));
            
            for (int i = 0; i < fileBYTES.Num(); i++) {
                //82 71 66 32

                if (fileBYTES[i] != 82)continue;
                if (fileBYTES[i+1] != 71)continue;
                if (fileBYTES[i+2] != 66)continue;
                if (fileBYTES[i+3] != 32)continue;
                UE_LOG(LogTemp, Warning, TEXT("Found color!"));

                float r;
                float g;
                float b;


                {
                    TArray<uint8> Bytes;
                    UE_LOG(LogTemp, Warning, TEXT("%d %d %d %d"), fileBYTES[i + 4], fileBYTES[i + 5], fileBYTES[i + 6], fileBYTES[i + 7]);
                    Bytes.Add(fileBYTES[i + 4]);
                    Bytes.Add(fileBYTES[i + 5]);
                    Bytes.Add(fileBYTES[i + 6]);
                    Bytes.Add(fileBYTES[i + 7]);
                    r = *reinterpret_cast<float*>(Bytes.GetData());
                    i += 4;
                }
                {
                    TArray<uint8> Bytes;
                    Bytes.Add(fileBYTES[i + 4]);
                    Bytes.Add(fileBYTES[i + 5]);
                    Bytes.Add(fileBYTES[i + 6]);
                    Bytes.Add(fileBYTES[i + 7]);
                    g = *reinterpret_cast<float*>(Bytes.GetData());
                    i += 4;
                }
                {
                    TArray<uint8> Bytes;
                    Bytes.Add(fileBYTES[i + 4]);
                    Bytes.Add(fileBYTES[i + 5]);
                    Bytes.Add(fileBYTES[i + 6]);
                    Bytes.Add(fileBYTES[i + 7]);
                    b = *reinterpret_cast<float*>(Bytes.GetData());
                }

                r = ReverseFloat(r);
                g = ReverseFloat(g);
                b = ReverseFloat(b);

                UE_LOG(LogTemp, Warning, TEXT("R: %f G: %f B: %f"), r, g, b);

                uint8 R = r * 255;
                uint8 G = g * 255;
                uint8 B = b * 255;
                Palette.AddUnique(FLinearColor::FromSRGBColor(FColor(R, G, B)));
            }
         
        }

        
        break;
    case HEX:
    default:
        for (FString line: content) {
            Palette.AddUnique(ConvertHexToColor(line));
        }
        break;
    }

    return true;
}

//----ColorSpaceConvertions

FVector UPixelizationMaterialsBPLibrary::HSVposition(FLinearColor HSV) {
    FVector pos = FVector(HSV.G, 0, HSV.B);
    return pos.RotateAngleAxis(HSV.R, FVector::UpVector);
}

FLinearColor UPixelizationMaterialsBPLibrary::positionHSV(FVector HSV) {
    float H = FVector::VectorPlaneProject(HSV, FVector::UpVector).Rotation().Yaw;
    if (H < 0) H += 360;
    float S = FVector::VectorPlaneProject(HSV, FVector::UpVector).Length();
    float V = HSV.Z;

    return FLinearColor(H, S, V);
}

FVector UPixelizationMaterialsBPLibrary::sRGBToXYZcolor(FColor color) {
    //sR, sG and sB (Standard RGB) input range = 0 ÷ 255
    //X, Y and Z output refer to a D65/2° standard illuminant.

    float var_R = color.R / 255.;
    float var_G = color.G / 255.;
    float var_B = color.B / 255.;

    if (var_R > 0.04045) var_R = pow((var_R + 0.055) / 1.055, 2.4);
    else                 var_R = var_R / 12.92;
    if (var_G > 0.04045) var_G = pow((var_G + 0.055) / 1.055, 2.4);
    else                 var_G = var_G / 12.92;
    if (var_B > 0.04045) var_B = pow((var_B + 0.055) / 1.055, 2.4);
    else                 var_B = var_B / 12.92;

    var_R = var_R * 100;
    var_G = var_G * 100;
    var_B = var_B * 100;

    float X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
    float Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
    float Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

    return FVector(X, Y, Z);
}

FVector UPixelizationMaterialsBPLibrary::XYZcolorToCIELUV(FVector XYZcolor) {
    //ReferenceX, Y and Z refer to specific illuminants and observers.
    const float ReferenceX = 95.047;
    const float ReferenceY = 100.000;
    const float ReferenceZ = 108.883;

    float X = XYZcolor.X;
    float Y = XYZcolor.Y;
    float Z = XYZcolor.Z;

    float var_U = (4. * X) / (X + (15. * Y) + (3. * Z));
    float var_V = (9. * Y) / (X + (15. * Y) + (3. * Z));

    float var_Y = Y / 100.;
    if (var_Y > 0.008856) var_Y = pow(var_Y, (1. / 3.));
    else                  var_Y = (7.787 * var_Y) + (16. / 116.);

    float ref_U = (4. * ReferenceX) / (ReferenceX + (15. * ReferenceY) + (3. * ReferenceZ));
    float ref_V = (9. * ReferenceY) / (ReferenceX + (15. * ReferenceY) + (3. * ReferenceZ));

    float CIE_L = (116. * var_Y) - 16.;
    float CIE_u = 13. * CIE_L * (var_U - ref_U);
    float CIE_v = 13. * CIE_L * (var_V - ref_V);

    return FVector(CIE_L, CIE_u, CIE_v);
}

FVector UPixelizationMaterialsBPLibrary::sRGBToCIELUV(FColor color) {
    return XYZcolorToCIELUV(sRGBToXYZcolor(color));
}

FVector UPixelizationMaterialsBPLibrary::CIELUVToXYZcolor(FVector CIELUV) {
    //Reference-X, Y and Z refer to specific illuminants and observers.
    const float ReferenceX = 95.047;
    const float ReferenceY = 100.000;
    const float ReferenceZ = 108.883;

    float CIE_L = CIELUV.X;
    float CIE_u = CIELUV.Y;
    float CIE_v = CIELUV.Z;

    float var_Y = (CIE_L + 16.) / 116.;
    if (pow(var_Y, 3) > 0.008856) var_Y = pow(var_Y, 3);
    else                          var_Y = (var_Y - 16. / 116.) / 7.787;

    float ref_U = (4 * ReferenceX) / (ReferenceX + (15 * ReferenceY) + (3 * ReferenceZ));
    float ref_V = (9 * ReferenceY) / (ReferenceX + (15 * ReferenceY) + (3 * ReferenceZ));

    float var_U = CIE_u / (13 * CIE_L) + ref_U;
    float var_V = CIE_v / (13 * CIE_L) + ref_V;

    float Y = var_Y * 100;
    float X = -(9 * Y * var_U) / ((var_U - 4) * var_V - var_U * var_V);
    float Z = (9 * Y - (15 * var_V * Y) - (var_V * X)) / (3 * var_V);

    return FVector(X, Y, Z);
}

FColor UPixelizationMaterialsBPLibrary::XYZcolorTosRGB(FVector XYZcolor) {
    //X, Y and Z input refer to a D65/2° standard illuminant.
    //sR, sG and sB (standard RGB) output range = 0 ÷ 255

    float X = XYZcolor.X;
    float Y = XYZcolor.Y;
    float Z = XYZcolor.Z;

    float var_X = X / 100.;
    float var_Y = Y / 100.;
    float var_Z = Z / 100.;

    float var_R = var_X * 3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
    float var_G = var_X * -0.9689 + var_Y * 1.8758 + var_Z * 0.0415;
    float var_B = var_X * 0.0557 + var_Y * -0.2040 + var_Z * 1.0570;

    if (var_R > 0.0031308) var_R = 1.055 * pow(var_R, (1. / 2.4)) - 0.055;
    else                   var_R = 12.92 * var_R;
    if (var_G > 0.0031308) var_G = 1.055 * pow(var_G, (1 / 2.4)) - 0.055;
    else                   var_G = 12.92 * var_G;
    if (var_B > 0.0031308) var_B = 1.055 * pow(var_B, (1 / 2.4)) - 0.055;
    else                   var_B = 12.92 * var_B;

    uint8 sR = var_R * 255;
    uint8 sG = var_G * 255;
    uint8 sB = var_B * 255;

    return FColor(sR, sG, sB);
}

FColor UPixelizationMaterialsBPLibrary::CIELUVTosRGB(FVector CIELUV) {
    return XYZcolorTosRGB(CIELUVToXYZcolor(CIELUV));
}

//----

FVector UPixelizationMaterialsBPLibrary::ConvertLinearColorToSpace(FLinearColor color, EColorSpace ColorSpace) {
    switch (ColorSpace) {
    case EColorSpace::RGB:
        return FVector(color);
        break;
    case EColorSpace::HSV:
        return HSVposition(color.LinearRGBToHSV());
        break;
    case EColorSpace::XYZ:
        return sRGBToXYZcolor(color.ToFColorSRGB());
        break;
    case EColorSpace::CIELUV:
        return sRGBToCIELUV(color.ToFColorSRGB());
        break;
    default:
        return FVector(color);
        break;
    }
}

FLinearColor UPixelizationMaterialsBPLibrary::ConvertSpaceToLinearColor(FVector color, EColorSpace ColorSpace) {
    switch (ColorSpace) {
    case EColorSpace::RGB:
        return FLinearColor(color);
        break;
    case EColorSpace::HSV:
        return positionHSV(color).HSVToLinearRGB();
        break;
    case EColorSpace::XYZ:
        return FLinearColor(XYZcolorTosRGB(color));
        break;
    case EColorSpace::CIELUV:
        return  FLinearColor(CIELUVTosRGB(color));
        break;
    default:
        return FLinearColor(color);
        break;
    }
}

FVector UPixelizationMaterialsBPLibrary::ConvertColorForSearch(FLinearColor color, EColorSpace colorSpace, EColorSearchType searchType) {
    if (colorSpace == EColorSpace::HSV && (searchType < 3)) {
        return ConvertLinearColorToSpace(color.LinearRGBToHSV(), EColorSpace::RGB) * FVector(1./360.,1.,1.);
    } else {
        return ConvertLinearColorToSpace(color, colorSpace);
    }
}

TArray<FVector> UPixelizationMaterialsBPLibrary::ConvertPaletteForSearch(TArray<FLinearColor> Palette, EColorSpace ColorSpace, EColorSearchType SearchType) {
        TArray<FVector> updatedPalette;
        for (FLinearColor color : Palette) {
            updatedPalette.Add(ConvertColorForSearch(color, ColorSpace, SearchType));
        }
        return updatedPalette;
}

FLinearColor UPixelizationMaterialsBPLibrary::ConvertColorFromSearch(FVector color, EColorSpace colorSpace, EColorSearchType searchType) {
    if (colorSpace == EColorSpace::HSV && (searchType < 3)) {
        return ConvertSpaceToLinearColor(color * FVector(360,1,1), EColorSpace::RGB).HSVToLinearRGB();
    } else {
        return ConvertSpaceToLinearColor(color, colorSpace);
    }
}

//----

void UPixelizationMaterialsBPLibrary::findClosestAndOffset(TArray<FVector> palette, FVector targetColor, FVector& colorA, FVector& colorB, float& blend) {
    float dist = UE_MAX_FLT;
    for (FVector color : palette) {
        if (FVector::Dist(targetColor, color) < dist) {
            dist = FVector::Dist(targetColor, color);
            colorA = color;
        }
    }

    dist = UE_MAX_FLT;
    FVector tgt = (targetColor - colorA).GetUnsafeNormal();
    for (FVector color : palette) {
        FVector offs = (color - colorA).GetUnsafeNormal();
        if (FVector::Dist(tgt, offs) < dist) {
            dist = FVector::Dist(tgt, offs);
            colorB = color;
        }
    }

    double angle = (targetColor - colorA).GetUnsafeNormal().Dot((colorB - colorA).GetUnsafeNormal());
    blend = ((targetColor - colorA).Length() * angle) / (colorB - colorA).Length();
}

void UPixelizationMaterialsBPLibrary::findClosestLine(TArray<FVector> palette, FVector targetColor, FVector& colorA, FVector& colorB, float& blend) {
    float dist = UE_MAX_FLT;
    for (FVector color_A : palette) {
        for (FVector color_B : palette) {
            float n_dist = FMath::PointDistToSegment(targetColor, color_A, color_B);
            if (n_dist < dist) {
                dist = n_dist;
                colorA = color_A;
                colorB = color_B;
            }
        }
    }

    //double angle = (targetColor - colorA).GetUnsafeNormal().Dot((colorB - colorA).GetUnsafeNormal());
    //blend = ((targetColor - colorA).Length() * angle) / (colorB - colorA).Length();

    blend = (targetColor - colorB).Length() / ((targetColor - colorA).Length() + (targetColor - colorB).Length());
}

void UPixelizationMaterialsBPLibrary::findClosestOnAxis(TArray<FVector> palette, FVector targetColor, EAxis::Type Axis, EColorSpace ColorSpace, FVector& colorA, FVector& colorB, float& blend) {
    float tgt = targetColor.GetComponentForAxis(Axis);

    float vMAX = -UE_MAX_FLT;
    float vMIN = UE_MAX_FLT;

    for (FVector color : palette) {
        float v = color.GetComponentForAxis(Axis);
        if (v > vMAX) {
            vMAX = v;
            colorB = color;
        }
        if (v < vMIN) {
            vMIN = v;
            colorA = color;
        }
    }

    float posA = vMIN;
    float posB = vMAX;

    for (FVector color : palette) {
        float v = color.GetComponentForAxis(Axis);
        bool vLess = v < tgt;
        if (ColorSpace == EColorSpace::HSV && Axis!=EAxis::X) vLess = (v / vMAX) < tgt;
        if(vLess) {
            if (v > posA) {
                posA = v;
                colorA = color;
            }
        } else {
            if (v < posB) {
                posB = v;
                colorB = color;
            }
        }
    }

    blend = (tgt - posA) / (posB - posA);
}

void UPixelizationMaterialsBPLibrary::findClosestSelectSearchType(TArray<FVector> palette, FVector targetColor, EColorSearchType searchType, EColorSpace ColorSpace, FVector& colorA, FVector& colorB, float& blend) {
    switch (searchType) {
    case ClosestOffset:
        findClosestAndOffset(palette, targetColor, colorA, colorB, blend);
        break;
    case ClosestLine:
        findClosestLine(palette, targetColor, colorA, colorB, blend);
        break;
    case ClosestX:
        findClosestOnAxis(palette, targetColor, EAxis::X, ColorSpace, colorA, colorB, blend);
        break;
    case ClosestY:
        findClosestOnAxis(palette, targetColor, EAxis::Y, ColorSpace, colorA, colorB, blend);
        break;
    case ClosestZ:
        findClosestOnAxis(palette, targetColor, EAxis::Z, ColorSpace, colorA, colorB, blend);
        break;
    default:
        findClosestAndOffset(palette, targetColor, colorA, colorB, blend);
        break;
    }
}






