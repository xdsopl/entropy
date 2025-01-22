CFLAGS = -std=c99 -W -Wall -Ofast
# CFLAGS += -g -fsanitize=address
LDFLAGS = -lm
CODERS = copy rle_byte rle_zeros rle_switch freq_varint

test: fixed sine $(CODERS)
	$(foreach coder,$(CODERS),./fixed g 1234 1 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 1;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.99 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 0.99;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.9 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 0.9;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.75 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 0.75;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.5 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 0.5;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.25 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 0.25;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.1 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 0.1;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.01 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 0.01;)
	$(foreach coder,$(CODERS),./fixed g 1234 0 | ./$(coder) e | ./$(coder) d | ./fixed v 1234 0;)
	$(foreach coder,$(CODERS),./sine g 1234 10 | ./$(coder) e | ./$(coder) d | ./sine v 1234 10;)

info: fixed sine $(CODERS)
	$(foreach coder,$(CODERS),./fixed g 1234 1 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.99 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.9 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.75 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.5 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.25 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.1 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fixed g 1234 0.01 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fixed g 1234 0 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./sine g 1234 10 2> /dev/null | ./$(coder) e > /dev/null;)

clean:
	rm -f sma sine fixed $(CODERS)

