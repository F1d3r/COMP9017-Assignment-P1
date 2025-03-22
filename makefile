#TODO
CFLAGS = -fsanitize=address

sound_seg: sound_seg.c
	${CC} -o bin/sound_seg sound_seg.c ${CFLAGS}

clean:
	rm bin/*