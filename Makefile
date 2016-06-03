NAME = sensor_bridge

# Flags
CFLAGS = -fdump-rtl-expand

# Sources
SRC = src

# Includes
INCLUDE = inc/

# MAVLink library
MAVLIB = mavlink/include/mavlink/v1.0/

# Libs
LIBS = -lm

# Linker
OBJS = $(NAME).o interface.o serial_port.o mfunctions.o

all: $(NAME)

$(NAME): $(OBJS)
	g++ $(OBJS) -o $(NAME) 

$(NAME).o: $(SRC)/$(NAME).cpp
	g++ $(CFLAGS) -I$(INCLUDE) -I$(MAVLIB) $(LIBS) -c $(SRC)/$(NAME).cpp 
	g++ $(CFLAGS) -I$(INCLUDE) -I$(MAVLIB) $(LIBS) -c $(SRC)/interface.cpp
	g++ $(CFLAGS) -I$(INCLUDE) -I$(MAVLIB) $(LIBS) -c $(SRC)/serial_port.cpp
	g++ $(CFLAGS) -I$(INCLUDE) -I$(MAVLIB) $(LIBS) -c $(SRC)/mfunctions.cpp

graph: $(NAME)
	mkdir Graphs
	mv *.expand Graphs/
	egypt Graphs/*.expand | dot -Tsvg -Grankdir=LR -o Graphs/$(NAME).svg
	eog Graphs/$(NAME).svg

clean:
	rm -rf *.o $(NAME) Graphs/ *.expand
