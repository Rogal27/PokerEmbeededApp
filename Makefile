CC=$(CROSS_COMPILE)g++
LIBS=-lgpiod 
FILES=prog.cpp card.cpp
TARGET=prog

all: ${TARGET}
${TARGET}: ${FILES}
	${CC} ${LIBS} -o ${TARGET} ${FILES}
$(OBJS) : %.o : %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean
	rm -f ${TARGET}
