# Entropy coding compression challenge of various Bernoulli distributions
## About
The coders here are meant to be useful with binary streams, like those coming from [fax machines](https://en.wikipedia.org/wiki/Fax) or [bit planes](https://en.wikipedia.org/wiki/Bit_plane).

## Conclusion
`rle_switch` is a robust choice when the distribution of zeros and ones can change between extremes but is beaten by `rle_zeros` the moment we have more zeros than ones. `rle_byte` is a solid choice if we need a simple byte based encoding and mostly deal with either lots of zeros or ones.

## Testing bit planes from a CDF53 transformed lena image:
The range of values after the transformation was -181 to 213. Inverting the negative values made it possible to store them as a one byte per pixel grayscale [PGM](https://en.wikipedia.org/wiki/Netpbm) image. The signs have been appended to the end of the [lena_cdf53.pgm](lena_cdf53.pgm) file, so the original transformation can be losslessly reconstructed.

![CDF53 transformed lena](lena_cdf53.jpg)

As we go from the most significant to the least significant bit plane, our chances of getting long runs diminishes:

![./bit_plane g lena_cdf53.pgm | ./sma 100](lena_cdf53_sma.png)

The last 1/8 of the plot shows the appended sign bits.

| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | -48.36%   |
| rle_zeros     | -55.83%   |
| rle_switch    | -54.14%   |
| freq_varint   | -37.92%   |

## Testing a fixed probability of 0% ones (100% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | -99.03%   |
| rle_zeros     | +0.97%    |
| rle_switch    | -99.51%   |
| freq_varint   | -87.14%   |

## Testing a fixed probability of 99% ones (1% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | -78.07%   |
| rle_zeros     | +0.97%    |
| rle_switch    | -90.61%   |
| freq_varint   | -82.61%   |

## Testing a fixed probability of 90% ones (10% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | +7.77%    |
| rle_zeros     | +8.25%    |
| rle_switch    | -41.34%   |
| freq_varint   | -38.92%   |

## Testing a fixed probability of 75% ones (25% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | +6.96%    |
| rle_zeros     | +15.53%   |
| rle_switch    | -2.27%    |
| freq_varint   | +24.03%   |

## Testing a fixed probability of 50% ones (50% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | +0.65%    |
| rle_zeros     | +15.45%   |
| rle_switch    | +15.45%   |
| freq_varint   | +82.69%   |

Simply copying the data, also known as `do nothing` works best if we are dealing with a uniform distribution where all symbols have the same probability of occuring. `rle_byte` also shows the property of resisting the urge to inflate the encoding.

## Testing a fixed probability of 25% ones (75% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | +7.12%    |
| rle_zeros     | -9.30%    |
| rle_switch    | -2.02%    |
| freq_varint   | +24.68%   |

## Testing a fixed probability of 10% ones (90% zeros):
![./fixed g 1234 0.1 | ./sma 10](fixed.png)
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | +10.36%   |
| rle_zeros     | -45.47%   |
| rle_switch    | -39.72%   |
| freq_varint   | -37.06%   |

## Testing a fixed probability of 1% ones (99% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | -74.43%   |
| rle_zeros     | -90.21%   |
| rle_switch    | -89.08%   |
| freq_varint   | -82.04%   |

## Testing a fixed probability of 0% ones (100% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | -99.03%   |
| rle_zeros     | -99.51%   |
| rle_switch    | -99.51%   |
| freq_varint   | -87.14%   |

## Testing a sinusoidal probability with 10 periods:
![./sine g 1234 10 | ./sma 10](sine.png)
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_byte      | -15.78%   |
| rle_zeros     | -13.27%   |
| rle_switch    | -33.41%   |
| freq_varint   | +7.93%    |

The bitstream coming from a [bit plane](https://en.wikipedia.org/wiki/Bit_plane) looks a bit more interesting than this sinusoidal probability but it is good enough to show the strength of `rle_switch` here: We alternate back and forth [run length encoding](https://en.wikipedia.org/wiki/Run-length_encoding) sequences of zeros and ones. This way we only need to encode the run length. We use adaptive [Rice coding](https://en.wikipedia.org/wiki/Golomb_coding) for the lengths while alternating between two contexts.

