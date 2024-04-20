cmake_minimum_required(VERSION 3.15)
project(Descent3LevelEditor)

## Set CMake policy
if(POLICY CMP0076)
  cmake_policy(SET CMP0076 NEW)
endif()

# Specify to use shared MFC libraries
set(CMAKE_MFC_FLAG 2)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Individual project inclusions
add_library(2dlib STATIC
  2dlib/font.cpp
  2dlib/hardsurf.cpp
  2dlib/memsurf.cpp
  2dlib/pen.cpp
  2dlib/pentext.cpp
  2dlib/screen.cpp
  2dlib/surface.cpp
  2dlib/viewport.cpp
)
target_include_directories(2dlib PRIVATE ${PROJECT_SOURCE_DIR}/2dlib)

add_library(3d STATIC
  3d/CLIPPER.cpp
  3d/DRAW.cpp
  3d/GLOBVARS.cpp
  3d/INSTANCE.cpp
  3d/POINTS.cpp
  3d/SETUP.cpp
)
target_include_directories(3d PRIVATE ${PROJECT_SOURCE_DIR}/3d)

add_library(bitmap STATIC
  bitmap/bitmain.cpp
  bitmap/bumpmap.cpp
  bitmap/iff.cpp
  bitmap/lightmap.cpp
  bitmap/pcx.cpp
  bitmap/tga.cpp
)
target_include_directories(bitmap PRIVATE ${PROJECT_SOURCE_DIR}/bitmap)

add_library(cfile STATIC
  cfile/CFILE.cpp
  cfile/hog.cpp
  cfile/InfFile.cpp
)
target_include_directories(cfile PRIVATE ${PROJECT_SOURCE_DIR}/cfile)

add_library(ddgr_win32 STATIC
  ddgr_win32/ddgrWin32API.cpp
  ddgr_win32/ddgrWin32DX.cpp
  ddgr_win32/ddgrWin32GDI.cpp
  ddgr_win32/ddgrWin32Init.cpp
)
target_include_directories(ddgr_win32 PRIVATE ${PROJECT_SOURCE_DIR}/ddgr_win32)

add_library(ddio_common STATIC
  ddio_common/ddio.cpp
  ddio_common/key.cpp
)
target_include_directories(ddio_common PRIVATE ${PROJECT_SOURCE_DIR}/ddio_common)

add_library(ddio_win STATIC
  ddio_win/winfile.cpp
  ddio_win/winio.cpp
  ddio_win/winkey.cpp
  ddio_win/winmouse.cpp
  ddio_win/wintimer.cpp
)
target_include_directories(ddio_win PRIVATE ${PROJECT_SOURCE_DIR}/ddio_win)

add_library(ddvid_win32 STATIC
  ddvid_win32/video_win32.cpp
  ddvid_win32/vidWin32FS.cpp
  ddvid_win32/vidWin32Win.cpp
)
target_include_directories(ddvid_win32 PRIVATE ${PROJECT_SOURCE_DIR}/ddvid_win32)

add_library(fix STATIC
  fix/fix.cpp
)
target_include_directories(fix PRIVATE ${PROJECT_SOURCE_DIR}/fix)

add_library(mem STATIC
  mem/mem.cpp
)
target_include_directories(mem PRIVATE ${PROJECT_SOURCE_DIR}/mem)

add_library(misc STATIC
  misc/endian.cpp
  misc/error.cpp
  misc/psglob.cpp
  misc/psrand.cpp
  misc/pstring.cpp
)
target_include_directories(misc PRIVATE ${PROJECT_SOURCE_DIR}/misc)

set(CMAKE_MFC_FLAG 2)  # Use this for shared MFC DLLs

