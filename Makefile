CXX = g++

TARGET_NAME = jogo
TARGET = $(TARGET_NAME)

SRCDIR = src
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

CXXFLAGS = -std=c++11 -Wall -g -Iinclude
LDFLAGS =
LIBS =

ifeq ($(OS),Windows_NT)
SDL_PATHS ?= C:/SDL-2.32.10

CXXFLAGS += -I$(SDL_PATHS)/include
LDFLAGS += -L$(SDL_PATHS)/lib
LIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
TARGET := $(TARGET).exe
MKDIR_CMD = if not exist "$(OBJDIR)" mkdir "$(OBJDIR)"
RMDIR_CMD = if exist "$(OBJDIR)" rmdir /s /q "$(OBJDIR)"
RMEXE_CMD = if exist "$(TARGET)" del /q "$(TARGET)"
else
LIBS += -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
MKDIR_CMD = mkdir -p $(OBJDIR)
RMDIR_CMD = rm -rf $(OBJDIR)
RMEXE_CMD = rm -f $(TARGET)
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linkando para criar o executável..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS)
	@echo "Compilação concluída!"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	$(MKDIR_CMD)

clean:
	@echo "Limpando arquivos compilados..."
	$(RMDIR_CMD)
	$(RMEXE_CMD)
	@echo "Limpeza concluída."

.PHONY: all clean
