
# Points to framework's root directory.
#PARENT = Silver
#ROOT = ../../

#### Metadata ####
NAME = NoteSorter
NAMESPACE := com.h4tch.$(NAME).app
FILENAME := $(shell echo $(NAME) | tr A-Z a-z)
VERSION = 1.0
ICON = icon.png
DESCRIPTION = Quickly sort small notes
PROJECT_TYPE = Application


#### Directories ####
SRCDIR = src
LIBDIR = lib
INCLUDEDIR = include
TESTDIR = test
BUILDDIR = build
DOCDIR = doc
DATADIR = data
THIRDPARTYDIR = third_party
SCRIPTDIR = tools


SOURCES := $(shell find $(SRCDIR) -type f -name "*.cpp")
INCLUDES = -I$(INCLUDEDIR) -I$(SRCDIR) -I$(THIRDPARTYDIR) -I$(ROOT)/$(INCLUDEDIR)/
LIBS = -L$(LIBDIR)/$(SYSTEM) -L$(THIRDPARTYDIR) -L$(ROOT)/$(LIBDIR)/$(SYSTEM) -L. -L$(ROOT)/$(THIRDPARTYDIR)
STATICLIBS = -static-libstdc++ -static-libgcc
DEFINES = 
CXXFLAGS = -c -fPIC -std=c++11 -Wall -pedantic -pthread -frtti -fexceptions \
			-fvisibility=hidden -fvisibility-inlines-hidden \
			-ffunction-sections -fdata-sections
LDFLAGS = -fuse-ld=gold -Wl,--gc-sections,-Bdynamic,-rpath=$$ORIGIN/$(LIBDIR)/$(SYSTEM)/
LIBFLAGS = -export-dynamic -shared

VPATH += $(SRCDIR) $(INCLUDEDIR) $(ROOT)/$(INCLUDEDIR)


define RELEASE_PROFILE
	# Todo Optimizations
	CXXFLAGS := $(CXXFLAGS)
	LDFLAGS := $(LDFLAGS)
	DEFINES := $(DEFINES) -DNDEBUG 
endef
define DEBUG_PROFILE
	CXXFLAGS := -g -fno-pie $(CXXFLAGS)
	LDFLAGS := -g -fno-pie $(LDFLAGS)
	DEFINES := $(DEFINES) -DDEBUG 
endef


define WINDOWS_PROFILE
	EXT := .exe
	LIBEXT := .dll
	LIBEXTSTATIC := .lib
	LIBS := -lmingw32 -lwinpthread $(LIBS)
	DEFINES := $(DEFINES) -DWINDOWS -DWIN32
	LIBFLAGS := $(LIBFLAGS) -Wl,-out-implib,lib$(NAME)$(LIBEXT).a
	# The CXX_PREFIX should probably be different depending on HOST_OS too.
	ifeq ($(HOST_ARCH),x86)
		CXX_PREFIX = i686-w64-mingw32-
	else ifeq ($(HOST_ARCH),x86_64)
		CXX_PREFIX = x86_64-w64-mingw32-
	endif
	CXX = $(CXX_PREFIX)g++
	AR = $(CXX_PREFIX)ar
endef

define LINUX_PROFILE
	ifeq ($(OS), x86)
	endif
	EXT := 
	LIBEXT := .so
	LIBEXTSTATIC := .a
	LIBS := $(LIBS) -ldl
	DEFINES := $(DEFINES) -DLINUX
	LIBFLAGS := $(LIBFLAGS) -Wl,-soname,lib$(NAME)$(LIBEXT)
	CXX = g++
	AR = ar
endef

define MAC_PROFILE
	EXT := 
	LIBEXT := .dylib
	LIBEXTSTATIC := .a
	LIBS := $(LIBS) -ldl
	DEFINES := $(DEFINES) -DOSX
	LIBFLAGS := $(LIBFLAGS) -dynamiclib -Wl,-dylib-install_name,lib$(NAME)$(LIBEXT)
	CXX = g++
	AR = ar
endef


define X86_PROFILE
	DEFINES := $(DEFINES) -DX86
	CXX = $(CXX) -m32
endef

define X86_64_PROFILE
	DEFINES := $(DEFINES) -DX86_64
	CXX = $(CXX) -m64
endef

define ARM_PROFILE
	DEFINES := $(DEFINES) -DARM
endef


# Determines the platforms you will be building on.
BUILD_ON_LINUX = 1
BUILD_ON_MAC = 0
BUILD_ON_WINDOWS = 1

# Determines what scripts and libraries are copied into your project.
BUILD_FOR_LINUX_32 = 1
BUILD_FOR_LINUX_64 = 1
BUILD_FOR_MAC_32 = 0
BUILD_FOR_MAC_64 = 0
BUILD_FOR_WINDOWS_32 = 1
BUILD_FOR_WINDOWS_64 = 1
BUILD_FOR_ANDROID_ARM = 0
BUILD_FOR_ANDROID_X86 = 0


#PACKAGE_ALL_IN_ONE=0

PACKAGE_ARCHIVE_TYPE_LINUX = tar.gz
PACKAGE_ARCHIVE_TYPE_WINDOWS = zip


#### Documentation Settings ####
# Customize the Doxyfile for greater control.
DOCSET_NAME = Silver.$(NAME) Documentation
PUBLISHERNAME = Silver.$(NAME)
PUBLISHER_NAMESPACE = com.Silver.app
# Don't document source from these paths.
HIDE_DOC_WITHIN_PATHS =
# Don't document source files that match the patterns.
# Ex Pattern: "*/test/*"
HIDE_DOC_WITH_PATTERNS =
# Hide part of the user's include path from the docs.
# Ex: Turn "src/Lib/lib.h" into "/Lib/lib.h"
HIDE_INC_PATH_PREFIX =
# Hide namespace, classes, functions, etc from the docs.
# Ex: "Lib::*Test Lib::Test*"
HIDE_DOC_WITH_SYMBOLS =
# Directory containing images for documentation. Used with image tag.
DOC_IMAGE_DIR =


