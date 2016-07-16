all: joy

joy: main.o joystick.o
	g++ main.o joystick.o -o joy

main.o: main.cpp
	g++ -c main.cpp

joystick.o: joystick.cpp
	g++ -c joystick.cpp

clean:
	rm *o joy
