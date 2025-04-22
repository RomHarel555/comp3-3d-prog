cmake_minimum_required(VERSION 3.16)
project(QtVulkanApp LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(QtVulkanApp
    RenderWindow.cpp RenderWindow.h
    MainWindow.cpp MainWindow.h
    main.cpp
    VulkanWindow.h VulkanWindow.cpp
    gamemanager.h gamemanager.cpp
    ObjLoader.h ObjLoader.cpp
    HeightMap.h HeightMap.cpp
    stb_image.cpp
    assets.qrc
)
# Define the shader files
set(SHADER_FILES
    color.frag
    color.vert
    noTexture.frag
    noTexture.vert
)

# Add the shader files to the project
add_custom_target(Shaders
    SOURCES ${SHADER_FILES}
)

set_target_properties(QtVulkanApp PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(QtVulkanApp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Resources:
set_source_files_properties("color_frag.spv"
    PROPERTIES QT_RESOURCE_ALIAS "color_frag.spv"
)

set_source_files_properties("color_vert.spv"
    PROPERTIES QT_RESOURCE_ALIAS "color_vert.spv"
)

set_source_files_properties("noTexture_frag.spv"
    PROPERTIES QT_RESOURCE_ALIAS "noTexture_frag.spv"
)

set_source_files_properties("noTexture_vert.spv"
    PROPERTIES QT_RESOURCE_ALIAS "noTexture_vert.spv"
)

set(QtVulkanApp_resource_files
    "color_frag.spv"
    "color_vert.spv"
    "noTexture_frag.spv"
    "noTexture_vert.spv"
)

qt_add_resources(QtVulkanApp "QtVulkanApp"
    PREFIX
        "/"
    FILES
        ${QtVulkanApp_resource_files}
)

qt_add_resources(QtVulkanApp "assets_resources"
    PREFIX
        "/"
    FILES
        "assets/textures/wood_texture.jpg"
        "assets/heightmaps/Heightmap.jpg"
)

qt_add_resources(QtVulkanApp "model_resources"
    PREFIX
        "/"
    FILES
        "assets/models/CrateCube.obj"
)

install(TARGETS QtVulkanApp
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QtVulkanApp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# auto-compilation of the shaders:
add_custom_target(
    PreBuildCommandF ALL
    COMMAND glslc color.frag -o color_frag.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling fragment shader"
)
add_custom_target(
    PreBuildCommandV ALL
    COMMAND glslc color.vert -o color_vert.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling vertex shader"
)
add_custom_target(
    PreBuildCommandNoTexF ALL
    COMMAND glslc noTexture.frag -o noTexture_frag.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling no-texture fragment shader"
)
add_custom_target(
    PreBuildCommandNoTexV ALL
    COMMAND glslc noTexture.vert -o noTexture_vert.spv
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling no-texture vertex shader"
)

add_dependencies(QtVulkanApp PreBuildCommandF)
add_dependencies(QtVulkanApp PreBuildCommandV)
add_dependencies(QtVulkanApp PreBuildCommandNoTexF)
add_dependencies(QtVulkanApp PreBuildCommandNoTexV)


