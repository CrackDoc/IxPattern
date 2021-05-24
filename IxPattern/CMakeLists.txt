# CMakeList.txt: CMakeProject 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

#-----------------TODO 自定义编译选项
# set(CMAKE_CXX_FLAGS “-std=c++11 -O2 -g”)

STRING( REGEX REPLACE ".*/(.*)" "\\1" PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} ) 

#-----------------TODO 添加模块变量
set(_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/include)
set(_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/src)

# 添加执行EXE目录
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/include _INC_EXPATH)
#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/src _SRC_EXPATH)
file(GLOB_RECURSE HEADER_FILES ${_INC_PATH}/*.h)
file(GLOB_RECURSE SOURCE_FILES ${_SRC_PATH}/*.cpp)

#-----------------TODO EXE编译连接
#add_executable (${PROJECT_NAME} ${_INC_EXPATH} ${_SRC_EXPATH})

#-----------------TODO 创建工程内的库
#-----------------STATIC表示创建静态库，目标文件为 libmylib.a，如果是 SHARED， 则为创建动态库.
#add_library(${PROJECT_NAME} SHARED ${_INC_EXPATH} ${_SRC_EXPATH})

file(GLOB_RECURSE HEADER_FILES ${_INC_PATH}/*.h)
file(GLOB_RECURSE SOURCE_FILES ${_SRC_PATH}/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})

#-----------------TODO添加头文件的搜索目录
include_directories(${_INC_PATH}
					${_SRC_PATH})
					
#-----------------TODO 添加lib文件的搜索目录
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
	
find_package(pthread)
if(pthread_FOUND)
	include_directories(${pthread_DIR}/include)
	link_directories(${pthread_DIR}/lib)
	target_link_libraries(${PROJECT_NAME} general ${pthread_LIBRARY})
endif(pthread_FOUND)

find_package(IxPublic)
if(IxPublic_FOUND)
	include_directories(${IxPublic_DIR}/include)
	link_directories(${IxPublic_DIR}/lib)
	target_link_libraries(${PROJECT_NAME} debug ${IxPublic_LIBRARY_DEBUG})
	target_link_libraries(${PROJECT_NAME} optimized ${IxPublic_LIBRARY})
endif(IxPublic_FOUND)

find_package(ExtendStructure)
if(ExtendStructure_FOUND)
	include_directories(${ExtendStructure_DIR}/include)
	link_directories(${ExtendStructure_DIR}/lib)
	target_link_libraries(${PROJECT_NAME} debug ${ExtendStructure_LIBRARY_DEBUG})
	target_link_libraries(${PROJECT_NAME} optimized ${ExtendStructure_LIBRARY})
	
endif(ExtendStructure_FOUND)

find_package(IxSystem)
if(IxSystem_FOUND)
	include_directories(${IxSystem_DIR}/include)
	link_directories(${IxSystem_DIR}/lib)
	target_link_libraries(${PROJECT_NAME} debug ${IxSystem_LIBRARY_DEBUG})
	target_link_libraries(${PROJECT_NAME} optimized ${IxSystem_LIBRARY})
endif(IxSystem_FOUND)
#-----------------TODO 执行外部命令
#set(THRIFT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/mythrift.thrift)
#exec_program("thrift --gen cpp -o ${CMAKE_CURRENT_SOURCE_DIR} ${THRIFT_FILE}")

#-----------------TODO: 如有需要，请添加测试并安装目标。
