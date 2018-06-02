# CNC
CNC-Machine on STM32 Nucleo F3.

The communication interface between the machine and the user is the computer via an executable application.

## PRINCIPLE OF OPERATION

The operator connects the CNC machine with the computer, so the user could open the executable previously installed on the PC.

The program offers a workspace equivalent to 1 pixel (according to the step motors) per millimeter of the actual workspace.

The program offers you to draw simple geometric shapes (Lines / Circles / Set of points ...) that will be translated later by the movement of the element carried by the head of the CNC machine.

After the schema is complete, the program proceeds to translate the drawing into a path. This path will be transmitted by a USB cable.

The program included in the microcontroller decrypts the path of the executable. The head then starts moving.

