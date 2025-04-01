#TODO
CFLAGS = -fsanitize=address -lm -fPIC

sound_seg.o: sound_seg.c
	${CC} -g sound_seg.c -o sound_seg.o ${CFLAGS}

clean:
	rm bin/*