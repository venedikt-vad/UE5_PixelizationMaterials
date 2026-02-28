# UE5 Pixelization/Dithering PostProcess
Unreal Engine 5 Plugin for pixelization / dithering / reduced pallete

This plugin allows you to add old-school pixelated style to your Unreal Engine project. 
Plugin adds post process material with next features: Pixelization, Reduced color palette, Custom color palette, Dithering, Different dithering patterns, Partial pixelization.

[Preview video](https://youtu.be/g4vheV6iv_w?si=eBmYgtOtw-mkUPmQ) <br/>
[itch.io page](https://venediktvad.itch.io/ue5-pixelizationdithering-postprocess)

<details>
  <summary>How to install</summary>
  
  1. Download plugin from [releases](https://github.com/venedikt-vad/UE5_PixelizationMaterials/releases) or from [itch.io](https://venediktvad.itch.io/ue5-pixelizationdithering-postprocess)
  2. Extract archive and place in your prroject's Plugins directory. `YourProject\Plugins\PixelizationMaterials` (make sure you don't create additional subdirectories when extracting)
  3. If your UnrealEngine version differs from one that plugin was build for, you may need to rebuild your project from visual studio.
</details>

<details>
  <summary>How to use</summary>

  1. Place `post process volume` in your scene <img width="974" height="789" alt="image" src="https://github.com/user-attachments/assets/c44210e8-c3b8-4a6c-b786-c4f205321af6" />
  2. Navigate to `Plugins/PixelizationMaterials/Materials/PostProcess` (if you don’t see plugins directory, you need to enable its visibility in content drawer settings) <img width="974" height="304" alt="image" src="https://github.com/user-attachments/assets/7fdc3b77-5102-4328-9e10-e42ed50b2c17" />
  3. Create `Material Instance` of `PP_Pixelate` <br/><img width="256" height="516" alt="image" src="https://github.com/user-attachments/assets/5eee4d6d-c336-41a9-952d-0807018a2f19" />
  4. Apply your instance to post process volume by adding it to `Post Process Materials` array <img width="974" height="766" alt="image" src="https://github.com/user-attachments/assets/7e02f653-c7c0-48b0-8ae8-5304e0dc91ac" />

</details>

<details>
  <summary>Material Parameters</summary>

  | Name                    | Description           |  Example |
  |-------------------------|-----------------------|----------|
  | -**Blur**-              |                       |    |
  | BlurAmount              |                       |         |
  | UseBlur                 |                       |         |
  | -**ColorSpace**-        |                           |          |
  | EnableRedusedColorSpace |       |                             |
  | CustomPalette           |         |    |
  | DitherColorLUT          |        |    |
  | -**Dithering**-         |  |        |
  | UseDithering            | |         |
  | DitherPattern           |  |        |
  | Pattern Steps           |   |      |
  | Pattern Width           |   |     |
  | InvertDithering         |  |       |
  | DitherAmount            |   |         |
  | -**Pixelization**-      |         | |
  | PixelInPeercents        |         | |
  | TrianglePixels          |        |  |
  | PixelAmount             |        |  |
  | X-Stretch               |        |  |
  | Y-Stretch               |        |  |
</details>
