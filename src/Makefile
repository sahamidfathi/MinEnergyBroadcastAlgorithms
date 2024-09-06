CXX = g++

TARGET = a.out
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) locations.txt

clean:
	rm -f $(TARGET)

