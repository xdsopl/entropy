CFLAGS = -std=c99 -W -Wall -Ofast
# CFLAGS += -g -fsanitize=address
LDFLAGS = -lm
STREAMS = fixed sine bit_plane fax_machine
TRANSFORMS = copy xor bwt
CODERS = copy rle_byte rle_zeros rle_switch freq_varint arithmetic

test: $(STREAMS) $(TRANSFORMS) $(CODERS)
	@$(foreach transform,$(TRANSFORMS),\
		echo "==================== Transform: $(transform) ===================="; \
		echo "--- Testing FIXED stream of 100% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 1 | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 1; \
		) \
		echo "--- Testing FIXED stream of 99% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.99| ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 0.99; \
		) \
		echo "--- Testing FIXED stream of 90% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.9 | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 0.9; \
		) \
		echo "--- Testing FIXED stream of 75% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.75 | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 0.75; \
		) \
		echo "--- Testing FIXED stream of 50% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.5 | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 0.5; \
		) \
		echo "--- Testing FIXED stream of 25% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.25 | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 0.25; \
		) \
		echo "--- Testing FIXED stream of 10% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.1 | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 0.1; \
		) \
		echo "--- Testing FIXED stream of 1% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.01| ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 0.01; \
		) \
		echo "--- Testing FIXED stream of 0% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0 | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fixed v 1234 0; \
		) \
		echo "--- Testing SINE stream with 10 periods ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./sine g 1234 10 | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./sine v 1234 10; \
		) \
		echo "--- Testing BIT_PLANE lena_cdf53.pgm stream ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./bit_plane g lena_cdf53.pgm | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./bit_plane v lena_cdf53.pgm; \
		) \
		echo "--- Testing FAX_MACHINE lena_fax.pbm stream ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fax_machine g lena_fax.pbm | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fax_machine v lena_fax.pbm; \
		) \
		echo "--- Testing FAX_MACHINE paper.pbm stream ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fax_machine g paper.pbm | ./$(transform) f | ./$(coder) e | ./$(coder) d | ./$(transform) b | ./fax_machine v paper.pbm; \
		) \
	)

info: $(STREAMS) $(TRANSFORMS) $(CODERS)
	@$(foreach transform,$(TRANSFORMS),\
		echo "==================== Transform: $(transform) ===================="; \
		echo "--- Testing FIXED stream of 100% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 1    2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FIXED stream of 99% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.99 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FIXED stream of 90% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.9 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FIXED stream of 75% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.75 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FIXED stream of 50% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.5 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FIXED stream of 25% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.25 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FIXED stream of 10% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.1 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FIXED stream of 1% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0.01 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FIXED stream of 0% ones ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fixed g 1234 0 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing SINE stream with 10 periods ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./sine g 1234 10 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing BIT_PLANE lena_cdf53.pgm stream ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./bit_plane g lena_cdf53.pgm 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FAX_MACHINE lena_fax.pbm stream ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fax_machine g lena_fax.pbm 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
		echo "--- Testing FAX_MACHINE paper.pbm stream ---"; \
		$(foreach coder,$(CODERS),\
			echo "-------- Coder: $(coder) --------"; \
			./fax_machine g paper.pbm 2> /dev/null | ./$(transform) f 2> /dev/null | ./$(coder) e > /dev/null; \
		) \
	)

clean:
	rm -f sma $(STREAMS) $(TRANSFORMS) $(CODERS)

