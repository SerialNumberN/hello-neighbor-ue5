# Hello Neighbor: UE4 to UE5 Migration Guide

This document outlines the first steps and overall strategy for migrating the original *Hello Neighbor* assets (from the Mod Kit) into this Unreal Engine 5 project.

## 1. Initial Strategy: A 1:1 Port
Before leveraging any of Unreal Engine 5's advanced features, the initial goal is to achieve a stable 1:1 port of the game. This means getting the game running in UE5 exactly as it did in UE4. This approach ensures a solid foundation and helps isolate any compatibility issues between the engine versions before introducing new complexities.

## 2. Extracting Assets from the Mod Kit
Since you have the Hello Neighbor Mod Kit from the Epic Games Store, this will be your primary source for the game's original assets (meshes, textures, materials, blueprints, audio).

**Steps:**
1. **Install the Mod Kit:** Download and install the Hello Neighbor Mod Kit via the Epic Games Launcher.
2. **Open the Mod Kit in UE4:** Launch the Mod Kit. It will likely open in a specific version of Unreal Engine 4 (check the Epic Games Launcher for the exact version, often 4.2X).
3. **Locate the Content:** In the UE4 Editor's Content Browser, you will find all the game's assets organized into folders.
4. **Migrate to UE5:**
   - *Do not just copy the `.uasset` files directly through your operating system's file explorer.* This often breaks references.
   - Instead, inside the UE4 Editor (Mod Kit), select the root folder(s) containing the assets you want to move (e.g., the main game folder, characters, environments).
   - Right-click the selected folder(s) -> **Asset Actions** -> **Migrate...**
   - The Asset Report window will appear, showing all dependencies. Click **OK**.
   - A file browser will prompt you for a destination. Navigate to the `Content` folder of *this* new UE5 project (`HelloNeighborUE5/Content`) and select it.
   - UE4 will package and transfer the assets along with their correct folder structure and internal references to your UE5 project.

## 3. First Launch in UE5
Once the migration process from the UE4 Mod Kit is complete:
1. Open this `HelloNeighborUE5.uproject` in Unreal Engine 5 (version 5.3 or later recommended).
2. **Expect Errors:** The first time you open the project, UE5 will need to recompile all shaders and update blueprints. You will likely encounter warnings and errors related to deprecated UE4 nodes, changed physics systems (PhysX to Chaos), or lighting settings.
3. **Fixing Blueprints:** Go through the Output Log and fix any broken Blueprint nodes. Many nodes have updated equivalents in UE5.
4. **Physics (Chaos):** UE5 uses the Chaos Physics engine instead of PhysX. You may need to adjust physical materials, collision settings, and any custom physics logic to work correctly with Chaos.

## 4. Enhancing with UE5 Features
After achieving a stable 1:1 port where the core game runs without major errors, we can begin implementing UE5 features to enhance the game. *Note: Basic configurations for these are already included in the `Config/DefaultEngine.ini`.*

### A. Lumen (Global Illumination & Reflections)
Lumen provides fully dynamic global illumination and reflections.
- **Implementation:** Replace the baked lighting (Lightmass) setup from UE4. Remove reflection captures if they are no longer needed. Ensure materials are physically based (PBR) to interact correctly with Lumen.

### B. Nanite (Virtualized Geometry)
Nanite allows for incredibly detailed meshes without traditional LOD (Level of Detail) setups or polycount budgets.
- **Implementation:** Enable Nanite on static meshes (especially complex environmental assets). This can be done in bulk via the Content Browser (Right-click -> Nanite -> Enable). *Note: Nanite does not currently support skeletal meshes (characters) or materials with certain types of deformation/translucency in standard setups.*

### C. World Partition (Optional but Recommended for Large Maps)
If the game world is large, World Partition is UE5's modern solution for level streaming.
- **Implementation:** Convert the existing level(s) to use World Partition. This will break the map into a grid and automatically stream cells in and out based on camera distance, improving performance.

---
**Next Steps:** Begin the migration process using the Mod Kit as described in Section 2. Once the assets are in this project's `Content` folder, we can start addressing the inevitable compile errors and upgrading the systems.
