#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T08:25:12
#
#-------------------------------------------------

QT       -= core gui

TARGET = zsengine
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/engine.cpp \
    src/vulkan/zsvulkan.cpp \
    src/render/zs-pipeline.cpp \
    src/render/zs-shader.cpp \
    src/render/zs-math.cpp \
    src/engine/resources.cpp \
    src/world/World.cpp \
    src/world/go_properties.cpp \
    src/world/GoProperty/tile_properties.cpp \
    src/world/GoProperty/go_filestream.cpp \
    src/world/GameObject.cpp \
    src/world/zs-camera.cpp \
    src/render/zs-mesh.cpp \
    src/engine/EngineManager.cpp \
    src/render/zs-texture.cpp \
    src/Scripting/LuaScript.cpp \
    src/engine/loader.cpp \
    src/misc/oal_manager.cpp \
    src/Scripting/zsensdk.cpp \
    src/misc/randomg.cpp

HEADERS += \
        headers/engine.h \
    headers/vulkan/zsvulkan.h \
    headers/render/zs-pipeline.h \
    headers/render/zs-shader.h \
    headers/render/zs-math.h \
    headers/engine/resources.h \
    headers/world/World.h \
    headers/world/go_properties.h \
    headers/game.h \
    headers/world/tile_properties.h \
    headers/engine/engine_window.h \
    headers/world/zs-camera.h \
    headers/render/zs-mesh.h \
    headers/engine/EngineManager.h \
    headers/render/zs-texture.h \
    headers/Scripting/LuaScript.h \
    headers/engine/loader.h \
    headers/engine_types.h \
    headers/misc/oal_manager.h \
    headers/Scripting/zsensdk.h \
    headers/misc/randomg.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
