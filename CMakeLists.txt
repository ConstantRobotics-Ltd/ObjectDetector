cmake_minimum_required(VERSION 3.13)



################################################################################
## PROJECT
## name and version
################################################################################
project(ObjectDetector LANGUAGES CXX)



################################################################################
## SETTINGS
## basic project settings before use
################################################################################
# If this project is used as a submodule, the variable should be overridden
# to "OFF" in the top-level application (to disable forced cache rewriting)
option(${PARENT}_SUBMODULE_CACHE_OVERWRITE "Enable forced cache rewriting" ON)
if (${PARENT}_SUBMODULE_CACHE_OVERWRITE)
    SET(REWRITE_FORCE "FORCE")
else()
    SET(REWRITE_FORCE "")
endif()



################################################################################
## CONFIGURATION
## project configuration
################################################################################
SET(${PARENT}_OBJECT_DETECTOR                ON  CACHE BOOL "" ${REWRITE_FORCE})
SET(${PARENT}_OBJECT_DETECTOR_TEST           ON  CACHE BOOL "" ${REWRITE_FORCE})
SET(${PARENT}_OBJECT_DETECTOR_EXAMPLE        ON  CACHE BOOL "" ${REWRITE_FORCE})



################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the project configuration
################################################################################
if (${PARENT}_OBJECT_DETECTOR)
    add_subdirectory(3rdparty)
    add_subdirectory(src)
endif()

if (${PARENT}_OBJECT_DETECTOR_TEST)
    add_subdirectory(test)
endif()

if (${PARENT}_OBJECT_DETECTOR_EXAMPLE)
    add_subdirectory(example)
endif()
