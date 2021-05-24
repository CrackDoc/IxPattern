SET(pthread_DIR ${CMAKE_INSTALL_PREFIX}/pthread CACHE STRING "settting pthread_DIR path" FORCE)

#SET(pthread_DIR "" CACHE PATH "Location of pthread")

FIND_PATH(pthread_INCLUDE_DIR pthread.h
	${pthread_DIR}/include
    "${pthread_DIR}/Public Headers"
    "$ENV{pthread_PATH}/Public Headers"
    $ENV{pthread_PATH}
    $ENV{pthread_DIR}/include
    $ENV{pthread_DIR}
    /usr/local/include
	/usr/local/pthread/include
    /usr/include
	/usr/pthread/include
	/opt/include
    /opt/local/pthread/include
    /opt/pthread/include
    "C:/pthread/include"
	"C:/Program Files/pthread/include"
)

FIND_LIBRARY(pthread_LIBRARY
NAMES "pthread"
PATHS
	${pthread_DIR}/lib
    $ENV{pthread_PATH}
    $ENV{pthread_DIR}/lib
    $ENV{pthread_DIR}
    /usr/local/lib
	/usr/local/pthread/lib
    /usr/lib
	/usr/pthread/lib
	/opt/lib
    /opt/local/pthread/lib
    /opt/pthread/lib
    "C:/pthread/lib"
	"C:/Program Files/pthread/lib"
)
SET(pthread_FOUND FALSE)
IF (pthread_INCLUDE_DIR AND pthread_LIBRARY)
   SET(pthread_FOUND TRUE)
ENDIF (pthread_INCLUDE_DIR AND pthread_LIBRARY)
