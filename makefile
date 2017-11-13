LDFLAGS := -Wall -g -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
CPPFLAGS := -c -std=c++14 -g

OBJDIR := obj

CLIENT_SRC := client.cpp risk.cpp sfvm.cpp movement.cpp network_screens.cpp networking.cpp
SERVER_SRC := server.cpp risk.cpp sfvm.cpp movement.cpp network_screens.cpp networking.cpp
RISK_SRC := main.cpp risk.cpp sfvm.cpp movement.cpp screens.cpp

CLIENT_OBJ := $(CLIENT_SRC:%.cpp=$(OBJDIR)/%.o)
SERVER_OBJ := $(SERVER_SRC:%.cpp=$(OBJDIR)/%.o)
RISK_OBJ := $(RISK_SRC:%.cpp=$(OBJDIR)/%.o)


all: risk server client

risk: $(RISK_OBJ)
	if [ ! -d "bin" ]; then mkdir bin; fi
	g++ $(RISK_OBJ) -o bin/risk $(LDFLAGS)

server: $(SERVER_OBJ)
	if [ ! -d "bin" ]; then mkdir bin; fi
	g++ $(SERVER_OBJ) -o bin/server $(LDFLAGS)

client: $(CLIENT_OBJ)
	if [ ! -d "bin" ]; then mkdir bin; fi
	g++ $(CLIENT_OBJ) -o bin/client $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	if [ ! -d "obj" ]; then mkdir obj; fi
	g++ $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o *~
