# mkeeper
Memory Keeper(MK) is a project for memory protection and runtime memory debugging.

## Memory issues

### Simple Double Free
If the application frees same memory twice and the memory has not been re-allocated between two free operations, we call it Simple Double Free (SDF).
SDF won't cause issue in application, but it may cause issue in some memory allocator implement. The memory allocator may put the same memory into Free Pool so it breaks the Free Pool. Next time, when applicaton allocates the memory from Free Pool, memory allocator crashes. Memory Keeper checks double free and gives warning to application.

### Simple Use After Free
If the application still read/write the data after the memory freed, and the memory was not re-allocated after freed, we call it Simple Use After Free (SUAF).
SUAF won't cause issue in application and memory allocator. It's very hard to detect. 

### Simple Memory Corruption
If the application writes more data than allocated, we call it Simple Memory Corruption (SMC)
SMC causes issues in application and memory allocator, but it's not hard to detect by adding magic pad data. Memory Keeper can add the pad data if memory allocator doesn't support it.

### Complicate Double Free
If the application frees same memory twice and the memory has been re-allocated after first free, we call it Complicate Double Free (CDF).
CDF causes Memory Corruption, and it's very hard to debug without tool such as memory keeper. Memory Keeper requires different alloc-free interface in application for the protection. But it still supports debugging without application modification.

### Complicate Use After Free
If the application uses same memory after freed and it has been re-allocated, we call it Complicate Use After Free (CUAF).
CUAF causes Memory Corruption heavily. It's hard to protect but it's easy to debug in Memory Keeper.

### Complicate Memory Corruption
Most of time, CDF and CUAF are the reasons of Complicate Memory Corruption. A vary case, someone's code writes the data in some addresses randomly. It's a real story that one guy merged his test code. 

### Memory Leak
Memory leak is very common in large software project. Even the software is not written by C/C++, it still can leak resources such as File Descriptor. Memory Keeper provides the method to safely cleanup the memory, and again, it supports debugging without application modification. For File Descriptor leaking, Memory Keeper provides APIs can be very easily integreated. 

### Simple Segment Fault
We also can call it Null Pointer Segment Fault. It happens if developer forget put sanity check before use.

## backtrace

