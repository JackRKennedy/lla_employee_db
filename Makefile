TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -f ./mynewdb.db -n
	./$(TARGET) -f ./mynewdb.db -a "John Doe,123 Fake St.,120"
	./$(TARGET) -f ./mynewdb.db -a "Jane Doe,157 Not Real Ave.,107"
	./$(TARGET) -f ./mynewdb.db -a "Mike Hunt,10 Made up Ln.,118"
	./$(TARGET) -f ./mynewdb.db -h

default:  $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude


