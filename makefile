LDFLAGS := -Wall -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
CPPFLAGS := -c -std=c++11

all: risk server client
	make clean

risk: main.o risk.o sfvm.o movement.o screens.o
	if [ ! -d "bin" ]; then mkdir bin; fi
	g++ -g main.o risk.o sfvm.o movement.o screens.o -o bin/risk $(LDFLAGS)

server: server.o risk.o sfvm.o movement.o network_screens.o networking.o
	if [ ! -d "bin" ]; then mkdir bin; fi
	g++ -g server.o risk.o sfvm.o movement.o network_screens.o networking.o -o bin/server $(LDFLAGS)

client: client.o risk.o sfvm.o movement.o network_screens.o networking.o
	if [ ! -d "bin" ]; then mkdir bin; fi
	g++ -g client.o risk.o sfvm.o movement.o network_screens.o networking.o -o bin/client $(LDFLAGS)

%.o: %.cpp
	g++ $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o *~
