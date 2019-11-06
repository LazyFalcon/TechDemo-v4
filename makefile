DIRECTORIES = $(addprefix -I./,$(shell ls -d ./src/*/))
DIRECTORIES_2 = $(addprefix -I./,$(shell ls -d ./src/*/*/))
DIRECTORIES_3 = $(addprefix -I./,$(shell ls -d ./src/*/*/*/)) # todo: shouldn't be here, should be left for factories
TARGET_NAME = TechDemo

DEFINES = \
-DGLM_ENABLE_EXPERIMENTAL \
-DGLM_FORCE_SWIZZLE \
-DGLM_SWIZZLE \
-DGLM_FORCE_RADIANS \
-DUSE_BULLET
# -DBT_USE_DOUBLE_PRECISION=ON

CORE_PCH_FILENAME=./src/core_pch.hpp
CORE_PCH=$(CORE_PCH_FILENAME).gch

CXX_FLAGS = -isystem C:\MinGW\include -std=c++17 -O2 -msse2 -mfpmath=sse -g -pipe -I. -I./src $(DIRECTORIES) $(DIRECTORIES_2) $(DIRECTORIES_3) $(DEFINES)
# CXX_FLAGS = -std=c++17 -O2 -msse2 -mfpmath=sse -g -pipe -I. -I./src $(DIRECTORIES) $(DIRECTORIES_2) $(DIRECTORIES_3) $(DEFINES)
# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# https://packages.msys2.org/base
EXTRA_CXX_FLAGS = -Werror=return-type
EXTRA_CXX_FLAGS += -Werror=reorder
EXTRA_CXX_FLAGS += -Wuninitialized
EXTRA_CXX_FLAGS += -Winvalid-pch
# EXTRA_CXX_FLAGS += -Wshadow
# EXTRA_CXX_FLAGS += -Wmaybe-uninitialized
# EXTRA_CXX_FLAGS += -Wal
# EXTRA_CXX_FLAGS += -Wunused-function
# EXTRA_CXX_FLAGS += -Wswitch-enum

ifeq ("$(clang)", "yes")
	# echo "CLANG INCOMING!"
	CXX = C:\msys64\mingw64\bin\clang++.exe -stdlib=libstdc++
	LDX = C:\MinGW\bin\g++.exe
else
	# CXX = C:\msys64\mingw64\bin\g++.exe
	CXX = C:\MinGW\bin\g++.exe
	LDX = $(CXX)
endif


# LDX = C:\msys64\mingw64\bin\ld.exe

FFMPEG = C:\ffmpeg\bin\ffmpeg.exe
SRC = ./src
BIN = ./bin
OBJ_DIR = ./obj

LIBS = -lPMK-audio -lboost_system-mt -lboost_filesystem-mt -lboost_thread-mt -lboost_date_time-mt -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath \
-lglfw3 -lgdi32 -lglu32 -lopengl32 -lassimp.dll -lFreeImage -lFreeImagePlus -lpng -ljpeg -lz -lOpenAL32 -lvorbisfile  -lvorbis -logg -lvorbisenc -lFLAC

TARGETS = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)  $(wildcard src/*/*/*/*.cpp)

OBJS = $(TARGETS:%.cpp=$(OBJ_DIR)/%.o)

DEP = $(OBJS:%.o=%.d)
# include($(shell find . -type f -name \*.d))

build: ./src/core_pch.hpp.gch $(BIN)/$(TARGET_NAME)
	@echo "Done"

run: ./src/core_pch.hpp.gch $(BIN)/$(TARGET_NAME)
	@echo "Starting $(TARGET_NAME)"
	@$(BIN)/$(TARGET_NAME).exe


$(BIN)/$(TARGET_NAME): $(OBJS) ./obj/res.o
	@mkdir -p ./bin
	@echo "Linking: $@"
	@$(LDX) $^ -o $@ $(LIBS)
	@echo "Done"

-include $(DEP)

$(OBJ_DIR)/%.o : %.cpp
	@echo "Compiling: $< "
	@mkdir -p $(@D)
	@$(CXX) $(CXX_FLAGS) $(EXTRA_CXX_FLAGS) -MMD  -c $< -o $@

$(OBJ_DIR)/res.o: ./resource.rc ./icon.ico
	windres ./resource.rc ./obj/res.o

./src/core_pch.hpp.gch : ./src/core_pch.hpp
	@echo "Creating PCH"
	@$(CXX) $(CXX_FLAGS) $(EXTRA_CXX_FLAGS) $(CORE_PCH_FILENAME)

clean:
	rm -rf $(OBJ_DIR)
	rm $(BIN)/$(TARGET_NAME).exe


debug: $(BIN)/$(TARGET_NAME)
	gdb $(BIN)/$(TARGET_NAME).exe -ex=run

format:
	find src/ -iname *.hpp -o -iname *.cpp | xargs clang-format -i

.PHONY: clean
