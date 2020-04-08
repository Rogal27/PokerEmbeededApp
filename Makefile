CC=$(CROSS_COMPILE)g++
LIBS=-lgpiod 
FILES=prog.cpp
TARGET=prog

all: ${TARGET}
${TARGET}: ${FILES}
	${CC} ${LIBS} -o ${TARGET} ${FILES}
.PHONY: clean
	rm -f ${TARGET}
