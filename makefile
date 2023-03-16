PWD := $(shell pwd)
client_src = $(wildcard ./src/InetAddress.cpp) client.cpp
client = client
server_src = $(wildcard ./src/*.cpp) server.cpp
server = server
INC = -I $(wildcard ./include)
CINC = -I$(PWD)/include
$(server):
	$(CXX) $(client_src) $(CINC) -o $(client) && \
	$(CXX) $(server_src) $(INC) -o $(server) -std=c++11 -g -pthread
.PHONY:clean
clean:
	rm server && rm client
	
