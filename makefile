CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I./include

all: Snake.out

Snake.out: src/Snake.cpp #src/Creature.cpp src/Player.cpp src/Enemy.cpp src/GameCycle.cpp src/GameManager.cpp
	$(CXX) $(CXXFLAGS) src/Snake.cpp 
#src/Creature.cpp src/Player.cpp src/Enemy.cpp src/GameCycle.cpp src/GameManager.cpp -o JRPG.out
clean:
	rm -f *.out

.PHONY: all clean

#17
# Breyta þessu líka eftir nýja verkefninu