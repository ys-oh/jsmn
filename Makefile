# You can put your build options here
-include config.mk

test: test_default test_strict test_links test_strict_links
test_default: test/tests.c jsmn.h
	$(CC) -DJSMN_BUILD -I. $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_strict: test/tests.c jsmn.h
	$(CC) -DJSMN_STRICT=1 -DJSMN_BUILD -I. $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_links: test/tests.c jsmn.h
	$(CC) -DJSMN_PARENT_LINKS=1 -DJSMN_BUILD -I. $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_strict_links: test/tests.c jsmn.h
	$(CC) -DJSMN_STRICT=1 -DJSMN_PARENT_LINKS=1 -DJSMN_BUILD -I. $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@

jsmn_helper: example/jsmn_helper.c jsmn.h jsmn_helper.h
	$(CC) -DJSMN_BUILD -I. $(LDFLAGS) $< -o $@

simple_example: example/simple.c jsmn.h
	$(CC) -DJSMN_BUILD -I.  $(LDFLAGS) $< -o $@

jsondump: example/jsondump.c jsmn.h
	$(CC) -DJSMN_BUILD -I.  $(LDFLAGS) $< -o $@

fmt:
	clang-format -i jsmn.h test/*.[ch] example/*.[ch]

lint:
	clang-tidy jsmn.h --checks='*'

clean:
	rm -f *.o example/*.o
	rm -f simple_example
	rm -f jsondump

.PHONY: clean test

