============================
=   Name: Ziyuan Guan      =
=   UFID: 94722121         =
============================
=   Assignment <1>         =
============================
Files Contained:
----------------------------
1.mapper.c
2.redducer.c
3.combiner.c (main program and create the Makefile)
4.input.txt
5.mapper_output.txt(created by mapper.c)
5.reducer_output.txt(created by reducer.c)
=============================
How To Run The Program:
----------------------------
Step1: Open the terminal by shortcut[ctrl+T]
Step2: Enter the destinated directory by command[cd]
Step3: Type in the command[make]
Step4: Type in the execution command[./combiner]
Step5: Check the files:
           1. MakeFile for combiner(named as "makefile" in the combiner folder)
           2. MakeFile for mapper(namend as "makefile" in the mapper folder)
           3. MakeFile for reducer(named as "makefile" in the reducer folder)
           4. Output Txt File "mapper_output.txt"
           5. Output Txt File "reducer_output.txt"
           6. Object Files
           7. Execution Files
=============================
Result:
-----------------------------
1.Fully Satisfy The Requests.
2.Solve The Bugs
=============================
Detailed Analysis:
-----------------------------
[1]mapper.c:
          1.Open the file "input.txt" with function(fopen)
          2.Save the contents in the Char array str[26] because each line is 25 digits
          3.The name of each element is listed in the range from 1 to 4
          4.The score of each element is listed on the location 6
          5.The subject of each eelement is listed in the range from 8 to 22
          6.Map the P\L\C\D\S with integer by applying (switch)
          7.Then print the new contents into "mapper_output.txt" with function(fputs)
[2]reducer.c:
          1.Read the "mapper_output.txt" with function(fopen)
          2.As same, read and save the contents by their line index into three arrays[name\subject\score].
          3.Create two levels of loop,in the outer loop, pick out one of the lines indexed by "i"
          4.Pick out another line from the lines following the "i" line and mark it with index "j"
          5.Compare the "name" and "subject" between "i" and "j" lines. If they are same, we will add the score of "j" with "i"
          6.The calculation process can be devided into three steps:
                            (1) divide the "score" into three parts[sign\tens\unit]
                            (2) summary them with formula: 
                                        sign[i]*(10*tens[i]+unit[i])+sign[j]*(10*tens[j]+unit[j])
                            (3) store the sum into "i"'s score and mark "j"'s name with 'X'.
          7.Then loop continues. The lines which are already registed will not be used again.
          8.Print the lines whose name has not been marked with'X' into file: "reducer_output.txt"with function(fputs)
[3]combiner.c:
          1.Create one pipe and one child, duplicate the write end of pipe.
          2.Create the MakeFile of "mapper" by ("cc -o mapper ./mapper.c")
          3.Execute the "mapper" by("mapper","./mapper",NULL)
          4.Create another child and duplicate the read end of pipe.
          5.Create the MakeFile of "reducer" by ("cc -o reducer ./reducer.c")
          6.Execute the "reducer"by("reducer","./reducer",NULL)
[4]makefile:
          1.Create the object such as combiner.o
          2.Add the command for the object :cc combiner.o -o combiner
          3.Add the command for the source file combiner.c
          4.Remove the object
==============================
BUGS & SOLUTIONS:
[1]Segmentation Fault: Created by the sentence "while(strlen(fgets(str,26,fp1))>0)" 
   Solution: Change it into "while(fgets(str,26,fp1))"because the stack is overflow
[2]Reducer.c has no directory
   Solution: Add a wait between two children. The second process will begin after the first has finished
[3]Print the messy code
   Solution: Add the '\0' at the end of the char array. 
[4]With using "GCC", the output file can be changed sometimes and re-execute the MakeFile, the output will recover.
   Solution: We need to delete all of the files when we need to re-execute the MakeFile. And also I have reinstalled the GCC   in the Linux System.
==============================
Questions:
[1] Why we cannot print out in the child's process. is it because we have close the file descriptor?



 
