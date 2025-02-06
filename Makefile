CFLAGS = -std=c99 -W -Wall -Ofast
# CFLAGS += -g -fsanitize=address
LDFLAGS = -lm
STREAMS = fixed sine bit_plane fax_machine
TRANSFORMS = xor bwt
CODERS = copy rle_byte rle_zeros rle_switch freq_varint arithmetic

test: $(STREAMS) $(CODERS)
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
	$(foreach coder,$(CODERS),./bit_plane g lena_cdf53.pgm | ./$(coder) e | ./$(coder) d | ./bit_plane v lena_cdf53.pgm;)
	$(foreach coder,$(CODERS),./fax_machine g lena_fax.pbm | ./$(coder) e | ./$(coder) d | ./fax_machine v lena_fax.pbm;)
	$(foreach coder,$(CODERS),./fax_machine g paper.pbm | ./$(coder) e | ./$(coder) d | ./fax_machine v paper.pbm;)

info: $(STREAMS) $(CODERS)
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
	$(foreach coder,$(CODERS),./bit_plane g lena_cdf53.pgm 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fax_machine g lena_fax.pbm 2> /dev/null | ./$(coder) e > /dev/null;)
	$(foreach coder,$(CODERS),./fax_machine g paper.pbm 2> /dev/null | ./$(coder) e > /dev/null;)

clean:
	rm -f sma $(STREAMS) $(TRANSFORMS) $(CODERS)

