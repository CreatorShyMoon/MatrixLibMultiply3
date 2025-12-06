CC = g++
CFLAGS = -O2 -Wall -fPIC
INCLUDES = -I include
SRC = src/MatrixLib.cpp
OBJ = src/MatrixLib.o
STATIC_LIB = lib/libMatrixLib.a
DYNAMIC_LIB = lib/libMatrixLib.dylib

# Цель по умолчанию — сборка обоих типов библиотек
all: static dynamic

# Сборка статической библиотеки
static: $(OBJ)
	ar rcs $(STATIC_LIB) $(OBJ)
	@echo "Static library $(STATIC_LIB) has been created."

# Сборка динамической библиотеки
dynamic: $(OBJ)
	$(CC) -shared -o $(DYNAMIC_LIB) $(OBJ)
	@echo "Dynamic library $(DYNAMIC_LIB) has been created."

# Компиляция исходников
$(OBJ): $(SRC)
	$(CC) -c $(SRC) $(CFLAGS) $(INCLUDES) -o $(OBJ)

# Очистка
clean:
	rm -f $(OBJ) $(STATIC_LIB) $(DYNAMIC_LIB)
	@echo "Cleaned."