NAME = HomeLab

SRC_DIR     = srcs/
OBJ_DIR     = obj/
INC_DIR     = include/

SRC_FILES   = main.cpp \
			  config/Config.cpp \
              core/Server.cpp \
			  core/Server_handleClientData.cpp \
			  core/Server_handleRequests.cpp \
              http/HttpRequest.cpp \
              http/HttpResponse.cpp \
			  cloud/cloudManager.cpp
	
SRC = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ = $(addprefix $(OBJ_DIR), $(SRC_FILES:.cpp=.o))

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I $(INC_DIR)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re