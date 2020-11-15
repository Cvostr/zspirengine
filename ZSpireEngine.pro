#-------------------------------------------------
#
# Project created by QtCreator 2019-01-29T08:25:12
#
#-------------------------------------------------

QT       -= core gui

TARGET = zsengine
TEMPLATE = lib
CONFIG += staticlib
#unix:LIBS += -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lfreetype
unix:INCLUDEPATH += /usr/include/bullet/
unix:INCLUDEPATH += /usr/include/freetype2/
unix:INCLUDEPATH += ../vendor/
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
    code/src/Scripting/AngelScript.cpp \
    code/src/Scripting/AngelScriptMgr.cpp \
    code/src/Scripting/FilesAG.cpp \
    code/src/Scripting/GameObjectAG.cpp \
    code/src/Scripting/InputAG.cpp \
    code/src/Scripting/MathAG.cpp \
    code/src/Scripting/ResourcesAG.cpp \
    code/src/Scripting/Types/StdString.cpp \
    code/src/Scripting/ZPScriptClass.cpp \
        code/src/engine.cpp \
    code/src/engine/EngineComponent.cpp \
    code/src/engine/LocaleStringsFile.cpp \
    code/src/engine/loader.cpp \
    code/src/engine/resources.cpp \
    code/src/input/zs-input.cpp \
    code/src/math/BB3.cpp \
    code/src/math/Mat4.cpp \
    code/src/math/Quat.cpp \
    code/src/math/Vec3.cpp \
    code/src/misc/GlyphManager.cpp \
    code/src/misc/KeyValueFile.cpp \
    code/src/misc/misc.cpp \
    code/src/misc/oal_manager.cpp \
    code/src/misc/output.cpp \
    code/src/misc/zs3m-master.cpp \
    code/src/ogl/ogl_mesh.cpp \
    code/src/ogl/ogl_shader.cpp \
    code/src/ogl/ogl_texture.cpp \
    code/src/ogl/ogl_uniform_buf.cpp \
    code/src/render/Animation.cpp \
    code/src/render/MatShaderProps.cpp \
    code/src/render/Material.cpp \
    code/src/render/Math.cpp \
    code/src/render/Mesh.cpp \
    code/src/render/Shader.cpp \
    code/src/render/Texture.cpp \
    code/src/render/UniformBuffer.cpp \
    code/src/render/terrain-render.cpp \
    code/src/render/zs-pipeline.cpp \
    code/src/render/zs-screen-effect.cpp \
    code/src/resources/AnimationResource.cpp \
    code/src/resources/AudioResource.cpp \
    code/src/resources/GlyphResource.cpp \
    code/src/resources/MaterialResource.cpp \
    code/src/resources/MeshResource.cpp \
    code/src/resources/ScriptResource.cpp \
    code/src/resources/TextureResource.cpp \
    code/src/ui/Layouts/FreeLayout.cpp \
    code/src/ui/Layouts/LinearLayout.cpp \
    code/src/ui/View.cpp \
    code/src/ui/Widgets/Button.cpp \
    code/src/ui/Widgets/Widget.cpp \
    code/src/vulkan/vk_mesh.cpp \
    code/src/vulkan/vk_shader.cpp \
    code/src/vulkan/vk_texture.cpp \
    code/src/vulkan/vk_uniform_buf.cpp \
    code/src/vulkan/zsvk_mem.cpp \
    code/src/vulkan/zsvulkan.cpp \
    code/src/world/Camera.cpp \
    code/src/world/GameObject.cpp \
    code/src/world/GoProperty/AnimationComponent.cpp \
    code/src/world/GoProperty/AudioSourceProperty.cpp \
    code/src/world/GoProperty/CharacterController.cpp \
    code/src/world/GoProperty/ColliderProperty.cpp \
    code/src/world/GoProperty/LabelProperty.cpp \
    code/src/world/GoProperty/LightsourceProperty.cpp \
    code/src/world/GoProperty/MaterialProperty.cpp \
    code/src/world/GoProperty/MeshProperty.cpp \
    code/src/world/GoProperty/NodeProperty.cpp \
    code/src/world/GoProperty/RigidbodyProperty.cpp \
    code/src/world/GoProperty/ShadowCasterProperty.cpp \
    code/src/world/GoProperty/TerrainComponent.cpp \
    code/src/world/GoProperty/TransformProperty.cpp \
    code/src/world/GoProperty/TriggerProperty.cpp \
    code/src/world/GoProperty/ZPScriptProperty.cpp \
    code/src/world/GoProperty/go_properties.cpp \
    code/src/world/GoProperty/tile_properties.cpp \
    code/src/world/PhysicalWorld.cpp \
    code/src/world/Terrain.cpp \
    code/src/world/World.cpp


HEADERS += \
    code/headers/Scripting/AngelScript.hpp \
    code/headers/Scripting/AngelScriptMgr.h \
    code/headers/Scripting/FilesAG.hpp \
    code/headers/Scripting/Types/StdString.hpp \
    code/headers/Scripting/ZPScriptClass.hpp \
        code/headers/engine.h \ \
    code/headers/engine/EngineComponent.h \
    code/headers/engine/LocaleStringsFile.h \
    code/headers/engine/engine_window.h \
    code/headers/engine/loader.h \
    code/headers/engine/resources.h \
    code/headers/engine_types.h \
    code/headers/game.h \
    code/headers/input/zs-input.h \
    code/headers/math/BB3.hpp \
    code/headers/math/Mat4.hpp \
    code/headers/math/Quat.hpp \
    code/headers/math/RGBColor.hpp \
    code/headers/math/Vec2.hpp \
    code/headers/math/Vec3.hpp \
    code/headers/math/Vec4.hpp \
    code/headers/math/Vertex.hpp \
    code/headers/misc/GlyphManager.h \
    code/headers/misc/KeyValueFile.hpp \
    code/headers/misc/RefCounter.hpp \
    code/headers/misc/SmArray.hpp \
    code/headers/misc/misc.h \
    code/headers/misc/oal_manager.h \
    code/headers/misc/output.hpp \
    code/headers/misc/zs3m-master.h \
    code/headers/ogl/ogl.h \
    code/headers/render/Animation.hpp \
    code/headers/render/Material.hpp \
    code/headers/render/Math.hpp \
    code/headers/render/Mesh.hpp \
    code/headers/render/Shader.hpp \
    code/headers/render/Texture.h \
    code/headers/render/UniformBuffer.hpp \
    code/headers/render/screen_effects/gauss_blur.h \
    code/headers/render/zs-pipeline.h \
    code/headers/render/zs-screen-effect.h \
    code/headers/ui/Layouts/FreeLayout.hpp \
    code/headers/ui/Layouts/ILayout.hpp \
    code/headers/ui/Layouts/LinearLayout.hpp \
    code/headers/ui/UI.hpp \
    code/headers/ui/View.hpp \
    code/headers/ui/Widgets/Button.hpp \
    code/headers/ui/Widgets/Widget.hpp \
    code/headers/vulkan/vk_data.h \
    code/headers/vulkan/zsvk_mem.h \
    code/headers/vulkan/zsvulkan.h \
    code/headers/world/Camera.hpp \
    code/headers/world/Physics.h \
    code/headers/world/Terrain.hpp \
    code/headers/world/World.hpp \
    code/headers/world/go_properties.h \
    code/headers/world/tile_properties.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
