#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_PATH
# GLEW_LIBRARY
# 

IF (WIN32)
    FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
        ${GLEW_ROOT_DIR}/include
        DOC "The directory where GL/glew.h resides")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(GLEWNAMES glew GLEW glew64 glew64s)
        FIND_LIBRARY( GLEW_LIBRARY
          NAMES ${GLEWNAMES}
	      PATHS
          ${GLEW_ROOT_DIR}/bin/x86_64
          ${GLEW_ROOT_DIR}/lib/x86_64
          DOC "The GLEW library")
    else ()
        set(GLEWNAMES glew GLEW glew32 glew32s)
        FIND_LIBRARY( GLEW_LIBRARY
          NAMES ${GLEWNAMES}
	      PATHS
          ${GLEW_ROOT_DIR}/bin/x86
          ${GLEW_ROOT_DIR}/lib/x86
          DOC "The GLEW library")
    endif (CMAKE_SIZEOF_VOID_P EQUAL 8)  
    
    
ELSE (WIN32)
    FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where GL/glew.h resides")
    FIND_LIBRARY( GLEW_LIBRARY
        NAMES GLEW glew
        PATHS
		# path on Gordon's notebook mac
		/opt/local/lib  # 64bit on mac notebook
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        DOC "The GLEW library"
		NO_DEFAULT_PATH)
ENDIF (WIN32)

IF (GLEW_INCLUDE_PATH)
    SET( GLEW_FOUND 1 CACHE STRING "Set to 1 if GLEW is found, 0 otherwise")
ELSE (GLEW_INCLUDE_PATH)
    SET( GLEW_FOUND 0 CACHE STRING "Set to 1 if GLEW is found, 0 otherwise")
ENDIF (GLEW_INCLUDE_PATH)

MARK_AS_ADVANCED( GLEW_FOUND )
