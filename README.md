# Entropy coding compression challenge of various Bernoulli distributions

## Coders
The coders here are meant to be useful with binary streams, like those coming from [fax machines](https://en.wikipedia.org/wiki/Fax) or [bit planes](https://en.wikipedia.org/wiki/Bit_plane).
* copy: This is the `do nothing` coder
* rle_byte: Byte values between 1 and 254 are simply copied, while either 0 or 255 values are [run length encoded](https://en.wikipedia.org/wiki/Run-length_encoding) up to a run length of 256 bytes
* rle_zeros: We only encode the run length of bits with value zero using adaptive [Rice coding](https://en.wikipedia.org/wiki/Golomb_coding). This means that every bit with the value one gets encoded by a run length of zero zeros.
* rle_switch: We alternate back and forth [run length encoding](https://en.wikipedia.org/wiki/Run-length_encoding) sequences of zeros and ones. This way we only need to encode the run length. We use adaptive [Rice coding](https://en.wikipedia.org/wiki/Golomb_coding) for the lengths while alternating between two contexts.
* freq_varint: Currently the only non-streaming coder. We create a [histogram](https://en.wikipedia.org/wiki/Histogram) of byte values and use that to create a [permutation](https://en.wikipedia.org/wiki/Permutation) that maps frequent values to smaller values and encode those using a [variable length integer code](https://en.wikipedia.org/wiki/Universal_code_(data_compression)).
* arithmetic: We encode the current bit according to the past probability of some bits via [Arithmetic coding](https://en.wikipedia.org/wiki/Arithmetic_coding)

## Transforms
The transforms here help the coders to perform better by [preconditioning](https://en.wikipedia.org/wiki/Preconditioner) the input.
* copy: This is the `do nothing` transformer
* xor: Xor with the previous bit. This helps breaking up long sequences of ones and turn them into zeros, but creates long sequences of ones when bits alternate, therefore doing the opposite.
* bwt: The [Burrows–Wheeler transform](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform) rearranges the input in a way that creates longer runs. We use a block size of 1024 bits here.

## Conclusion
`rle_switch` is a robust choice when the distribution of zeros and ones can change between extremes but is beaten by `rle_zeros` the moment we have more zeros than ones.
`rle_byte` is a solid choice if we need a simple byte based encoding and mostly deal with either lots of zeros or ones.
Last but not least, arithmetic coding excels at tackling hard cases and pairs well with the BWT transform when ample CPU time is available.

## Testing binary image of dithered lena image:
The binary image was created by applying [Floyd–Steinberg dithering](https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering) to the grayscale image:

![Dithered lena](lena_fax.png)

Dithering makes it possible to bring grayscale pictures to paper using various printing techniques. Unfortunately it also makes it hard to compress, as noise is intentionally introduced:

![./fax_machine g lena_fax.pbm | ./sma 100](lena_fax_sma.png)

| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | +0.06%    | +2.63%    | -7.61%    |
| rle_zeros     | +23.94%   | +1.17%    | +4.99%    |
| rle_switch    | -4.90%    | -12.34%   | -22.97%   |
| freq_varint   | -0.20%    | +2.65%    | +4.17%    |
| arithmetic    | -13.32%   | -8.30%    | -27.54%   |

This is a perfect example to show the boosting effect of `bwt`.
Nice to see that `arithmetic` here is able to achieve more with this tough data.

## Testing binary image of text:
![one page about quadrature decoders](paper.jpg)

Binary images of text on the other hand are easier to compress:

![./fax_machine g paper.pbm | ./sma 100](paper_sma.png)

| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | -87.30%   | -87.28%   | -87.36%   |
| rle_zeros     | -89.69%   | -91.39%   | -91.16%   |
| rle_switch    | -92.20%   | -90.51%   | -91.75%   |
| freq_varint   | -81.23%   | -80.93%   | -80.42%   |
| arithmetic    | -86.53%   | -88.63%   | -88.18%   |

## Testing bit planes from a CDF53 transformed lena image:
The range of values after the transformation was -181 to 213. Inverting the negative values made it possible to store them as a one byte per pixel grayscale [PGM](https://en.wikipedia.org/wiki/Netpbm) image. The signs have been appended to the end of the [lena_cdf53.pgm](lena_cdf53.pgm) file, so the original transformation can be losslessly reconstructed.

![CDF53 transformed lena](lena_cdf53.jpg)

As we go from the most significant to the least significant bit plane, our chances of getting long runs diminishes:

![./bit_plane g lena_cdf53.pgm | ./sma 100](lena_cdf53_sma.png)

The last 1/8 of the plot shows the appended sign bits.

| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | -48.36%   | -48.41%   | -45.95%   |
| rle_zeros     | -55.83%   | -50.54%   | -55.04%   |
| rle_switch    | -54.14%   | -51.01%   | -52.71%   |
| freq_varint   | -37.92%   | -32.35%   | -36.29%   |
| arithmetic    | -57.03%   | -51.96%   | -56.72%   |

For bit planes, none of the transformations `copy`, `xor`, or `bwt` improve performance; all incur significant penalties.

## Testing a fixed probability of 100% ones (0% zeros):
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | -99.03%   | -98.95%   | -95.93%   |
| rle_zeros     | +0.97%    | -99.51%   | -0.16%    |
| rle_switch    | -99.51%   | -99.51%   | -97.76%   |
| freq_varint   | -87.14%   | -87.06%   | -85.78%   |
| arithmetic    | -95.06%   | -95.06%   | -90.34    |

This is an example where `xor` helps `rle_zeros` to perform better, while hurting the performance of the other coders.

## Testing a fixed probability of 99% ones (1% zeros):
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | -78.07%   | -77.27%   | -78.27%   |
| rle_zeros     | +0.97%    | -85.36%   | +0.96%    |
| rle_switch    | -90.61%   | -88.67%   | -88.34%   |
| freq_varint   | -82.61%   | -81.72%   | -79.63%   |
| arithmetic    | -90.21%   | -84.79%   | -87.22%   |

Same here, `xor` helps `rle_zeros` but hurts others.

## Testing a fixed probability of 90% ones (10% zeros):
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | +7.77%    | +7.04%    | +6.31%    |
| rle_zeros     | +8.25%    | -26.38%   | +8.15%    |
| rle_switch    | -41.34%   | -29.77%   | -41.29%   |
| freq_varint   | -38.92%   | -28.56%   | -36.58%   |
| arithmetic    | -48.95%   | -27.83%   | -48.24%   |

## Testing a fixed probability of 75% ones (25% zeros):
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | +6.96%    | +5.50%    | +9.03%    |
| rle_zeros     | +15.53%   | +5.42%    | +15.73%   |
| rle_switch    | -2.27%    | +7.28%    | -3.04%    |
| freq_varint   | +24.03%   | +43.53%   | +25.64%   |
| arithmetic    | -16.02%   | -1.86%    | -15.34%   |

## Testing a fixed probability of 50% ones (50% zeros):
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | +0.65%    | +0.49%    | +0.56%    |
| rle_zeros     | +15.45%   | +15.45%   | +15.50%   |
| rle_switch    | +15.45%   | +16.02%   | +15.50%   |
| freq_varint   | +82.69%   | +83.09%   | +81.31%   |
| arithmetic    | +2.59%    | +2.35%    | +2.32%    |

Simply copying the data, also known as `do nothing` works best if we are dealing with a uniform distribution where all symbols have the same probability of occuring. `rle_byte` also shows the property of resisting the urge to inflate the encoding.

## Testing a fixed probability of 25% ones (75% zeros):
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | +7.12%    | +5.66%    | +7.27%    |
| rle_zeros     | -9.30%    | +4.77%    | -9.19%    |
| rle_switch    | -2.02%    | +7.52%    | -3.12%    |
| freq_varint   | +24.68%   | +43.28%   | +25.32%   |
| arithmetic    | -15.78%   | -1.78%    | -15.50%   |

## Testing a fixed probability of 10% ones (90% zeros):
![./fixed g 1234 0.1 | ./sma 10](fixed.png)
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | +10.36%   | +9.39%    | +6.71%    |
| rle_zeros     | -45.47%   | -23.46%   | -45.21%   |
| rle_switch    | -39.72%   | -26.78%   | -39.30%   |
| freq_varint   | -37.06%   | -27.27%   | -35.62%   |
| arithmetic    | -47.73%   | -26.21%   | -47.20%   |

## Testing a fixed probability of 1% ones (99% zeros):
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | -74.43%   | -73.79%   | -73.72%   |
| rle_zeros     | -90.21%   | -82.93%   | -87.70%   |
| rle_switch    | -89.08%   | -86.81%   | -87.14%   |
| freq_varint   | -82.04%   | -81.31%   | -79.95%   |
| arithmetic    | -89.32%   | -82.69%   | -86.82%   |

## Testing a fixed probability of 0% ones (100% zeros):
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | -99.03%   | -99.03%   | -98.96%   |
| rle_zeros     | -99.51%   | -99.51%   | -99.52%   |
| rle_switch    | -99.51%   | -99.51%   | -99.44%   |
| freq_varint   | -87.14%   | -87.14%   | -87.06%   |
| arithmetic    | -95.06%   | -95.06%   | -95.05%   |

## Testing a sinusoidal probability with 10 periods:
![./sine g 1234 10 | ./sma 10](sine.png)
| Coder \ Trans | copy      | xor       | bwt       |
| ------------- | --------- | --------- | --------- |
| copy          | 0%        | 0%        | 0%        |
| rle_byte      | -15.78%   | -16.18%   | -7.59%    |
| rle_zeros     | -13.27%   | -25.24%   | -8.15%    |
| rle_switch    | -33.41%   | -26.05%   | -23.08%   |
| freq_varint   | +7.93%    | +1.78%    | +20.29%   |
| arithmetic    | -41.59%   | -29.53%   | -28.83%   |

The bitstream coming from a [bit plane](https://en.wikipedia.org/wiki/Bit_plane) looks a bit more interesting than this sinusoidal probability but it is good enough to show the strength of `rle_switch` here. `arithmetic` is doing better still.

## References
* Run-length encodings  
Solomon W. Golomb - 1966
* Arithmetic Coding for Data Compression  
Ian H. Witten, Radford M. Neal, and John G. Cleary - 1987
* A Block-sorting Lossless Data Compression Algorithm  
M. Burrows and D.J. Wheeler - 1994

## YouTube
* [Data Compression (Summer 2020) -Lecture 15- Arithmetic Coding II (Infinite Precision in Finite Bits)](https://www.youtube.com/watch?v=WT57qXMpKjQ)
* [COMP526 (Spring 2022) 7-10 §7.10 Inverse BWT](https://www.youtube.com/watch?v=-srE1JQ9uEg)
