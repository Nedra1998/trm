# Features

[TOC]

## OpenMP Parallelization

The first feature to improve rendering performace is to implement
parallelization. One of the simpler methods for implementing parallelization is
to use [OpenMP](https://www.openmp.org/) OpenMP makes the addition of
parallelization of a for loop very simple. Just annotate the loop with one of
their `#pragma` macros, for example.

```cpp
#pragma omp parallel for schedule(dynamic, 1024)
for (std::size_t i = 0; i < img.width * img.height; ++i) {
  // Body of loop
}
```

OpenMP is able to split the for loop into configured chunk sizes, and executing
each chunk in parallel. So there is a balence between the number of chunks and
the size of the chunks. If the chunks are too small, the overhead of creating
new threads and managing them will be too high. If the chunks are too large,
the work will not be distributed evenly across the threads.

### Benchmarks

To evaluate a good setting for the chunk size, I rendered the same scene (the
[basic
example](http://vega.lyra:9070/Arden/trm/src/commit/cdbfa4c20c926084c4a1c344a668089696cad68a/examples/basic.cpp))
and varied the chunk size and the number of samples between snapshots during
rendering. Then examining the time per sample from each of the configurations.
I've included the data below to show the results.

| Chunk size | Samples  | Time per sample (ms) |
| ---------- | -------- | -------------------- |
| 256        | 8        | 759.25               |
| 512        | 8        | 744.70               |
| 1024       | 8        | 727.13               |
| 256        | 16       | 741.38               |
| 512        | 16       | 717.49               |
| 1024       | 16       | 716.54               |
| 256        | 32       | 744.72               |
| 512        | 32       | 748.91               |
| 1024       | 32       | 752.30               |

![omp](omp.png)

> [!NOTE]
> Although a chunk size of 1024 with 16 samples per snapshot showed the best
> performance. I will be using a chunk size of 1024 with 8 samples per snapshot
> in order to collect more data on the performance of the renderer at higher
> sample rates.

As the data shows with 8 samples per snapshot, a chunk size of 1024 is the best
performing configuration. As it does enough work to keep the threads busy but
there are enough chunks that no thread is left idle while the others finish.
However, with 32 samples per snapshot the chunk size of 256 becomes the best
performing configuration. This is likely due to the additional samples being
taken, means that a smaller chunk size does enough work to make the overhead of
starting a new thread worth it and the extra chunks allow for better ballencing
of computationally intensive chunks with the simpler chunks.
