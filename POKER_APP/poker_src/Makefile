CXX=g++
LIBS=-lgpiod 
FILES=prog.cpp card.cpp poker.cpp
TARGET=poker
OBJS=prog.o card.o poker.o

all: ${TARGET}
${TARGET}: ${FILES}
	${CXX} ${LIBS} -o ${TARGET} ${FILES}
$(OBJS) : %.o : %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

.PHONY: clean
	rm -f ${TARGET}
