1. Pick 3 of the 10 categories. For each, pick a language that gives it to you for free and say what that language pays for it. "Python has dictionaries" isn't an answer. What does Python's dictionary cost in memory or in speed compared to what you built, and where would you notice?

**dt_array**

*Python* - Python’s array, otherwise known as lists, accessing is safer compared to C since most of the access instructions are handled by the Python interpreter. Additionally, Python’s interpretation of lists as an object means each declaration of a list has methods attached to it that the programmer can use. For example, Python lists can easily expand further due to the .append method and are not restricted to its size upon declaration. The main downside to this is that there is a lot of overhead and thus consumes more memory compared to arrays in C. This is due to arrays in C having direct access to the memory due to it being mainly a pointer to a memory location. Unlike Python lists, the array that was implemented in the problem set is restricted to a fixed size which is allocated upon declaration with dt_array_new.
 
**dt_record**

*Python* - dt_record is most similar to Python dictionaries in terms of function. One major difference between the two implementations is how they search for a key. In the Problem Set, keys are located through the use of a linear search, where each key is compared to the set of keys in the record and only returns an error after going through the entire key set. On the other hand, Python dictionaries implement a hash map to search for keys. This implementation is very efficient in terms of time complexity, only taking O(1) versus linear search’s O(n). However, the downside is the difference in consumed memory between the two lookup methods. Using a hash map is more expensive space-wise due to needing to store the hashes. On the other hand, linear search only requires the variable that iterates through the set of keys.  

**dt_ref**

*Rust* - Memory management through the concept of borrowing and ownership. Ensures memory safety without garbage collection. Rust has strict rules on pointers in its borrowing and ownership concept applications. Multiple immutable references may be made, but only one mutable reference can be created and both types cannot exist at the same time to maintain concurrency safety.  The implementation in the problem set does not make this distinction since it only checks if a certain memory allocation has been released. The problems that the Rust ownership and borrowing system seeks to avoid can be encountered in the problem set’s implementation. However, since there are additional checks that Rust does to enforce the concept, there is additional overhead in terms of memory usage compared to the implementation in the Problem Set.


2. You wrote the tag check in dt_value_as_int by hand. Some languages don't let you. They make the tagged union a language construct, so the compiler writes the check for you, refuses to compile a read that skips it, and refuses to compile a set of cases that misses one. Rust's enum and match work this way, and so do ML's datatypes and Swift's enumerations with associated values. What does the C version let you do that a compiler enforcing the check wouldn't, and is any of it worth wanting?

In our C implementation, dt_value uses a tag to identify what kind of value is stored. In dt_value_as_int, we have to check the tag ourselves before reading the integer value. One advantage of doing this in C is that we have more control over how the data is represented and how the checks are handled. We can decide how the errors are reported and when the checks should happen.

The downside is that C does not force the programmer to perform the tag check. If we forget to check the tag or try to read the value as the wrong type, the compiler will not automatically stop us. For example, in Rust, enum and match allow the compiler to enforce the possible cases and catch certain mistakes before the program runs.

We think that the extra control provided by C can be useful when we need direct control over the data representation and memory. However, for this type of data structure, we would prefer an approach where the possible cases are checked by the compiler because it provides more safety and reduces the chance of mistakes. The C version gives us more control but at the same time, it also puts more responsibility on the programmer.

3. Your dt_map keeps insertion order separately from the hash buckets, which is memory spent on something no lookup uses. Argue the other side: describe a design that drops it, say what breaks, and say whether you'd ship it.

Our dt_map currently uses hash buckets for lookup and a separate structure to keep track of insertion order. An alternative design would remove this separate insertion-order structure and store only the hash buckets and their entries. When a new key is added, we would only place its entry into the appropriate hash bucket. When looking up or removing a key, we would also only work with the hash buckets. This would reduce the memory used by the map and remove the extra bookkeeping needed to maintain the insertion order.

However, the problem is that removing the insertion-order structure would affect dt_map_key_at. Without that structure, we could no longer guarantee that keys are returned in the order they were inserted. Instead, the order would depend on how the entries are arranged in the hash buckets which is based on their hashes rather than their insertion order. Any code that expects dt_map_key_at to follow insertion order would therefore behave differently.

For this problem set, we would keep the insertion-order structure. Although removing it would save some memory and simplify the implementation, it would change the behavior provided by dt_map_key_at. We think the extra memory is reasonable because it allows the map to support insertion-order access as part of its current interface. We would only remove the structure if insertion order was no longer required by the interface.

4. Compare access after release with an allocation that remains unreleased at the driver's final check. What damage can each cause in a long-running server? How does that answer change for a command-line tool that exits in a second?

Accessing a section of memory after it has been freed would confuse the program. It expects to see an address that points to a specific section of memory yet only sees NULL. Whatever instruction that the program has planned on executing with the accessed memory cannot proceed due the allocation not being available. 

On the other hand, not releasing memory on a long running application, like in a server, would cause it to suffer a memory leak. A memory leak is when an application does not free up their allocated memory that they no longer need causing the existing memory pool to continuously shrink. Eventually, when another application or process requests a memory allocation, it is unable to fulfill this requirement due to the lack of available memory to allocate causing errors in the program.

On the other hand, with small programs that only exist for a short period of time memory allocation is not much of a problem. After a program terminates, most modern operating systems automatically free up the allocated memory to be used by other programs. Although freeing up memory before termination is not necessary in this case, it is still a good practice to free up any allocated memory before the program terminates. 