# NewEditor project
add_executable(NewEditor WIN32
  NewEditor/ambient.cpp
  NewEditor/BendDialog.cpp
  NewEditor/bnode.cpp
  NewEditor/boa.cpp
  NewEditor/CameraSlew.cpp
  Editor/DallasFlagDlg.cpp
  Editor/DallasGenericPromptDlg.cpp
  Editor/DallasImportDlg.cpp
  Editor/DallasMainDlg.cpp
  Editor/DallasSoundDlg.cpp
  Editor/DallasStrmAudioDlg.cpp
  Editor/DallasTextureDlg.cpp
  Editor/DallasUserTypesDlg.cpp
  Editor/DallasUtilities.cpp
  Editor/DallasVectorPromptDialog.cpp
  NewEditor/DoorwayDialogBar.cpp
  Editor/ebnode.cpp
  NewEditor/EditLineDialog.cpp
  NewEditor/editor_lighting.cpp
  NewEditor/editor_object_lighting.cpp
  Editor/edoors.cpp
  Editor/EPath.cpp
  Editor/Erooms.cpp
  NewEditor/ExtrudeDialog.cpp
  NewEditor/FileNewDialog.cpp
  NewEditor/FolderDialog.cpp
  NewEditor/gamepath.cpp
  NewEditor/globals.cpp
  NewEditor/GoalDialog.cpp
  NewEditor/GrListBox.cpp
  Editor/Group.cpp
  NewEditor/HogBrowser.cpp
  Editor/HRoom.cpp
  Editor/HTexture.cpp
  NewEditor/LatheDialog.cpp
  NewEditor/LevelFrame.cpp
  NewEditor/LevelInfoDialog.cpp
  NewEditor/LightingDialog.cpp
  NewEditor/LightingProgress.cpp
  NewEditor/lightmap_info.cpp
  NewEditor/ListDialog.cpp
  NewEditor/loadlevel.cpp
  NewEditor/MainFrm.cpp
  NewEditor/matcen.cpp
  NewEditor/MatcenDialog.cpp
  NewEditor/MissionHogConfigDlg.cpp
  NewEditor/ned_Door.cpp
  NewEditor/ned_DrawWorld.cpp
  NewEditor/ned_GameTexture.cpp
  NewEditor/ned_Geometry.cpp
  NewEditor/ned_GrWnd.cpp
  NewEditor/ned_HFile.cpp
  NewEditor/ned_LevelGoal.cpp
  NewEditor/ned_LevelWnd.cpp
  NewEditor/ned_newstyle.cpp
  NewEditor/ned_Object.cpp
  NewEditor/ned_OrthoWnd.cpp
  NewEditor/ned_PathNode.cpp
  NewEditor/ned_PerspWnd.cpp
  NewEditor/ned_polymodel.cpp
  NewEditor/ned_Render.cpp
  NewEditor/ned_Renderer.cpp
  NewEditor/ned_RendOpenGL.cpp
  NewEditor/ned_Sound.cpp
  NewEditor/ned_TableRead.cpp
  NewEditor/ned_Trigger.cpp
  NewEditor/ned_Util.cpp
  NewEditor/NewEditor.cpp
  NewEditor/object_lighting.cpp
  NewEditor/ObjectDialogBar.cpp
  Editor/ObjectListCombo.cpp
  NewEditor/ObjectPalette.cpp
  NewEditor/PathDialogBar.cpp
  NewEditor/postrender.cpp
  NewEditor/ProgressDialog.cpp
  NewEditor/rad_cast.cpp
  NewEditor/rad_init.cpp
  NewEditor/RefFrameDialog.cpp
  NewEditor/room.cpp
  NewEditor/RoomDialogBar.cpp
  NewEditor/RoomFrm.cpp
  Editor/RoomListCombo.cpp
  NewEditor/RoomProperties.cpp
  Editor/RoomUVs.cpp
  NewEditor/ScriptCompiler.cpp
  Editor/ScriptCompilerAPI.cpp
  Editor/selectedroom.cpp
  Editor/SelManager.cpp
  NewEditor/SettingsDialog.cpp
  NewEditor/ship.cpp
  NewEditor/SoundDialogBar.cpp
  Editor/SoundListCombo.cpp
  NewEditor/special_face.cpp
  NewEditor/SplashScreen.cpp
  NewEditor/StdAfx.cpp
  NewEditor/TablefileManager.cpp
  NewEditor/TableView.cpp
  NewEditor/terrain.cpp
  NewEditor/TerrainDialogBar.cpp
  NewEditor/terrainrender.cpp
  NewEditor/TerrainSearch.cpp
  NewEditor/TexAlignDialog.cpp
  NewEditor/TextureDialogBar.cpp
  NewEditor/TexturePalette.cpp
  NewEditor/TipOfTheDay.cpp
  NewEditor/TriggerDialog.cpp
  Editor/TriggerListCombo.cpp
  NewEditor/Vclip.cpp
)

