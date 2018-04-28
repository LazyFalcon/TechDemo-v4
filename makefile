DIRECTORIES = $(addprefix -I./,$(shell ls -d ./src/*/))
DIRECTORIES_2 = $(addprefix -I./,$(shell ls -d ./src/*/*/))
TARGET_NAME = TDv4

CXX_FLAGS = -isystem C:\MinGW\include -std=c++17 -O2 -msse2 -mfpmath=sse -g -pipe -I. -I./src $(DIRECTORIES) $(DIRECTORIES_2) -DBT_USE_DOUBLE_PRECISION=ON -DUSE_BULLET
# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
ADDITIONAL_FLAGS = \
-Werror=return-type
# -Wunused-function \
# -Wswitch-enum \

CXX = C:\MinGw\bin\g++.exe
FFMPEG = C:\ffmpeg\bin\ffmpeg.exe
SRC = ./src
BIN = ./bin
OBJ_DIR = ./obj

LIBS = -lPMK-audio -lboost_system -lboost_filesystem -lboost_thread -lboost_date_time -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath \
-lglfw3 -lgdi32 -lglu32 -lopengl32 -lassimp.dll -lFreeImage -lFreeImagePlus -lpng -ljpeg -lz -lOpenAL32 -lvorbisfile  -lvorbis -logg -lvorbisenc -lFLAC
# -lglew32

TARGETS = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)

OBJS = $(TARGETS:%.cpp=$(OBJ_DIR)/%.o)

DEP = $(OBJS:%.o=%.d)

$(BIN)/$(TARGET_NAME): $(OBJS) ./obj/res.o
	@mkdir -p ./bin
	@echo "Linking: $@"
	@$(CXX) $^ -o $@ $(LIBS)
	@echo "Done"

-include $(DEP)

$(OBJ_DIR)/%.o : %.cpp
	@echo "Compiling: $< "
	@mkdir -p $(@D)
	@$(CXX) $(CXX_FLAGS) $(ADDITIONAL_FLAGS) -MMD -c $< -o $@

$(OBJ_DIR)/res.o: ./resource.rc ./icon.ico
	windres ./resource.rc ./obj/res.o

clean:
	rm -rf $(OBJ_DIR)
	rm $(BIN)/$(TARGET_NAME).exe

run: $(BIN)/$(TARGET_NAME)
	(cd $(BIN) && ./$(TARGET_NAME).exe)

debug: $(BIN)/$(TARGET_NAME)
	(cd $(BIN) && gdb $(TARGET_NAME).exe -ex=run)

.PHONY: clean
