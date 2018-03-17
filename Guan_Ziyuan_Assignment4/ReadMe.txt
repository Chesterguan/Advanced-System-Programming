============================
=   Name: Ziyuan Guan      =
=   UFID: 94722121         =
============================
=   Assignment <4>         =
============================
Files Contained:
----------------------------
1.combiner.c
2.makeFile
3.input.txt
4.mapper_output.txt(the sample,when execute,please delete firstly)
5.reducer_output.txt(the sample,when execute, please delete firstly)
6.ReadMe.txt
============================
How To Run The Program:
----------------------------
Step1: Open the terminal by shortcut[ctrl+T]
Step2: Enter the destinated directory by command[cd]
Step3: Type in the command[make]
Step4: Type in the execution command[./combiner 10 7 <input.txt] (the input file will be stored in stdin)  
Step5: Check the files:
           1. Output Txt File "mapper_output.txt"
           2. Output Txt File "reducer_output.txt"
           3. Object Files
           4. Execution Files
=============================
Results:
-----------------------------
1.Fully Satisfy The Requests. The input file contains 50 lines and after combining, the result should contain 42 lines.
2.Solve The Bugs
=============================
Analysis: Mostly as the same as the Assignment2&3
----------------------------
[Steps]:
           1. The difference between this assignment with the previous is that we need to use multiple processes and shared memory.
	   2. First we need to set up the global variables(the variables needed to be shared)such as buffer,buffer_num, mutex(semaphore lock), full and empty.
           3. We need to use "mmap" function to set up the sharing. We need to differ the variables with separated size. The "fd" should be set as"-1", since we need to use MAP_ANONYMOUS.
	   4, We should use fork to build the relationship of parents and sons. In the assignment, we have one producer and several consumers. We can make them all as sons and when (producer==0), it enters the producer procedure, and else, we can fork to consumers.
	   5. The "producer" and "consumer" steps are the same with the previous problems.
	   6. Each time, we need to sync the statement of the mmap since the value of variables are changed.
           7. We need to unmmap in the final. The main process will wait for all the sons.
============================
Tips in MakeFile:
----------------------------
           1. Before we use the standard C99 to make sure the "For" loop will not make mistakes, in this assignment,since we use the MAP_ANONYMOUS as the first time , we need to use the standard as "gnu99"  -std=gnu99
           2. We need to add "-plthread" after the object since the object will use the pthread library.
	   3. The location of the locks are also important to avoid the ddeadlock. Just like the assignment3.
============================
During this assignment, I have the assitance from FangZhu and he helped me solved some problems.


