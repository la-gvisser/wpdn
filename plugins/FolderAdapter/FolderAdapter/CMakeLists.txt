cmake_minimum_required (VERSION 3.10)
project (FolderAdapterOE)
# Version: 1.12.0.0
set (PROJECT_VERSION_MAJOR 1)
set (PROJECT_VERSION_MINOR 12)
set (PROJECT_VERSION_PATCH 0)
set (PROJECT_VERSION_TWEAK 0)
set (PROJECT_PLUGIN_DATE_REVISION "2024.03.13.0")
set (CMAKE_CXX_COMPILER "g++")
set (OMNI_PLUGIN_DIRECTORY "/usr/lib/omni/plugins")
set (OMNI_PLUGIN_WEB_DIRECTORY "${OMNI_PLUGIN_DIRECTORY}/web")
set (LINUX_RELEASE "${OS_Version}")

execute_process(COMMAND lsb_release -rs
    OUTPUT_VARIABLE OS_Version
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message ("Building with Ubuntu OS Version: ${OS_Version}")
string(REPLACE "." "_" LINUX_RELEASE "${OS_Version}")

set (ProjectDirectory	"${CMAKE_CURRENT_SOURCE_DIR}")
set (PluginDirectory 	  	"${ProjectDirectory}/Plugin")
set (PluginWebDirectory 	"${PluginDirectory}/web")
set (FrameworkDirectory   	"${ProjectDirectory}/Framework")
set (HeliumDirectory 	  	"${ProjectDirectory}/Helium")
set (HeliumLibrary	 	"${ProjectDirectory}/lib/libheruntime.so")

set (FLAGS   	  "-pthread -Wall -flto -fPIC -std=c++14 -Wno-non-virtual-dtor -Wno-multichar -march=westmere -O2 -g3 -Wno-unused-but-set-variable -Wno-address-of-packed-member -Wno-class-memaccess ${CMAKE_RELEASE_MACROS}")
set (FLAGS_DBG    "-pthread -Wall -flto -fPIC -std=c++14 -Wno-non-virtual-dtor -Wno-multichar -g3 -ggdb3 -O0 -fno-inline -Wno-unused-function -Wno-unused-but-set-variable -Wno-address-of-packed-member -Wno-class-memaccess ${CMAKE_DEBUG_MACROS}")
set (MACROS       "-D_UNICODE -DUNICODE -DTARGET_OS_LINUX -D_NPROTOSPECS -D_REENTRANT")
set (MACROS_DBG   "-D_UNICODE -DUNICODE -DTARGET_OS_LINUX -D_NPROTOSPECS -D_REENTRANT -D_DEBUG")
set (LDFLAGS      "-pthread -Wl,-rpath,. -Wl,--version-script=module.map -lpthread -Llib") 
set (LDFLAGS_DBG  "-g -pthread -Wl,-rpath,. -Wl,--version-script=module.map -lpthread -Llib")

find_library (HeliumLibrary NAMES heruntime PATHS "${ProjectDirectory}/lib"  NO_DEFAULT_PATH)
if (HeliumLibrary)
	message (STATUS "Found Helium Library ${HeliumLibrary}")
endif()

file (GLOB FrameworkCppFiles "${FrameworkDirectory}/*.cpp")
		
set (PluginIncFiles     ${PluginDirectory}/AdapterOptions.h
			${PluginDirectory}/OmniAdapter.h
			${PluginDirectory}/OmniEngineContext.h
			${PluginDirectory}/OmniPlugin.h
			${PluginDirectory}/Options.h
			${PluginDirectory}/PluginMessages.h
			${PluginDirectory}/resource.h
			${PluginDirectory}/StdAfx.h
			${PluginDirectory}/XmlUtil.h)

set (PluginCppFiles 	${PluginDirectory}/AdapterOptions.cpp
			${PluginDirectory}/OmniAdapter.cpp		
		    	${PluginDirectory}/OmniEngineContext.cpp
		    	${PluginDirectory}/OmniPlugin.cpp
		    	${PluginDirectory}/Options.cpp
		    	${PluginDirectory}/PluginMessages.cpp
		    	${PluginDirectory}/StdAfx.cpp
		    	${PluginDirectory}/XmlUtil.cpp)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
	message (STATUS "performing Debug Build")
	set (CMAKE_CXX_FLAGS_DEBUG "${FLAGS_DBG} ${MACROS_DBG}")
	set (CMAKE_SHARED_LINKER_FLAGS "${LDFLAGS_DBG}")
	message (STATUS "CMAKE_DEBUG_FLAGS ${CMAKE_CXX_FLAGS_DEBUG}")
	message (STATUS "CMAKE_SHARED_LINKER_FLAGS ${LDFLAGS_DBG}")
elseif (CMAKE_BUILD_TYPE MATCHES RELEASE)
	message (STATUS "performing Release Build")
	set (CMAKE_CXX_FLAGS_RELEASE "${FLAGS} ${MACROS}")
	set (CMAKE_SHARED_LINKER_FLAGS "${LDFLAGS}")
	message (STATUS "CMAKE_RELEASE_FLAGS ${CMAKE_CXX_FLAGS_RELEASE}")
	message (STATUS "CMAKE_SHARED_LINKER_FLAGS ${LDFLAGS}")
endif()

add_library (${PROJECT_NAME} SHARED ${PluginCppFiles} ${PluginIncFiles} ${FrameworkCppFiles})
target_link_Libraries (${PROJECT_NAME} ${HeliumLibrary})
target_include_directories (${PROJECT_NAME} PUBLIC ${PluginDirectory} ${FrameworkDirectory} ${HeliumDirectory})
set_target_properties (${PROJECT_NAME} PROPERTIES PREFIX "")

set (EXE_PATHS "/usr/bin /usr/local/bin")
find_program(ZIP_EXECUTABLE 7z "${EXE_PATHS}" "${EXE_PATHS}")
if (ZIP_EXECUTABLE)
	message (STATUS "Zip Executable ${ZIP_EXECUTABLE}")
endif()
find_program(TAR_EXECUTABLE tar "${EXE_PATHS}" "${EXE_PATHS}")
if (TAR_EXECUTABLE) 
        message (STATUS "tar Executable ${TAR_EXECUTABLE}")
endif()

set (CAPTURE_PLUGIN_DIRECTORY "${PluginDirectory}/CaptureTab")
set (CAPTURE_ZIP_FILE "${PluginDirectory}/${PROJECT_NAME}-CaptureTab.zip")
add_custom_command (TARGET ${PROJECT_NAME}
		POST_BUILD 
		COMMAND ${ZIP_EXECUTABLE} "a" ${CAPTURE_ZIP_FILE} 
		WORKING_DIRECTORY ${CAPTURE_PLUGIN_DIRECTORY})
		
set (OPTIONS_PLUGIN_DIRECTORY "${PluginDirectory}/Options")
set (OPTIONS_ZIP_FILE "${PluginDirectory}/${PROJECT_NAME}-Options.zip")
add_custom_command (TARGET ${PROJECT_NAME}
		POST_BUILD 
		COMMAND ${ZIP_EXECUTABLE} "a" ${OPTIONS_ZIP_FILE} 
		WORKING_DIRECTORY ${OPTIONS_PLUGIN_DIRECTORY})
			
set (WEB_DIRECTORY "${PluginDirectory}/web")
message ("Plugin web directory: ${WEB_DIRECTORY}")
set (WEB_TARBALL_FILE "${PluginDirectory}/${PROJECT_NAME}-web.tar.gz")
message ("Plugin web tarball file: ${WEB_TARBALL_FILE}")
file (REMOVE [${WEB_TARBALL_FILE}])
add_custom_command(TARGET ${PROJECT_NAME} 
		POST_BUILD
		COMMAND ${TAR_EXECUTABLE} "-czf" ${WEB_TARBALL_FILE} "*"
		WORKING_DIRECTORY ${WEB_DIRECTORY})

set (PLUGIN_FILES ${CAPTURE_ZIP_FILE} ${OPTIONS_ZIP_FILE})
set (PLUGIN_HE_FILE "${PROJECT_NAME}.he")
set (PLUGIN_NAME "${PROJECT_NAME}.so")
set (PLUGIN_WEB_FILE ${WEB_TARBALL_FILE})
set (BIN_IN_FILE "${ProjectDirectory}/${PROJECT_NAME}.so" )

install (TARGETS ${PROJECT_NAME} LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DESTINATION ${OMNI_PLUGIN_DIRECTORY}) 
install (FILES ${PLUGIN_FILES} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DESTINATION ${OMNI_PLUGIN_DIRECTORY})
install (FILES ${PLUGIN_WEB_FILE} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DESTINATION ${OMNI_PLUGIN_DIRECTORY})
install (FILES ${PLUGIN_HE_FILE} PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DESTINATION ${OMNI_PLUGIN_DIRECTORY})
install (DIRECTORY "${PluginWebDirectory}" DESTINATION ${OMNI_PLUGIN_DIRECTORY} FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# setup CPack Variables
SET (CPACK_GENERATOR "DEB")
SET (CPACK_DEBIAN_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}-${OS_Version}.deb")
SET (CPACK_DEBIAN_PACKAGE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "n/a")
SET (CPACK_PACKAGE_NAME "${PROJECT_NAME}")
SET (CPACK_PACKAGE_VENDOR "n/a")
SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} for Ubuntu ${OS_Version}")
SET (CPACK_PACKAGE_INSTALL_DIRECTORY ${OMNI_PLUGIN_DIRECTORY})
SET (CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
SET (CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
SET (CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
SET (CPACK_PACKAGE_VERSION_TWEAK "${PROJECT_VERSION_TWEAK}")
set (CPACK_PACKAGE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
SET (CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}_${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}_${PROJECT_PLUGIN_DATE_REVISION}-Linux${OS_Version}-x64")
SET (CPACK_PACKAGE_DIRECTORY "${ProjectDirectory}")
INCLUDE (CPack)