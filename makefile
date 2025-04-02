#TODO
CC = gcc
CFLAGS = -fsanitize=address -lm -fPIC
MAIN = src/tests/main.c
SOURCE = src/track.c src/wav.c src/helper.c src/list_op.c src/child_parent.c
OBJECTS = track.o wav.o helper.o list_op.o child_parent.o
OBJECT_MERGE = sound_seg.o
EXECUTABLE = bin/sound_seg


target: ${OBJECT_MERGE}

sound_seg: ${OBJECT_MERGE} ${MAIN}
	${CC} ${OBJECT_MERGE} ${MAIN} -o ${EXECUTABLE} ${CFLAGS}


# Link all object files to get target object.
${OBJECT_MERGE}:${OBJECTS}
	ld -r -o $@ $^


%.o:src/%.c
	${CC} -c $< -o $@


tests: test1 test2 test3 test4 test5 test6


test1: src/tests/test1.c ${OBJECT_MERGE}
	${CC} ${OBJECT_MERGE} src/tests/test1.c -o tests/test1/program ${CFLAGS}


test2: src/tests/test2.c ${OBJECT_MERGE}
	${CC} ${OBJECT_MERGE} src/tests/test2.c -o tests/test2/program ${CFLAGS}


test3: src/tests/test3.c ${OBJECT_MERGE}
	${CC} ${OBJECT_MERGE} src/tests/test3.c -o tests/test3/program ${CFLAGS}


test4: src/tests/test4.c ${OBJECT_MERGE}
	${CC} ${OBJECT_MERGE} src/tests/test4.c -o tests/test4/program ${CFLAGS}


test5: src/tests/test5.c ${OBJECT_MERGE}
	${CC} ${OBJECT_MERGE} src/tests/test5.c -o tests/test5/program ${CFLAGS}


test6: src/tests/test6.c ${OBJECT_MERGE}
	${CC} ${OBJECT_MERGE} src/tests/test6.c -o tests/test6/program ${CFLAGS}


clean:clean_object clean_exe clean_tests


clean_object:
	rm -f ${OBJECTS} ${OBJECT_MERGE}


clean_exe:
	rm -f ${EXECUTABLE}


clean_tests:
	rm -f tests/test1/program
	rm -f tests/test2/program
	rm -f tests/test3/program
	rm -f tests/test4/program
	rm -f tests/test5/program
	rm -f tests/test6/program