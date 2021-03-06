HEADERS += \
    ../test/sphparametergroup.h \
    ../test/particleeffectparametergroup.h \
    ../test/ParamParser.h \
    ../test/mainwindow.h \
    ../test/glwidget.h \
    ../test/floatslider.h \
    ../test/aiwrapper.h \
    ../rtpslib/render/SSEffect.h \
    ../rtpslib/render/ShaderLibrary.h \
    ../rtpslib/render/Shader.h \
    ../rtpslib/render/ParticleEffect.h \
    ../rtpslib/render/MeshEffect.h \
    ../rtpslib/render/StreamlineEffect.h \
    ../rtpslib/render/Camera.h \
    ../rtpslib/render/Quaternion.h \
    ../rtpslib/render/RenderUtils.h \
    ../rtpslib/util.h \
    ../rtpslib/timer_eb.h \
    ../rtpslib/timege.h \
    ../rtpslib/structs.h \
    ../rtpslib/RTPSSettings.h \
    ../rtpslib/rtps_common.h \
    ../rtpslib/RTPS.h \
    ../rtpslib/materials_lights.h \
    ../rtpslib/gtod_windows.h \
    ../rtpslib/debug.h \
    ../rtpslib/system/System.h \
    ../rtpslib/system/SPHSettings.h \
    ../rtpslib/system/SPH.h \
    ../rtpslib/system/Simple.h \
    ../rtpslib/system/ParticleShape.h \
    ../rtpslib/system/ParticleRigidBodyParams.h \
    ../rtpslib/system/ParticleRigidBody.h \
    ../rtpslib/system/ForceField.h \
    ../rtpslib/system/FLOCKSettings.h \
    ../rtpslib/system/FLOCK.h \
    ../rtpslib/system/boids.h \
    ../test/rtpsparametergroup.h \
    ../test/rigidbodyparametergroup.h \
    ../test/flockingparametergroup.h \
    ../rtpslib/system/common/Hose.h \
    ../rtpslib/render/StereoCamera.h \
    ../rtpslib/render/util/stb_image_write.h

SOURCES += \
    ../test/sphparametergroup.cpp \
    ../test/particleeffectparametergroup.cpp \
    ../test/ParamParser.cpp \
    ../test/mainwindow.cpp \
    ../test/mainqt.cpp \
    ../test/glwidget.cpp \
    ../test/floatslider.cpp \
    ../test/aiwrapper.cpp \
    ../rtpslib/render/SSEffect.cpp \
    ../rtpslib/render/Shader.cpp \
    ../rtpslib/render/ParticleEffect.cpp \
    ../rtpslib/render/MeshEffect.cpp \
    ../rtpslib/render/StreamlineEffect.cpp \
    ../rtpslib/render/Camera.cpp \
    ../rtpslib/render/Quaternion.cpp \
    ../rtpslib/render/RenderUtils.cpp \
    ../rtpslib/util.cpp \
    ../rtpslib/timer_eb.cpp \
    ../rtpslib/timege.cpp \
    ../rtpslib/structs.cpp \
    ../rtpslib/RTPSSettings.cpp \
    ../rtpslib/RTPS.cpp \
    ../rtpslib/gtod_windows.cpp \
    ../rtpslib/system/System.cpp \
    ../rtpslib/system/SPHSettings.cpp \
    ../rtpslib/system/SPH.cpp \
    ../rtpslib/system/Simple.cpp \
    ../rtpslib/system/ParticleShape.cpp \
    ../rtpslib/system/ParticleRigidBody.cpp \
    ../rtpslib/system/FLOCKSettings.cpp \
    ../rtpslib/system/FLOCK.cpp \
    ../rtpslib/system/boids.cpp \
    ../rtpslib/render/ShaderLibrary.cpp \
    ../test/rtpsparametergroup.cpp \
    ../test/rigidbodyparametergroup.cpp \
    ../test/flockingparametergroup.cpp \
    ../rtpslib/system/common/Hose.cpp \
    ../rtpslib/render/StereoCamera.cpp

FORMS += \
    ../test/mainwindow.ui

