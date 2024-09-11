cmake_minimum_required(VERSION 3.26)
project(3DRenderer)

set(CMAKE_CXX_STANDARD 23)

add_definitions(-DSDL_MAIN_HANDLED)
add_executable(3DRenderer main.cpp
        Ray.cpp
        Ray.h
        Sphere.cpp
        Sphere.h
        Light.cpp
        Light.h
        imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/imgui_impl_sdl2.cpp
        imgui/imgui_impl_sdl2.h
        imgui/imgui_impl_sdlrenderer2.cpp
        imgui/imgui_impl_sdlrenderer2.h
        Model.cpp
        Model.h
        Gl.cpp
        Gl.h
        Geometry.cpp
        Geometry.h
        Hittable.h
        HittableList.cpp
        HittableList.h
        Rt.cpp
        Rt.h
        Interval.cpp
        Interval.h
)

target_link_libraries(3DRenderer SDL2_test SDL2)