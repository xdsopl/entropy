# Entropy coding compression challenge of various Bernoulli distributions

## Conclusion
`rle_switch` is a robust choice when the distribution of zeros and ones can change between extremes but is beaten by `rle_zeros` the moment we have more zeros than ones. `rle_leb128` is a solid choice if we need a simple byte based encoding and mostly deal with either lots of zeros or ones.

## Testing a fixed probability of 99% ones (1% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_leb128    | -78.07%   |
| rle_zeros     | +0.97%    |
| rle_switch    | -90.61%   |
| freq_varint   | -82.61%   |

## Testing a fixed probability of 90% ones (10% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_leb128	| +7.77%	|
| rle_zeros 	| +8.25%	|
| rle_switch	| -41.34%	|
| freq_varint	| -38.92%	|

## Testing a fixed probability of 75% ones (25% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_leb128	| +6.96%	|
| rle_zeros 	| +15.53%	|
| rle_switch	| -2.27%	|
| freq_varint	| +24.03%	|

## Testing a fixed probability of 50% ones (50% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_leb128	| +0.65%	|
| rle_zeros 	| +15.45%	|
| rle_switch	| +15.45%	|
| freq_varint	| +82.69%	|

Simply copying the data, also known as `do nothing` works best if we are dealing with a uniform distribution where all symbols have the same probability of occuring. `rle_leb128` also shows the property of resisting the urge to inflate the encoding.

## Testing a fixed probability of 25% ones (75% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_leb128	| +7.12%	|
| rle_zeros 	| -9.30%	|
| rle_switch	| -2.02%	|
| freq_varint	| +24.68%	|

## Testing a fixed probability of 10% ones (90% zeros):
![./fixed g 1234 0.1 | ./sma 10](fixed.png)
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_leb128	| +10.36%	|
| rle_zeros 	| -45.47%	|
| rle_switch	| -39.72%	|
| freq_varint	| -37.06%	|

## Testing a fixed probability of 1% ones (99% zeros):
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_leb128	| -74.43%	|
| rle_zeros 	| -90.21%	|
| rle_switch	| -89.08%	|
| freq_varint	| -82.04%	|

## Testing a sinusoidal probability with 10 periods:
![./sine g 1234 10 | ./sma 10](sine.png)
| Coder         | Change    |
| ------------- | --------- |
| copy          | 0%        |
| rle_leb128	| -15.78%	|
| rle_zeros 	| -13.27%	|
| rle_switch	| -33.41%	|
| freq_varint	| +7.93%	|