OTHER_FILES += \
    ../rtpslib/render/shaders/vector_vert.glsl \
    ../rtpslib/render/shaders/vector_geom.glsl \
    ../rtpslib/render/shaders/vector_frag.glsl \
    ../rtpslib/render/shaders/sprite_vert.glsl \
    ../rtpslib/render/shaders/sprite_tex_frag.glsl \
    ../rtpslib/render/shaders/sprite_smoke_frag.glsl \
    ../rtpslib/render/shaders/sphere_vert.glsl \
    ../rtpslib/render/shaders/sphere_tex_frag.glsl \
    ../rtpslib/render/shaders/sphere_light.glsl \
    ../rtpslib/render/shaders/sphere_frag.glsl \
    ../rtpslib/render/shaders/render_water_IBL_vert.glsl \
    ../rtpslib/render/shaders/render_water_IBL_frag.glsl \
    ../rtpslib/render/shaders/render_lit_vert.glsl \
    ../rtpslib/render/shaders/render_lit_frag.glsl \
    ../rtpslib/render/shaders/render_instanced_vert.glsl \
    ../rtpslib/render/shaders/render_instanced_frag.glsl \
    ../rtpslib/render/shaders/normal_vert.glsl \
    ../rtpslib/render/shaders/normal_tex_frag.glsl \
    ../rtpslib/render/shaders/normal_frag.glsl \
    ../rtpslib/render/shaders/mpvertex.glsl \
    ../rtpslib/render/shaders/mpgeometry.glsl \
    ../rtpslib/render/shaders/mpfragment.glsl \
    ../rtpslib/render/shaders/gaussian_blur_y_frag.glsl \
    ../rtpslib/render/shaders/gaussian_blur_x_frag.glsl \
    ../rtpslib/render/shaders/gaussian_blur_vert.glsl \
    ../rtpslib/render/shaders/depth_vert.glsl \
    ../rtpslib/render/shaders/depth_frag.glsl \
    ../rtpslib/render/shaders/curvature_flow.cl \
    ../rtpslib/render/shaders/copy_vert.glsl \
    ../rtpslib/render/shaders/copy_frag.glsl \
    ../rtpslib/render/shaders/boid_tex_frag.glsl \
    ../rtpslib/render/shaders/post_process.vert \
    ../rtpslib/render/shaders/sphere.vert \
    ../rtpslib/render/shaders/render_water_IBL.vert \
    ../rtpslib/render/shaders/render_water_IBL.frag \
    ../rtpslib/render/shaders/render_lit.vert \
    ../rtpslib/render/shaders/render_lit.frag \
    ../rtpslib/render/shaders/render_instanced.vert \
    ../rtpslib/render/shaders/render_instanced.frag \
    ../rtpslib/render/shaders/gaussian_blur_y.frag \
    ../rtpslib/render/shaders/gaussian_blur_x.frag \
    ../rtpslib/render/shaders/gaussian_blur.frag \
    ../rtpslib/render/shaders/draw_vector.vert \
    ../rtpslib/render/shaders/draw_vector.geom \
    ../rtpslib/render/shaders/draw_vector.frag \
    ../rtpslib/render/shaders/curvature_flow.frag \
    ../rtpslib/render/shaders/copy.frag \
    ../rtpslib/render/shaders/bilateral_blur.frag \
    ../rtpslib/render/shaders/sphere.frag \
    ../rtpslib/render/shaders/normal.vert \
    ../rtpslib/render/shaders/normal.frag \
    ../rtpslib/render/shaders/passthrough.vert \
    ../rtpslib/render/shaders/passthrough.frag \
    ../rtpslib/render/shaders/skybox.frag \
    ../rtpslib/render/shaders/skybox.vert \
    ../rtpslib/README \
    ../rtpslib/CMakeLists.txt \
    ../rtpslib/system/README \
    ../rtpslib/render/shaders/copy_scalar.frag \
    ../rtpslib/render/shaders/fixed_width_gaussian.frag \
    ../rtpslib/render/shaders/copy_depth_color.frag \
    ../rtpslib/render/shaders/sphere_thickness.frag \
    ../rtpslib/render/shaders/composite_fluid.frag \
    ../rtpslib/render/shaders/copy_inverse.frag \
    ../rtpslib/render/shaders/draw_streamline.vert \
    ../rtpslib/render/shaders/draw_streamline.frag
