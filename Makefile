CXX = g++

CXXFLAGS = -std=c++11 -Wall -g -Iinclude

LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

TARGET = jogo

SRCDIR = src
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.cpp)

OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))


all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linkando para criar o executável..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Compilação concluída!"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	@echo "Limpando arquivos compilados..."
	rm -rf $(OBJDIR) $(TARGET)
	@echo "Limpeza concluída."

.PHONY: all clean
