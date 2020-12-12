SRC_DIRS:=processor .
SRC_FILES:=$(foreach file,$(SRC_DIRS),$(wildcard $(file)/*.cpp))
OBJ_FILES:=$(patsubst %.cpp, %.o, $(SRC_FILES))

CC=g++

NAME:=Machine

all: $(OBJ_FILES)
	$(CC) -o $(NAME) $(OBJ_FILES)

%.o: %.cpp
	$(CC) -c $< -o $@

clean:
	@rm -f -v $(OBJ_FILES)
	@rm -f -v $(NAME)