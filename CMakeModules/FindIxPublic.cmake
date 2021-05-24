SET(IxPublic_DIR ${CMAKE_INSTALL_PREFIX}/IxPublic CACHE STRING "settting IxPublic_DIR path" FORCE)

#SET(IxPublic_DIR "" CACHE PATH "Location of IxPublic")

FIND_PATH(IxPublic_INCLUDE_DIR IxPublicExport.h
	${IxPublic_DIR}/include
    "${IxPublic_DIR}/Public Headers"
    "$ENV{IxPublic_PATH}/Public Headers"
    $ENV{IxPublic_PATH}
    $ENV{IxPublic_DIR}/include
    $ENV{IxPublic_DIR}
    /usr/local/include
	/usr/local/IxPublic/include
    /usr/include
	/usr/IxPublic/include
	/opt/include
    /opt/local/IxPublic/include
    /opt/IxPublic/include
    "C:/IxPublic/include"
	"C:/Program Files/IxPublic/include"
)
MACRO(FIND_IxPublic_LIBRARY MYLIBRARY MYLIBRARYNAME )

FIND_LIBRARY(${MYLIBRARY}
    NAMES
        ${MYLIBRARYNAME}
PATHS
	${IxPublic_DIR}/lib
    $ENV{IxPublic_PATH}
    $ENV{IxPublic_DIR}/lib
    $ENV{IxPublic_DIR}
    /usr/local/lib
	/usr/local/IxPublic/lib
    /usr/lib
	/usr/IxPublic/lib
	/opt/lib
    /opt/local/IxPublic/lib
    /opt/IxPublic/lib
    "C:/IxPublic/lib"
	"C:/Program Files/IxPublic/lib"
)
ENDMACRO(FIND_IxPublic_LIBRARY LIBRARY LIBRARYNAME)

FIND_IxPublic_LIBRARY(IxPublic_LIBRARY IxPublic )
FIND_IxPublic_LIBRARY(IxPublic_LIBRARY_DEBUG IxPublicd)

SET(IxPublic_FOUND FALSE)
IF (IxPublic_INCLUDE_DIR AND IxPublic_LIBRARY)
   SET(IxPublic_FOUND TRUE)
ENDIF (IxPublic_INCLUDE_DIR AND IxPublic_LIBRARY)
