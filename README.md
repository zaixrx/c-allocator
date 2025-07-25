# Goals
- learn unix memory scheme(.text, .data, .bss, .heap, .stack)
  and how to interact with it(brk, mmap).
- learn how modern heap alloctors are implemented
    - how to demand memory from the kernel.
    - how to manage the demanded memory in your side
      respecting how cpus interact with memory.
    - how to make reusable(free, realloc)

# Resources
- K&R The C Programming Language (Book)
- IBM OpenJ9 Java VM: https://www.ibm.com/docs/en/sdk-java-technology/8?topic=management-heap-allocation
- IBM Data alignment, Straighten up and fly right: https://developer.ibm.com/articles/pa-dalign/
- Writing an OS in Rust :: Allcator Designs: https://os.phil-opp.com/allocator-designs/
