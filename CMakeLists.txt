# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

STRING( REGEX REPLACE ".*/(.*)" "\\1" PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} ) 

project (${PROJECT_NAME})

set(IDE_PATH "D:/IDE/Utility/v142/x64")

if(NOT EXISTS ${IDE_PATH})
	file(MAKE_DIRECTORY ${IDE_PATH})
endif()

set(CMAKE_INSTALL_PREFIX ${IDE_PATH} CACHE STRING "Defulat Install prefix" FORCE)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" CACHE STRING "ModulesPATH" FORCE)

# Detect C++11 compiler support and set a variable.
INCLUDE(CXX11)
check_for_cxx11_compiler(CXX11_AVAILABLE)

SET(CMAKE_DEBUG_POSTFIX  "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

# 包含子项目。
add_subdirectory (${PROJECT_NAME})

add_subdirectory("UnitTest")

SET(IncludeDir ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME})

#install 工程

INSTALL(DIRECTORY ${IncludeDir}/include DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}
		#设置不包含CVS 目录的文件
        #PATTERN "CVS" EXCLUDE.
		#设置拷贝后的文件权限
        PATTERN "include/*"
        #PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
        #GROUP_EXECUTE GROUP_READ
		PERMISSIONS OWNER_READ)
		
INSTALL(TARGETS ${PROJECT_NAME} ${PROJECT_NAME} ${PROJECT_NAME}
       RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/bin
       LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/lib
       ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}/lib
)