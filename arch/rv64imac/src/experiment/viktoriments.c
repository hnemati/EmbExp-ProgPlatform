// TODO
// ------------------------------------------------------------------------
// todo: Move non-experiment code to another file, like test cases. Include this file should be enough?
// Do I want the results in appendix? The code can be linked from thesis to github. Maybe only use prime and probe code in report.
// The results of the test cases ( cache_exp_all() ), can be found in the appendix of the thesis.


// experiment definitions
// ------------------------------------------------------------------------
/*

#define __UNUSED __attribute__((unused))
#define __ALIGN(x) __attribute__ ((aligned (x)))
#define CACHEABLE(x) ((void *)(((uint64_t)(&x)) + 0x18000000))

// reserved memory used for basic experiments
uint64_t memory[CACHE_SIZE * 8 / 8] __ALIGN(CACHE_SIZE);


// experiments and test cases START
// ------------------------------------------------------------------------

void cache_exp_primeandprobe_two_executions(){
  printf("experiment: cache_exp_primeandprobe_two_executions\n");
  cache_state cache_state0;
  cache_state cache_state1;

  flush_cache(); // remove flush if not testing.
  cache_func_prime();
  // Arbitary Access
  volatile uint64_t xNew = 0;
  xNew = 0x1337;
  // access a cacheable value
  volatile uint64_t * xPNew = (uint64_t * )CACHEABLE(xNew);
  __UNUSED uint8_t tmp = check_address_is_in_cache((uint64_t)(xPNew));
  cache_func_probe_save(&cache_state0);

  printf("saved cache, flushing and executing again...\n");
  flush_cache(); // remove flush if not testing.
  cache_func_prime();
  // Arbitary Access
  volatile uint64_t yNew = 0;
  yNew = 0x1337;
  // access a cacheable value
  volatile uint64_t * yPNew = (uint64_t * )CACHEABLE(yNew);
  tmp = check_address_is_in_cache((uint64_t)(yPNew));
  cache_func_probe_save(&cache_state1);

  printf("saved cache, comparing cache...\n");
  uint8_t equal = compare_cache(&cache_state0, &cache_state1);
  if(equal){
    printf("Equal caches.\n");
  }else{
    printf("Unequal caches.\n");
  }
  // print the sets
  printf("rinting cache...\n");
  printf("cache_state0:\n");
  print_cache_state(&cache_state0);
  printf("cache_state1:\n");
  print_cache_state(&cache_state1);
}

void cache_exp_primeandprobe(){
  printf("experiment: cache_exp_primeandprobe\n");
  flush_cache(); // remove flush if not testing.

  cache_state cache_state;

  cache_func_prime();

  // Arbitary Access
  volatile uint64_t xNew = 0;
  xNew = 0x1337;
  // access a cacheable value
  volatile uint64_t * xPNew = (uint64_t * )CACHEABLE(xNew);
  __UNUSED uint8_t tmp = check_address_is_in_cache((uint64_t)(xPNew));

  // This is probe
  cache_func_probe_save(&cache_state);
  print_cache_state(&cache_state);
}

void cache_exp_cachesets_fill_with_access_inbetween(){
  printf("experiment: cache_exp_cachesets_fill_with_access_inbetween\n");
  flush_cache(); // remove flush if not testing.

  // This is prime
  for(int i = 0; i < SETS; i++){
    fill_set_0(i); // This fills the cache.
  }

  // Arbitary Access
  volatile uint64_t xNew = 0;
  xNew = 0x1337;
  // access a cacheable value
  volatile uint64_t * xPNew = (uint64_t * )CACHEABLE(xNew);

  if(check_address_is_in_cache((uint64_t)(xPNew))){
    printf(" x is in the cache \n");
  }else{
    printf(" x is NOT in the cache \n");
  }
  if(check_address_is_in_cache((uint64_t)(xPNew))){
    printf(" x is in the cache \n");
  }else{
    printf(" x is NOT in the cache \n");
  }

  // This is probe
  for(int i = 0; i < SETS; i++){
    uint8_t miss = fill_set_0_miss(i);
    if(miss){
      printf("There was a miss at index %d.\n", i);
    }
  }
  printf("There was NO miss.\n");
}

void cache_exp_cachesets_fill(){
  printf("experiment: cache_exp_cachesets_fill\n");
  flush_cache(); // remove flush if not testing.

  for(int i = 0; i < SETS; i++){
    fill_set_0(i); // This fills the cache.
  }

  for(int i = 0; i < SETS; i++){
    uint8_t miss = fill_set_0_miss(i);
    if(miss){
      printf("There was a miss.\n");
      return;
    }
  }
  printf("There was NO miss.\n");
}

void cache_exp_cachesets(){
  printf("experiment: cache_exp_cachesets\n");
  flush_cache(); // remove flush if not testing.

  for(int i = 0; i < SETS; i++){
    access_set_0(i);
  }

  for(int i = 0; i < SETS; i++){
    uint8_t miss = access_set_miss(i);
    if(miss){
      printf("There was a miss.\n");
      return;
    }
  }
  printf("There was NO miss.\n");
}

void cache_exp_miss_and_hit_from_base(){
  printf("experiment: cache_exp_miss_and_hit_from_base\n");
  // cache_exp_miss_and_hit_from_base(); // This test shows miss and miss, when starting at 8000 0000, in uncacheable area
  uint64_t dcache_misses0;
  uint64_t dcache_misses1;
  uint64_t cycles0;
  uint64_t cycles1;

  asm volatile(
     ".word 0x1111100b;\n" //fence.t
     "fence iorw, iorw;\n"
     "csrr t1, 0xb04;\n"
     "csrr t2, 0xb00;\n"
     "lw t0, 256(sp);\n"
     "csrr t3, 0xb00;\n"
     "csrr t4, 0xb04;\n"
     "sub %0, t4, t1;\n"
     "sub %1, t3, t2;\n"
     "fence iorw, iorw;\n"
     "csrr t1, 0xb04;\n"
     "csrr t2, 0xb00;\n"
     "lw t0, 256(sp);\n"
     "csrr t3, 0xb00;\n"
     "csrr t4, 0xb04;\n"
     "sub %2, t4, t1;\n"
     "sub %3, t3, t2;\n"
     : "=r"(dcache_misses0), "=r"(cycles0), "=r"(dcache_misses1), "=r"(cycles1)
     :
     :
   );

  printf("[First load: l1dc miss: %d, cycles: %d.] [Second load: l1dc miss: %d, cycles: %d.] \n", dcache_misses0, cycles0, dcache_misses1, cycles1);
}

void cache_exp_flushinbetween(){
  printf("experiment: cache_exp_flushinbetween\n");
  uint64_t dcache_misses0;
  uint64_t dcache_misses1;
  uint64_t cycles0;
  uint64_t cycles1;

  asm volatile(
     ".word 0x1111100b;\n" //fence.t
     "fence iorw, iorw;\n"
     "csrr t1, 0xb04;\n"
     "csrr t2, 0xb00;\n"
     "lw t0, 256(sp);\n"
     "csrr t3, 0xb00;\n"
     "csrr t4, 0xb04;\n"
     "sub %0, t4, t1;\n"
     "sub %1, t3, t2;\n"
     ".word 0x1111100b;\n" //fence.t
     "fence iorw, iorw;\n"
     "csrr t1, 0xb04;\n"
     "csrr t2, 0xb00;\n"
     "lw t0, 256(sp);\n"
     "csrr t3, 0xb00;\n"
     "csrr t4, 0xb04;\n"
     "sub %2, t4, t1;\n"
     "sub %3, t3, t2;\n"
     : "=r"(dcache_misses0), "=r"(cycles0), "=r"(dcache_misses1), "=r"(cycles1)
     :
     :
   );

  printf("[First load: l1dc miss: %d, cycles: %d.] [Second load: l1dc miss: %d, cycles: %d.] \n", dcache_misses0, cycles0, dcache_misses1, cycles1);
}

void cache_exp_branch_specload_part2(uint64_t x){
  //branch on a condition, try to see if a load is made while speculative,
  // maybe better to have function which branches on a argument.
  asm volatile(
     "addi t0, x0, 256;\n"
     "add t1, %0, sp;\n"
     "bge %0, t0, end;\n"//jump
     "lw t0, 0(t1);\n"//spec load
     "end:\n"           //jump dest.
     :
     :"r"(x)
     :
   );
}

void cache_exp_branch_specload_part1(){
  //branch on a condition, try to see if a load is made while speculative,
  // maybe better to have function which branches on a argument.
  printf("experiment: cache_exp_branch_specload\n");

  flush_cache();

  volatile uint64_t x = 0;
  x = 100;
  volatile uint64_t * xP = (uint64_t * )CACHEABLE(x);
  volatile uint64_t y = 0;
  x = 0x52;
  volatile uint64_t * yP = (uint64_t * )CACHEABLE(y);

  uint64_t dcache_misses0;
  uint64_t cycles0;
  uint64_t mispredicts;
  uint64_t dcache_misses1;
  uint64_t cycles1;
  uint64_t mispredict1;

  //train branch
  cache_exp_branch_specload_part2(*xP);

  asm volatile(
     "csrr t1, 0xb0e;\n"
     "csrr t2, 0xb04;\n"
     "csrr t3, 0xb00;\n"
     : "=r"(dcache_misses0), "=r"(cycles0), "=r"(mispredicts)
     :
     :
   );

  //bad x
  cache_exp_branch_specload_part2(*yP);

  asm volatile(
     "csrr t1, 0xb0e;\n"
     "csrr t2, 0xb04;\n"
     "csrr t3, 0xb00;\n"
     : "=r"(dcache_misses1), "=r"(cycles1), "=r"(mispredict1)
     :
     :
   );

   printf("[Exp time: l1dc miss: %d, cycles: %d, mispredicts: %d.] \n", dcache_misses1 - dcache_misses0, cycles1 - cycles0, mispredict1 - mispredicts);
}

void cache_exp_branch_specload(){
  //branch on a condition, try to see if a load is made while speculative,
  // maybe better to have function which branches on a argument.
  printf("experiment: cache_exp_branch_specload\n");

  uint64_t dcache_misses0;
  uint64_t cycles0;
  uint64_t mispredicts;

  asm volatile(
     ".word 0x1111100b;\n" //fence.t
     "add t0, x0, 5;\n"
     "add a1, x0, 5;\n"
     "sw t0, 0(sp);\n"
     "fence iorw, iorw;\n"
     "csrr t5, 0xb0e;\n"
     "csrr t1, 0xb04;\n"
     "csrr t2, 0xb00;\n"
     "lw a0, 0(sp);\n"//spec load
     "beq a1, a0, stop;\n"//jump
     "lw t0, 9(sp);\n"//spec load
     "stop:\n"           //jump dest.
     "csrr t3, 0xb00;\n"
     "csrr t4, 0xb04;\n"
     "csrr a3, 0xb0e;\n"
     "sub %0, t4, t1;\n"
     "sub %1, t3, t2;\n"
     "sub %2, a3, t5;\n"
     : "=r"(dcache_misses0), "=r"(cycles0), "=r"(mispredicts)
     :
     :
   );

   printf("[Exp time: l1dc miss: %d, cycles: %d, mispredicts: %d.] \n", dcache_misses0, cycles0, mispredicts);
}

void cache_exp_straight_spec(){
  //jmp to a address, based on a value in pipeline.
  //below jmp, have a load, see if load leaks.

  printf("experiment: cache_exp_straight_spec\n");

  uint64_t dcache_misses0;
  uint64_t cycles0;
  uint64_t mispredicts;

  asm volatile(
     ".word 0x1111100b;\n" //fence.t
     "add t0, sp, 512;\n"
     "sw a0, 0(sp);\n"
     "fence iorw, iorw;\n"
     "csrr t5, 0xb0e;\n"
     "csrr t1, 0xb04;\n"
     "csrr t2, 0xb00;\n"
     "lw a0, 0(sp);\n"//spec load
     "jr 0(a0);\n" //jump
     "lw t0, 16(sp);\n"//spec load
     //"end: a0, x0, 3;\n" //arbitary
     "csrr t3, 0xb00;\n"
     "csrr t4, 0xb04;\n"
     "csrr a3, 0xb0e;\n"
     "sub %0, t4, t1;\n"
     "sub %1, t3, t2;\n"
     "sub %2, a3, t5;\n"
     : "=r"(dcache_misses0), "=r"(cycles0), "=r"(mispredicts)
     :
     :
   );

   printf("[Exp time: l1dc miss: %d, cycles: %d, mispredicts: %d.] \n", dcache_misses0, cycles0, mispredicts);
}

void test_value_in_cache() {
  // Basically shows that two accesses to the same memory address within the cacheable area will cause a miss and then a hit.
  printf("experiment: test_value_in_cache\n");
  //Modified from cache_experiment.c
  // check memory alias
  volatile uint64_t x = 0;

  flush_cache();

  x = 0x41;
  // access a cacheable value
  volatile uint64_t * xP = (uint64_t * )CACHEABLE(x);

  if(check_address_is_in_cache((uint64_t)(xP))){
    printf(" x is in the cache \n");
  }else{
    printf(" x is NOT in the cache \n");
  }

  if(check_address_is_in_cache((uint64_t)(xP))){
    printf(" x is in the cache \n");
  }else{
    printf(" x is NOT in the cache \n");
  }

}

void test_two_ways() {
  // Shows that the 2 accesses to the same set are in the cache.
  printf("experiment: test_two_ways\n");
  //Modified from cache_experiment.c
  //Note experiment memory not changed at all, yet.
  flush_cache();
  uint64_t a1 = 0;
  uint64_t a2 = a1 + CACHE_SIZE * 1 / 8;

  memory[a1] = 0x123;
  memory[a2] = 0x456;

  volatile uint64_t * xP = (uint64_t * )CACHEABLE(memory[a1]);
  printf("addresses %x %x %x \n", &(memory[a1]), &(memory[a2]), xP);
  volatile uint64_t x = *(xP);
  volatile uint64_t * yP = (uint64_t * )CACHEABLE(memory[a2]);
  volatile uint64_t y = *(yP);

  printf("values %x %x\n", x, y);

  if(check_address_is_in_cache((uint64_t)(xP))){
    printf(" a1 is in the cache \n");
  }else{
    printf(" a1 is NOT in the cache \n");
  }
  if(check_address_is_in_cache((uint64_t)(yP))){
    printf(" a2 is in the cache \n");
  }else{
    printf(" a2 is NOT in the cache \n");
  }
}

void test_eight_ways() {
  // // Shows that the 8 accesses to the same set are in the cache.
  printf("experiment: test_eigth_ways\n");
  //Modified from cache_experiment.c
  //Note experiment memory not changed at all, yet.
  flush_cache();

  uint64_t aarry[8];

  aarry[0] = 0;
  memory[0] = 0x123;
  for(int i = 1; i < 8; i++){
    aarry[i] = aarry[i-1] + CACHE_SIZE * i / 8;
    memory[aarry[i]] = memory[aarry[i-1]] + 0x123;
  }

  printf("addresses ");
  for(int i = 0; i < 8; i++){
    printf("%x ", &(memory[aarry[i]]));
  }
  printf("\n");

  printf("values ");
  for(int i = 0; i < 8; i++){
    volatile uint64_t * xP = (uint64_t * )CACHEABLE(memory[aarry[i]]);
    volatile uint64_t x = *(xP);
    printf("%x ", x);
  }
  printf("\n");

  for(int i = 0; i < 8; i++){
    volatile uint64_t * xP = (uint64_t * )CACHEABLE(memory[aarry[i]]);
    if(check_address_is_in_cache((uint64_t)(xP))){
      printf(" a%d is in the cache. ", i);
    }else{
      printf(" a%d is NOT in the cache. ", i);
    }
  }
  printf("\n");
}

void test_nine_ways() {
  printf("experiment: test_nine_ways\n");
  // Shows that the 9 accesses to the same set are NOT all in the cache.
  // this case will cause more cache misses than thought due to the checking for cache hits

  //Modified from cache_experiment.c
  //Note experiment memory not changed at all, yet.
  flush_cache();

  uint64_t aarry[9];

  aarry[0] = 0;
  memory[0] = 0x123;
  for(int i = 1; i < 9; i++){
    aarry[i] = aarry[i-1] + CACHE_SIZE * i / 8;
    memory[aarry[i]] = memory[aarry[i-1]] + 0x123;
  }

  printf("addresses ");
  for(int i = 0; i < 9; i++){
    printf("%x ", &(memory[aarry[i]]));
  }
  printf("\n");

  printf("values ");
  for(int i = 0; i < 9; i++){
    volatile uint64_t * xP = (uint64_t * )CACHEABLE(memory[aarry[i]]);
    volatile uint64_t x = *(xP);
    printf("%x ", x);
  }
  printf("\n");

  for(int i = 0; i < 9; i++){
    volatile uint64_t * xP = (uint64_t * )CACHEABLE(memory[aarry[i]]);
    if(check_address_is_in_cache((uint64_t)(xP))){
      printf(" a%d is in the cache. ", i);
    }else{
      printf(" a%d is NOT in the cache. ", i);
    }
  }
  printf("\n");
}

void cache_exp_all(){

  // printf("== First experiment Start == \n");
  // cache_exp_miss_and_hit_from_base(); // This test shows miss and miss, when starting at 8000 0000, in uncacheable area
  // printf("== First experiment Done  == \n");
  //
  // printf("== Second experiment Start == \n");
  // cache_exp_cachesets(); // Fills each set with one access
  // printf("== Second experiment Done  == \n");
  //
  // printf("== Third experiment Start == \n");
  // //cache_exp_branch_specload(); // Not done. Not sure if this work as intended due to pref counter.
  // printf("== Third experiment Done  == \n");
  //
  // printf("== Forth experiment Start == \n");
  // //cache_exp_straight_spec(); // Not done. Not sure how to correctly do it. Will try constant jump.
  // printf("== Forth experiment Done  == \n");
  //
  // printf("== Fifth experiment Start == \n");
  // test_value_in_cache(); // Basically shows that two accesses to the same memory address within the cacheable area will cause a miss and then a hit.
  // printf("== Fifth experiment Done  == \n");
  //
  // printf("== Sixth experiment Start == \n");
  // test_eight_ways(); // Shows that the 8 accesses to the same set are in the cache.
  // printf("== Sixth experiment Done  == \n");
  //
  // printf("== Seventh experiment Start == \n");
  // test_nine_ways(); // Shows that the 9 accesses to the same set are NOT all in the cache.
  // printf("== Seventh experiment Done  == \n");
  //
  // printf("== Eight experiment Start == \n");
  // test_two_ways(); // Shows that the 2 accesses to the same set are in the cache.
  // printf("== Eight experiment Done  == \n");
  //
  // printf("== Ninth experiment Start == \n");
  // cache_exp_cachesets_fill(); // Shows that sets are being filled in all there ways.
  // printf("== Ninth experiment Done == \n");
  //
  // printf("== Tenth experiment Start == \n");
  // cache_exp_cachesets_fill_with_access_inbetween(); // Shows that sets are being filled in all 8 ways.
  // printf("== Tenth experiment Done == \n");
  //
  // printf("== Eleventh experiment Start == \n");
  // cache_exp_primeandprobe(); // prime and probe (with access inbetween)
  // printf("== Eleventh experiment Done == \n");
  //
  printf("== Twelfth experiment Start == \n");
  cache_exp_primeandprobe_two_executions(); // prime and probe 2 times, to compare caches.
  printf("== Twelfth experiment Done == \n");

}

// experiments and test cases END


//Could add normal flush instructions as experiment?
//fence_i_o,          // flush I$ and pipeline
//fence_o,            // flush D$ and pipeline

*/
