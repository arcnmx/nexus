file(GLOB_RECURSE sources_common RELATIVE "${SRC_ROOT}"
	"Util/*.cpp"
)

list(APPEND source_dirs
	Events
	Inputs
	Loader
	Paths
	Services
	UI
)

foreach(sdir ${source_dirs})
	file(GLOB_RECURSE sdir_sources
		"${sdir}/*.cpp"
	)
	list(APPEND sources ${sdir_sources})
endforeach()
file(GLOB sources_root RELATIVE ${SRC_ROOT}
	"*.cpp")

set(BRANCH_FILE ${CMAKE_CURRENT_BINARY_DIR}/Branch.h)
add_custom_command(
	COMMAND bash -c "tee \"${BRANCH_FILE}\" <<<\"#define BRANCH_NAME \\\"mew\\\"\""
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}"
	OUTPUT "${BRANCH_FILE}"
	COMMENT "generating Branch.h"
	VERBATIM
)
set(VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
add_custom_command(
	COMMAND bash -c "printf \'#define %s %d\\n\' V_MAJOR 1 V_MINOR 0 V_BUILD 0 V_REVISION 0 > \"${VERSION_FILE}\""
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}"
	OUTPUT "${VERSION_FILE}"
	COMMENT "generating Version.h"
	VERBATIM
)
set(HTTPLIB_FILE ${CMAKE_CURRENT_BINARY_DIR}/httplib/httplib.h)
add_custom_command(
	COMMAND bash -c "mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/httplib && tee ${HTTPLIB_FILE} <<<\"#include <httplib.h>\""
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}"
	OUTPUT "${HTTPLIB_FILE}"
	COMMENT "including httplib"
	VERBATIM
)
set(WINDOWS_HEADER ${CMAKE_CURRENT_BINARY_DIR}/Windows.h)
add_custom_command(
	COMMAND bash -c "tee ${WINDOWS_HEADER} <<<\"#include <windows.h>\""
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}"
	OUTPUT "${WINDOWS_HEADER}"
	COMMENT "including windows"
	VERBATIM
)

set(THIRDPARTY "${SRC_ROOT}/thirdparty")

add_custom_command(
	COMMAND bash -c "ln -sf ${THIRDPARTY}/imgui ${CMAKE_CURRENT_BINARY_DIR}/imgui"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}"
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/imgui"
	COMMENT "symlink include imgui"
	VERBATIM
)
add_custom_command(
	COMMAND bash -c "ln -sf ${THIRDPARTY}/ImAnimate ${CMAKE_CURRENT_BINARY_DIR}/ImAnimate"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}"
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/ImAnimate"
	COMMENT "symlink include ImAnimate"
	VERBATIM
)
add_custom_command(
	COMMAND bash -c "ln -sf ${THIRDPARTY}/minhook ${CMAKE_CURRENT_BINARY_DIR}/minhook"
	MAIN_DEPENDENCY "${CMAKE_CURRENT_BINARY_DIR}"
	OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/minhook"
	COMMENT "symlink include minhook"
	VERBATIM
)

set(CMAKE_THREAD_PREFER_PTHREAD OFF)
set(THREADS_PREFER_PTHREAD_FLAG OFF)
set(THREADS_HAVE_PTHREAD_ARG OFF)
pkg_check_modules(mcfgthread REQUIRED mcfgthread IMPORTED_TARGET mcfgthread)
set(CMAKE_THREAD_LIBS_INIT PkgConfig::mcfgthread)

pkg_check_modules(nlohmann_json REQUIRED IMPORTED_TARGET nlohmann_json)
pkg_check_modules(stb REQUIRED IMPORTED_TARGET stb)
pkg_check_modules(libcrypto REQUIRED IMPORTED_TARGET libcrypto)

find_package(httplib REQUIRED)
target_link_libraries(httplib::httplib INTERFACE
	crypt32
)
target_link_libraries(OpenSSL::Crypto INTERFACE
	ws2_32
)

set(IMGUI_ROOT "${THIRDPARTY}/imgui")
file(GLOB imgui_sources RELATIVE ${SRC_ROOT}
	"${IMGUI_ROOT}/*.cpp"
)
add_library(imgui SHARED ${imgui_sources})
target_include_directories(imgui PUBLIC "${IMGUI_ROOT}")
target_link_libraries(imgui PUBLIC d3dcompiler gdi32)
target_compile_options(imgui PUBLIC
	# imgui_extensions.h
	-Wno-error=format-security
)

set(IMANIMATE_ROOT "${THIRDPARTY}/ImAnimate")
add_library(imanimate STATIC "${IMANIMATE_ROOT}/ImAnimate.cpp")
target_include_directories(imanimate PUBLIC "${IMANIMATE_ROOT}")
target_link_libraries(imanimate PUBLIC imgui)

set(MINHOOK_ROOT "${THIRDPARTY}/minhook")
file(GLOB minhook_sources RELATIVE ${SRC_ROOT}
	"${MINHOOK_ROOT}/*.cpp"
)
add_library(minhook STATIC
	${minhook_sources}
	"${WINDOWS_HEADER}"
)
target_include_directories(minhook PUBLIC
	"${MINHOOK_ROOT}"
	"${CMAKE_CURRENT_BINARY_DIR}"
)

add_library(nexus SHARED
	${sources_root} ${sources} ${sources_common}
	"${BRANCH_FILE}" "${VERSION_FILE}" "${HTTPLIB_FILE}"
	"${SRC_ROOT}/Nexus.rc" "${SRC_ROOT}/exports.def"
	"${CMAKE_CURRENT_BINARY_DIR}/imgui"
	"${CMAKE_CURRENT_BINARY_DIR}/ImAnimate"
	"${CMAKE_CURRENT_BINARY_DIR}/minhook"
)
set_property(TARGET nexus PROPERTY CXX_STANDARD 23)
set_property(TARGET nexus PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET nexus PROPERTY CXX_VISIBILITY_PRESET hidden)
target_precompile_headers(nexus PRIVATE "${SRC_ROOT}/Shared.h")

target_include_directories(nexus PRIVATE
	"${SRC_ROOT}"
	"${SRC_ROOT}/Util"
	"${CMAKE_CURRENT_BINARY_DIR}"
)
target_compile_options(nexus PRIVATE
	-pedantic
)

target_link_libraries(nexus PRIVATE
	imgui imanimate
	minhook
	PkgConfig::nlohmann_json
	OpenSSL::Crypto
	httplib::httplib
	PkgConfig::stb
	ws2_32
)

install(TARGETS nexus imgui
	ARCHIVE
	RUNTIME
	PUBLIC_HEADER
)
