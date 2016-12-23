long random_range (long min, long max) {
    unsigned long num_bins = (unsigned long) max + 1;
    unsigned long num_rand = (unsigned long) RAND_MAX + 1;
    unsigned long bin_size = num_rand / num_bins;
    unsigned long defect   = num_rand % num_bins;
    long x = 0;
    int32_t iters = 0;

    do {
        x = random();
        ++iters;
    } while (num_rand - defect <= (unsigned long)x && x > min && iters < 100);
    return x/bin_size;
    // go over this
}
