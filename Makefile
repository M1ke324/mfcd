all: mfcd

mfcd: mfcd.c
	$(CC) mfcd.c -o mfcd -Wall -Wextra -pedantic

install: mfcd
	mkdir -p $(DESTDIR)/usr/local/bin
	cp mfcd  $(DESTDIR)/usr/local/bin/
	mkdir -p $(DESTDIR)/usr/local/man1
	cp man/mfcd.1 $(DESTDIR)/usr/local/share/man/man1/mfcd.1

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/mfcd
	rm -f $(DESTDIR)/usr/local/share/man/man1/mfcd.1
clean:
	rm -f mfcd
