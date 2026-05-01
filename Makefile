CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

TARGET = rtree

SRC = src/main.cpp src/disk.cpp src/rtree.cpp src/nearest.cpp src/str.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

clean-outputs:
	rm -f outputs/trees/*.bin outputs/results/*.csv outputs/results/*.txt outputs/plots/*.png outputs/plots/*.pdf

clean-all: clean clean-outputs

check-random:
	./$(TARGET) check-data data/random.bin 32

check-europa:
	./$(TARGET) check-data data/europa.bin 32

.PHONY: all clean clean-outputs check-random check-europa