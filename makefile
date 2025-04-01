#TODO
CC = gcc
CFLAGS = -fsanitize=address -lm -fPIC


sound_seg.o: src/sound_seg.c
	${CC} -c -g src/sound_seg.c -o sound_seg.o ${CFLAGS}

sound_seg: sound_seg.o
	${CC} sound_seg.o -o bin/sound_seg ${CFLAGS}

clean:
	rm sound_seg.o
	rm bin/sound_seg