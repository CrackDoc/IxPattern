SET(IxSystem_DIR ${CMAKE_INSTALL_PREFIX}/IxSystem CACHE STRING "settting IxSystem_DIR path" FORCE)

#SET(IxSystem_DIR "" CACHE PATH "Location of IxSystem")

FIND_PATH(IxSystem_INCLUDE_DIR IxSystemExport.h
	${IxSystem_DIR}/include
    "${IxSystem_DIR}/Public Headers"
    "$ENV{IxSystem_PATH}/Public Headers"
    $ENV{IxSystem_PATH}
    $ENV{IxSystem_DIR}/include
    $ENV{IxSystem_DIR}
    /usr/local/include
	/usr/local/IxSystem/include
    /usr/include
	/usr/IxSystem/include
	/opt/include
    /opt/local/IxSystem/include
    /opt/IxSystem/include
    "C:/IxSystem/include"
	"C:/Program Files/IxSystem/include"
)
MACRO(FIND_IxSystem_LIBRARY MYLIBRARY MYLIBRARYNAME )

FIND_LIBRARY(${MYLIBRARY}
    NAMES
        ${MYLIBRARYNAME}
PATHS
	${IxSystem_DIR}/lib
    $ENV{IxSystem_PATH}
    $ENV{IxSystem_DIR}/lib
    $ENV{IxSystem_DIR}
    /usr/local/lib
	/usr/local/IxSystem/lib
    /usr/lib
	/usr/IxSystem/lib
	/opt/lib
    /opt/local/IxSystem/lib
    /opt/IxSystem/lib
    "C:/IxSystem/lib"
	"C:/Program Files/IxSystem/lib"
)
ENDMACRO(FIND_IxSystem_LIBRARY LIBRARY LIBRARYNAME)

FIND_IxSystem_LIBRARY(IxSystem_LIBRARY IxSystem )
FIND_IxSystem_LIBRARY(IxSystem_LIBRARY_DEBUG IxSystemd)

SET(IxSystem_FOUND FALSE)
IF (IxSystem_INCLUDE_DIR AND IxSystem_LIBRARY)
   SET(IxSystem_FOUND TRUE)
ENDIF (IxSystem_INCLUDE_DIR AND IxSystem_LIBRARY)
