SET(IxKernel_DIR ${CMAKE_INSTALL_PREFIX}/IxKernel CACHE STRING "settting IxKernel_DIR path" FORCE)

#SET(IxKernel_DIR "" CACHE PATH "Location of IxKernel")

FIND_PATH(IxKernel_INCLUDE_DIR IxKernelExport.h
	${IxKernel_DIR}/include
    "${IxKernel_DIR}/Public Headers"
    "$ENV{IxKernel_PATH}/Public Headers"
    $ENV{IxKernel_PATH}
    $ENV{IxKernel_DIR}/include
    $ENV{IxKernel_DIR}
    /usr/local/include
	/usr/local/IxKernel/include
    /usr/include
	/usr/IxKernel/include
	/opt/include
    /opt/local/IxKernel/include
    /opt/IxKernel/include
    "C:/IxKernel/include"
	"C:/Program Files/IxKernel/include"
)
MACRO(FIND_IxKernel_LIBRARY MYLIBRARY MYLIBRARYNAME )

FIND_LIBRARY(${MYLIBRARY}
    NAMES
        ${MYLIBRARYNAME}
PATHS
	${IxKernel_DIR}/lib
    $ENV{IxKernel_PATH}
    $ENV{IxKernel_DIR}/lib
    $ENV{IxKernel_DIR}
    /usr/local/lib
	/usr/local/IxKernel/lib
    /usr/lib
	/usr/IxKernel/lib
	/opt/lib
    /opt/local/IxKernel/lib
    /opt/IxKernel/lib
    "C:/IxKernel/lib"
	"C:/Program Files/IxKernel/lib"
)
ENDMACRO(FIND_IxKernel_LIBRARY LIBRARY LIBRARYNAME)

FIND_IxKernel_LIBRARY(IxKernel_LIBRARY IxKernel)
FIND_IxKernel_LIBRARY(IxKernel_LIBRARY_DEBUG IxKerneld)

SET(IxKernel_FOUND FALSE)
IF (IxKernel_INCLUDE_DIR AND IxKernel_LIBRARY)
   SET(IxKernel_FOUND TRUE)
ENDIF (IxKernel_INCLUDE_DIR AND IxKernel_LIBRARY)
