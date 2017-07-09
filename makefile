

risk: main.o risk.o sfvm.o
	g++ -g main.o risk.o sfvm.o -o risk -Wall -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp sfvm.hpp risk.hpp
	g++ -c -std=c++11 main.cpp -o main.o

risk.o: risk.cpp risk.hpp
	g++ -c -std=c++11 risk.cpp -o risk.o

sfvm.o: sfvm.cpp sfvm.hpp
	g++ -c -std=c++11 sfvm.cpp -o sfvm.o

clean:
	rm -f *.o *~ risk
