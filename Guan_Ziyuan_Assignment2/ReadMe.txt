============================
=   Name: Ziyuan Guan      =
=   UFID: 94722121         =
============================
=   Assignment <2>         =
============================
Files Contained:
----------------------------
1.combiner.c
2.MakeFile
3.input.txt
4.mapper_output.txt
5.reducer_output.txt
============================
How To Run The Program:
----------------------------
Step1: Open the terminal by shortcut[ctrl+T]
Step2: Enter the destinated directory by command[cd]
Step3: Type in the command[make]
Step4: Type in the execution command[./combiner 10 7 input.txt]
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
Visions:
-----------------------------
Code Version 1:
-----------------------------
           1. Create a 2-dimension array called "buffer" and try to save all of the information.The first variable control the buffer number (the thread index as well), the second variable controls the length of slots(For Example, 10 slots, then the length should less than 260).
	   2. Store the name, subject, score information into "buffer" with every single char. The code failed because the end signal is not assigned and the contents of "buffer" is not stable. 
	   3. Try with the function"strncmp", most of the contents will be right, however, it's complex to control the range of the information needed to store.The final results of code is correct but not complete.

Code Version 2:
------------------------------
           1. Using the struct to store all of the turples. Then we should only create a one dimensional array and the results of reading is perfect. Trying to process and print out in reducer process.
           2. However, it's complex to complete the process of reducer, since the length of slots is limited, it will cause the problem after entering the thread. The turple who has the same subject may be contained in two separated slots, and then they can not be processed completely. The results are correct but not complete.
Final Code:
------------------------------
	   1. Using the struct to store all of the turples and apply the "reducer" thread to process the data but doesn't print out. It means, the turples left in the slot will be combined again in the next round.For axample, the first slot is full, there are two turples whose subject are different from the rest. So they wil be left in the slot and then the "mapper" thread will continue add the new turples into the slot.Then the "reducer" thread will process again until the end of file.
	   2. I put the print out process in the "main" thread after all of the slots have been processed and combined.
	   3. The condition variables that i applied are "&empty" and "&full".And just like the example code, i used a lock.
============================
Analysis:
----------------------------
[Steps]:
           1. Firstly, we need to create the threads in "main". Create two structs globally. Fist one is the buffer_data, contains the information of "name";"subject";"score";"buffer_number". Second one is the thread_data, contains the information of "thread_id";"thread_name", which are used to be compared with the information of turple.
           2. Create an integer globally array called "buffer_num" which is used to store the num of slots in each buffer. Create an integer globally called "ticket" to control the running order of different "reducer" threads.
           3. Then in each thread, we need to have a outer loop "while(1)" to keep thread running.
[Mapper]
           4. I used the "while(fgets())" to control that the file is end or not to break the loop.
           5. Read the turple and store the separated information into different parts. 
	   6. Go through all of the buffer and judge the value of "buffer_num", if equals the "Num_Slots". "mapper"thread waits. And then the "ticket" equals the vaule of the index of full buffer.
           7. If all of the buffer is not full, continue, read and store the information and process the value of "P L D C S" with switch.
           8. Print out the processed turple into "mapper_output.txt"
[Reducer]
           9. Use the "ticket" to control the process order.if "ticket==thread_id", then enter the thread.
           10.If the buffer[thread_id] is empty and end of input file, then finish, exit.
	   11.Else if buffer[thread_id] is not full and the input file is not empty, thread waits
           12.Else process the turples and combiner them."buffer_num--". This is important!."ticket++", Then we pass the running order to the next thread.
[Main] 
	   13.Join all of the threads to wait for the ending of process and print out the combined turples into "reducer_output.txt"
============================
Tips in MakeFile:
----------------------------
           1. Since the code should be complied in C99 standard, so i use the -std=c99 in MakeFile to make sure the "For" loop will not make mistake.
           2. We need to add "-plthread" after the object since the object will use the pthread library.
============================
Questions:
           [1] How to use the function"scanf" to control the format of input fils and assign to arguments? I tried this function for 3 days but failed.
