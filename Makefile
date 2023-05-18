######################################
### MACROS
######################################

# $(1): compiler
# $(2): Object file to generate
# $(3): Source file
# $(4): Compiler flags
# $(5): Other dependencies

define COMPILE
$(2): $(3) $(5)
	$(1) -c -o $(2) $(3) $(4) 
endef

# $(1): Source file
define C2O
$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst $(SRC)%,$(OBJ)%,$(1))))
endef

# $(1): Source file
define C2H
$(patsubst %.c,%.h,$(patsubst %.cpp,%.hpp,$(1)))
endef

#### MACROS END ######################


######################################
### CONFIG
######################################
APP 	:= app
CC		:= g++
CCFLAGS := -Wall -pedantic -std=c++20 -Wno-unused-variable
MKDIR 	:= mkdir -p
RM		:= rm -rf
SRC		:= src
OBJ 	:= obj

ALLCS			:= $(shell find $(SRC) -type f -iname *.c)
ALLCHEADERS		:= $(shell find $(SRC) -type f -iname *.h)
ALLCPPS			:= $(shell find $(SRC) -type f -iname *.cpp)
ALLCPPHEADERS	:= $(shell find $(SRC) -type f -iname *.hpp)
ALLOBJ			:= $(foreach F,$(ALLCS) $(ALLCPPS),$(call C2O,$(F)))
SUBDIRS 		:= $(shell find $(SRC) -type d)
OBJSUBDIRS 		:= $(patsubst $(SRC)%,$(OBJ)%,$(SUBDIRS))

#### CONFIG END ######################

.PHONY: info clean all

#################
# EXTERNAL LIBS #
#################
GLEW_LIBS			:=$(shell pkg-config --libs glew)
GLEW_INCLUDES		:=$(shell pkg-config --cflags glew)

GLFW_LIBS			:=$(shell pkg-config --libs glfw3)
GLFW_INCLUDES		:=$(shell pkg-config --cflags glfw3)

BULLET_LIBS			:=$(shell pkg-config --libs bullet)
BULLET_INCLUDES		:=$(shell pkg-config --cflags bullet)

#FT_LIBS			:=$(shell pkg-config --libs freetype2)
#FT_INCLUDES		:=$(shell pkg-config --cflags freetype2)

STB_LIBS			:=$(shell pkg-config --libs stb)
STB_INCLUDES		:=$(shell pkg-config --cflags stb)

ASSIMP_LIBS			:=$(shell pkg-config --libs assimp) -lz -lminizip
ASSIMP_INCLUDES		:=$(shell pkg-config --cflags assimp)

ALL_INCLUDES		:=$(GLEW_INCLUDES) $(GLFW_INCLUDES) $(BULLET_INCLUDES) $(FT_INCLUDES) $(STB_INCLUDES) $(ASSIMP_INCLUDES)
ALL_LIBS 			:=$(GLEW_LIBS) $(GLFW_LIBS) $(BULLET_LIBS) $(FT_LIBS) $(STB_LIBS) $(ASSIMP_LIBS) -lpthread -ldl 



##########################
# LINK EVERYTHING
##########################
all: $(APP)
$(APP): $(OBJSUBDIRS) $(ALLOBJ)
	$(CC) $(CCFLAGS) -o $(APP) $(ALLOBJ) $(ALL_LIBS)

##########################
# COMPILE INTO OBJECTS
##########################
objs: $(OBJSUBDIRS) $(ALLOBJ)

##########################
# CPP FILE COMPILE RULES
##########################
$(foreach F, $(ALLCPPS), \
	$(eval $(call COMPILE,$(CC),$(call C2O,$(F)),$(F),$(CCFLAGS) $(ALL_INCLUDES), $(call C2H,$(F)) $(OTHER_DEPENDENCIES))))


$(OBJSUBDIRS):
	$(MKDIR) $(OBJSUBDIRS)

%.h:
	@touch $@
%.hpp:
	@touch $@

info:
	$(info Source dir:  	$(SUBDIRS))
	$(info Objects dir: 	$(OBJSUBDIRS))
	$(info C files:     	$(ALLCS))
	$(info Cpp files:   	$(ALLCPPS))
	$(info Header files:	$(ALLCPPHEADERS) $(ALLCHEADERS))
	$(info Objects:     	$(ALLOBJ) )
	
clean:
	$(RM) $(OBJ) $(APP)