target_sources(NewEditor PRIVATE NewEditor/NewEditor.rc)

add_library(module STATIC
    module/module.cpp
    lib/module.h
)

add_library(ned_physics STATIC
    ned_physics/FindIntersection.cpp ned_physics/newstyle_fi.cpp ../editor/physics/physics_common.cpp
)

add_library(ned_virtualgcc STATIC
    ../editor/virtualgcc/virtualgcc.cpp
)

add_library(ned_virtualms STATIC
    ../editor/virtualms/virtualMS.cpp
)

add_library(texmap STATIC
    texmap/texture.cpp
)

add_library(vecmat STATIC
    vecmat/vector.cpp
)

add_library(win32 STATIC
    win32/winapp.cpp win32/windata.cpp win32/windebug.cpp win32/winmono.cpp win32/wintask.cpp
)

target_include_directories(NewEditor PRIVATE ${PROJECT_SOURCE_DIR}/NewEditor)
target_link_libraries(NewEditor PRIVATE
    2dlib 3d bitmap cfile ddgr_win32 ddio_common ddio_win ddvid_win32 fix mem misc module ned_physics ned_virtualgcc ned_virtualms texmap vecmat win32
)

# Set properties for debug and release configurations
set_property(TARGET 2dlib 3d bitmap cfile ddgr_win32 ddio_common ddio_win ddvid_win32 fix mem misc NewEditor PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} /Zc:forScope-")
endif()

add_compile_definitions(
  NEWEDITOR
  NED_PHYSICS
)

# Link libraries
target_link_libraries(NewEditor PRIVATE
  ddraw.lib
  dinput8.lib
  dxguid.lib
  winmm.lib
)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _AFXDLL)
endif()


# General include directory
# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/bitmap
    ${PROJECT_SOURCE_DIR}/cfile
    ${PROJECT_SOURCE_DIR}/ddgr_win32
    ${PROJECT_SOURCE_DIR}/ddio_common
    ${PROJECT_SOURCE_DIR}/ddio_win
    ${PROJECT_SOURCE_DIR}/ddvid_win32
    ${PROJECT_SOURCE_DIR}/fix
    ${PROJECT_SOURCE_DIR}/mem
    ${PROJECT_SOURCE_DIR}/misc
    ${PROJECT_SOURCE_DIR}/module
    ${PROJECT_SOURCE_DIR}/Editor
    ${PROJECT_SOURCE_DIR}/Editor/lib
    ${PROJECT_SOURCE_DIR}/Editor/NewEditor
    ${PROJECT_SOURCE_DIR}/Editor/Editor
    ${PROJECT_SOURCE_DIR}/NewEditor
    ${PROJECT_SOURCE_DIR}/ned_physics
    ${PROJECT_SOURCE_DIR}/ned_virtualgcc
    ${PROJECT_SOURCE_DIR}/ned_virtualms
    ${PROJECT_SOURCE_DIR}/texmap
    ${PROJECT_SOURCE_DIR}/vecmat
    ${PROJECT_SOURCE_DIR}/win32
    ${PROJECT_SOURCE_DIR}/2dlib
    ${PROJECT_SOURCE_DIR}/3d
    ${PROJECT_SOURCE_DIR}/lib
)

set_property(TARGET 2dlib 3d bitmap cfile ddgr_win32 ddio_common ddio_win ddvid_win32 fix mem misc NewEditor PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)

set_target_properties(NewEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../../
)

# Add definitions for DLL and MFC usage
add_compile_definitions(_AFXDLL)