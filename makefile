#TODO
CFLAGS = -fsanitize=address

sound_seg: sound_seg.c
<<<<<<< HEAD
	${CC} -o bin/sound_seg sound_seg.c ${CFLAGS}
=======
	${CC} -g sound_seg.c -o bin/sound_seg ${CFLAGS} -lm
>>>>>>> b80c02d (Completed the identify function)

clean:
	rm bin/*