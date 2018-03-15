============================
=   Name: Ziyuan Guan      =
=   UFID: 94722121         =
============================
=   Assignment <3>         =
============================
Files Contained:
----------------------------
1.combiner.c
2.makeFile
3.input.txt
4.mapper_output.txt
5.reducer_output.txt
6.ReadMe.txt
============================
How To Run The Program:
----------------------------
Step1: Open the terminal by shortcut[ctrl+T]
Step2: Enter the destinated directory by command[cd]
Step3: Type in the command[make]
Step4: Type in the execution command[./combiner 10 7 ]  (ps. we can also use the "stdin" as the input source)
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
Analysis: Mostly as the same as the Assignment2
----------------------------
[Steps]:
           1. Firstly, we need to create the threads in "main". Create two structs globally. Fist one is the buffer_data, contains the information of "name";"subject";"score";"buffer_number". Second one is the thread_data, contains the information of "thread_id";"thread_name", which are used to be compared with the information of turple.
           2. Create an integer globally array called "buffer_num" which is used to store the num of slots in each buffer. Create an integer globally called "ticket" to control the running order of different "reducer" threads.
           3. Then in each thread, we need to have a outer loop "while(1)" to keep thread running.
	   4. In the problem, we have changed the conditional variables into semaphores. We can use sem_t to define three conditions : full[Num_Threads], empty, mutex
[Mapper]
           5. Enter the mapper, use "sem_wait(&empty) sem_wait(&mutex)" to lock the region.Then I used the "while(fgets())" to control that the file is end or not to break the loop.
           6. Read the turple and store the separated information into different parts. 
	   7. Go through all of the buffer and judge the value of "buffer_num", if equals the "Num_Slots". "mapper"thread waits with the code: "sem_post(&full[thread_id])" to wake up the reducer thread.
           8. If all of the buffer is not full, continue, read and store the information and process the value of "P L D C S" with switch.
           9. Print out the processed turple into "mapper_output.txt"
[Reducer]
           10.Enter the reducer, use "sem_wait(&full[thread_id]) sem_wait(&mutex)" to lock the region.If the buffer[thread_id] is empty and end of input file, then finish, exit.
	   11.Else if buffer[thread_id] is not full and the input file is not empty, thread waits with code" sem_post(&empty)" and also release the lock "sem_post(&mutex)"
           12.Else process the turples and combiner them."buffer_num--". This is important!.Then we need to pass to the next reducer with "sem_post(&full[thread_id+1])".
[Main] 
	   13.Join all of the threads to wait for the ending of process and print out the combined turples into "reducer_output.txt"
============================
Tips in MakeFile:
----------------------------
           1. Since the code should be complied in C99 standard, so i use the -std=c99 in MakeFile to make sure the "For" loop will not make mistake.
           2. We need to add "-plthread" after the object since the object will use the pthread library.
============================
Bugs:
----------------------------
	   1. In this assignment, when i create the thread in Main process, i made a fault that "pthread_create(&threads[t],NULL,reducer,(void *)&t);"  the symbol"&" is wrong, when passsing the thread_id.This will effect the value of thread_id and the thread will run randomly.
============================

