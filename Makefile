# Исходные и заголовочные файлы
SOURCES=$(wildcard *.cpp)
HEADERS=$(SOURCES:.cpp=.h)

# Флаги компиляции
FLAGS=-g

# Аргумент командной строки для программы
NUMBER_OF_MPC=1

# Цель по умолчанию
all: main tags

# Компиляция основного исполняемого файла
main: $(SOURCES) $(HEADERS) Makefile
	mpiCC $(SOURCES) $(FLAGS) -o main

# Очистка скомпилированных файлов
clean:
	rm -f main a.out

# Проверка и создание тегов
tags: $(SOURCES) $(HEADERS)
	@if command -v ctags >/dev/null 2>&1; then \
		ctags -R . ; \
	else \
		echo "ctags not found, skipping tags generation."; \
	fi

# Запуск программы
run: main Makefile tags
	mpirun -oversubscribe -np 9 ./main $(NUMBER_OF_MPC)
