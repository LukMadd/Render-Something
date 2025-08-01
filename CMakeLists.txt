#-----------------------------------------General Setup-----------------------------------------#
cmake_minimum_required(VERSION 3.28.0)

project(Render-A-Triangle)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#-----------------------------------------Find GLFW and Vulkan-----------------------------------------#
if(DEFINED VULKAN_SDK_PATH)
  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/lib")
  set(Vulkan_FOUND "True")

  else()
  find_package(Vulkan REQUIRED)
  set(Vulkan_PATH "$ENV{VULKAN_SDK}")
  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")

  endif()

if(NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
else()
	message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()





if(DEFINED GLFW_PATH)
  message(STATUS "Using GLFW path specified in .env: ${GLFW_PATH}")
  set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
  
  if (MSVC)
    set(GLFW_LIB "${GLFW_PATH}/lib-vc2019")
  
  elseif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(STATUS "USING MINGW")
    set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64")
  endif()

else()
  find_package(glfw3 3.3 REQUIRED)

  set(GLFW_LIB_PATH ${glfw3_DIR})
  set(GLFW_FOUND "True")
  message(STATUS "GLFW library path: ${GLFW_LIB_PATH}")

  message(STATUS "Found GLFW")
endif()

if (NOT GLFW_FOUND)
  message(FATAL_ERROR "Could not find glfw library!")
else()
  message(STATUS "Using glfw lib: ${GLFW_LIB}")
endif()


find_package(glm REQUIRED)

#-----------------------------------------Set source files-----------------------------------------#
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.mm")

add_executable(Render-A-Triangle ${SOURCES})


#-----------------------------------------Find glslang-----------------------------------------#
find_program(GLSLANG_VALIDATOR glslangValidator)

if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "Please install glslangValidator(brew install glslang)")
else()
    message(STATUS "Glslang validator found")
endif()


#-----------------------------------------Shader compilation-----------------------------------------#
file(GLOB SHADER_SOURCES "${CMAKE_SOURCE_DIR}/shaders/*.vert" "${CMAKE_SOURCE_DIR}/shaders/*.frag")

set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
    file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

    set(SPIRV_SHADERS "")

    foreach(SHADER_FILE ${SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
        get_filename_component(SHADER_EXT ${SHADER_FILE} EXT)
        string(SUBSTRING ${SHADER_EXT} 1 -1 SHADER_EXT_NO_DOT)
        set(SHADER_OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}_${SHADER_EXT_NO_DOT}.spv")

        add_custom_command(
            OUTPUT ${SHADER_OUTPUT}
            COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER_FILE} -o ${SHADER_OUTPUT}
            DEPENDS ${SHADER_FILE}
            VERBATIM
        )

        list(APPEND SPIRV_SHADERS ${SHADER_OUTPUT})
    endforeach()

    add_custom_target(
        Shaders ALL
        DEPENDS ${SPIRV_SHADErs}
    )

    set(RUNTIME_SHADER_DIR "${CMAKE_SOURCE_DIR}/bin/shaders")
    file(MAKE_DIRECTORY ${RUNTIME_SHADER_DIR})
    add_custom_command(
        TARGET Shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_OUTPUT_DIR} ${RUNTIME_SHADER_DIR}
    )

add_dependencies(Render-A-Triangle Shaders)

#-----------------------------------------Include directories and libraries-----------------------------------------#
target_include_directories(Render-A-Triangle PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${VULKAN_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
)

message(STATUS "GLFW LIB PATH: ${GLFW_LIB_PATH}")
target_link_libraries(Render-A-Triangle PRIVATE
    glfw
    glm::glm
    ${Vulkan_LIBRARIES}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

target_link_libraries(Render-A-Triangle PRIVATE
    "${Vulkan_PATH}/lib/libMoltenVK.dylib"
    "-framework QuartzCore"
    "-framework CoreFoundation"
    "-framework CoreGraphics"
    "-framework IOKit"
    "-framework IOSurface"
    "-framework Metal"
    "-framework MetalKit"
    "-framework Foundation"
    "-framework AppKit"
)
endif()
